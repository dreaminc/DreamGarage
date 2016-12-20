#include "SenseController.h"

SenseController::SenseController()
{
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
	m_trigger = controllerState.trigger;
	m_touchpad = controllerState.ptTouchpad;
	m_grip = controllerState.fGrip;
	m_menu = controllerState.fMenu;

	return R_PASS;
}
