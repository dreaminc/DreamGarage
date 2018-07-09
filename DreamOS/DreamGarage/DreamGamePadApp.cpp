#include "DreamGamePadApp.h"

#include "DreamOS.h"

DreamGamePadApp::DreamGamePadApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamGamePadApp>(pDreamOS, pContext)
{
	// empty
}

DreamGamePadApp::~DreamGamePadApp()
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

DreamGamePadApp* DreamGamePadApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamGamePadApp *pDreamApp = new DreamGamePadApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamGamePadApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamGamePadApp");
	SetAppDescription("A Dream App for using game pad to control the camera");

	for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
		GetDOS()->RegisterSubscriber((SenseGamePadEventType)(i), this);
	}

	return r;
}

RESULT DreamGamePadApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamGamePadApp::Shutdown(void *pContext) {
	return R_PASS;
}

RESULT DreamGamePadApp::Update(void *pContext) {
	RESULT r = R_PASS;

	GetDOS()->GetCamera()->MoveStrafe(m_ptLeftStick.x());
	GetDOS()->GetCamera()->MoveForward(m_ptLeftStick.y());
	GetDOS()->GetCamera()->RotateCameraByDiffXY(m_ptRightStick.x(), -m_ptRightStick.y());

	GetDOS()->GetCamera()->MoveUp(m_trigger);

//Error:
	return r;
}

RESULT DreamGamePadApp::Notify(SenseGamePadEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->eventType) {
	case SENSE_GAMEPAD_LEFTSTICK: {
		m_ptLeftStick = pEvent->gamepadState.ptJoyStick / GAMEPAD_STRAFESPEED_SCALE;
	} break;

	case SENSE_GAMEPAD_RIGHTSTICK: {
		m_ptRightStick = pEvent->gamepadState.ptJoyStick / GAMEPAD_CAMERA_ROTATE_SCALE;
	} break;

	case SENSE_GAMEPAD_BUTTONS: {
		GetDOS()->GetCamera()->SetPosition(0, 0, 0);
		GetDOS()->GetCamera()->SetForwardSpeed(0.0f);
		GetDOS()->GetCamera()->SetStrafeSpeed(0.0f);
		GetDOS()->GetCamera()->SetUpSpeed(0.0f);
	} break;

	case SENSE_GAMEPAD_TRIGGER_LEFT: {
		m_trigger = -(pEvent->gamepadState.triggerRange / GAMEPAD_CAMERA_UP_SCALE);
	} break;

	case SENSE_GAMEPAD_TRIGGER_RIGHT: {
		m_trigger = pEvent->gamepadState.triggerRange / GAMEPAD_CAMERA_UP_SCALE;
	} break;

	default: {
	}
	}

//Error:
	return r;
}