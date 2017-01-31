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
	return m_controllerState;
}

RESULT SenseController::SetControllerState(ControllerState controllerState) {
	
	m_controllerState.type = controllerState.type;

	SENSE_CONTROLLER_EVENT_TYPE eventType;

	if (m_controllerState.triggerRange != controllerState.triggerRange) {
		eventType = SENSE_CONTROLLER_TRIGGER_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_controllerState.triggerRange = controllerState.triggerRange;


	if (m_controllerState.ptTouchpad != controllerState.ptTouchpad) {
		eventType = SENSE_CONTROLLER_PAD_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_controllerState.ptTouchpad = controllerState.ptTouchpad;


	if (!m_controllerState.fGrip && controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (m_controllerState.fGrip && !controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_controllerState.fGrip = controllerState.fGrip;


	if (!m_controllerState.fMenu && controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (m_controllerState.fMenu && !controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	m_controllerState.fMenu = controllerState.fMenu;


	return R_PASS;
}
