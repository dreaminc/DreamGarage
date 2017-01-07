#include "SenseController.h"

SenseController::SenseController()
{
	for (int i = 0; i < SENSE_CONTROLLER_INVALID; i++) {
		RegisterEvent((SenseControllerEventType)(i));
	}
}

SenseController::~SenseController()
{
}

ControllerState SenseController::GetControllerState() {
	ControllerState controllerState = {
		m_type,
		m_trigger,
		m_touchpad,
		m_grip,
		m_menu
	};
	return controllerState;
}

RESULT SenseController::SetControllerState(ControllerState controllerState) {
	
	m_type = controllerState.type;

	SENSE_CONTROLLER_EVENT_TYPE eventType;

	if (m_trigger != controllerState.trigger) {
		eventType = SENSE_CONTROLLER_TRIGGER_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_trigger = controllerState.trigger;


	if (m_touchpad != controllerState.ptTouchpad) {
		eventType = SENSE_CONTROLLER_PAD_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_touchpad = controllerState.ptTouchpad;


	if (!m_grip && controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (m_grip && !controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_grip = controllerState.fGrip;


	if (!m_menu && controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (m_menu && !controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_menu = controllerState.fMenu;


	return R_PASS;
}
