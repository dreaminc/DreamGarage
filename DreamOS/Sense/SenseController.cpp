#include "SenseController.h"
#include "DreamConsole/DreamConsole.h"

SenseController::SenseController()
{
	for (int i = 0; i < SENSE_CONTROLLER_INVALID; i++) {
		RegisterEvent((SenseControllerEventType)(i));
	}
}

SenseController::~SenseController()
{
}

ControllerState SenseController::GetControllerState(ControllerType type) {
	return m_controllerStates[type];
}

RESULT SenseController::SetControllerState(ControllerState controllerState) {

	ControllerState* currentState = &m_controllerStates[controllerState.type];

	SENSE_CONTROLLER_EVENT_TYPE eventType;

	if (currentState->triggerRange != controllerState.triggerRange) {
		eventType = SENSE_CONTROLLER_TRIGGER_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
		if (controllerState.triggerRange == 1.0f) {
			eventType = SENSE_CONTROLLER_TRIGGER_DOWN;
			NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
		}
		if (controllerState.triggerRange != 1.0f && currentState->triggerRange == 1.0f) {
			eventType = SENSE_CONTROLLER_TRIGGER_UP;
			NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
		}
	}
	currentState->triggerRange = controllerState.triggerRange;


	if (currentState->ptTouchpad != controllerState.ptTouchpad) {
		eventType = SENSE_CONTROLLER_PAD_MOVE;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	currentState->ptTouchpad = controllerState.ptTouchpad;


	if (!currentState->fGrip && controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (currentState->fGrip && !controllerState.fGrip) {
		eventType = SENSE_CONTROLLER_GRIP_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	currentState->fGrip = controllerState.fGrip;


	if (!currentState->fMenu && controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_DOWN;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
	}
	else if (currentState->fMenu && !controllerState.fMenu) {
		eventType = SENSE_CONTROLLER_MENU_UP;
		NotifySubscribers(eventType, &SenseControllerEvent(eventType, controllerState));
		//OVERLAY_DEBUG_SET("menuUp", )
	}
	currentState->fMenu = controllerState.fMenu;


	return R_PASS;
}
