#include "DreamGamepadCameraApp.h"

#include "DreamOS.h"

DreamGamepadCameraApp::DreamGamepadCameraApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamGamepadCameraApp>(pDreamOS, pContext)
{
	// empty
}

DreamGamepadCameraApp::~DreamGamepadCameraApp()
{
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

DreamGamepadCameraApp* DreamGamepadCameraApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamGamepadCameraApp *pDreamApp = new DreamGamepadCameraApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamGamepadCameraApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	SetAppName("DreamGamepadCameraApp");
	SetAppDescription("A Dream App for using game pad to control the camera");

	for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
		GetDOS()->RegisterSubscriber((SenseGamepadEventType)(i), this);
	}

	return r;
}

RESULT DreamGamepadCameraApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamGamepadCameraApp::Shutdown(void *pContext) {
	return R_PASS;
}

RESULT DreamGamepadCameraApp::Update(void *pContext) {
	RESULT r = R_PASS;

	GetDOS()->GetCamera()->MoveStrafe(m_leftStick(0,0));
	GetDOS()->GetCamera()->MoveForward(m_leftStick(0,1));
	GetDOS()->GetCamera()->RotateCameraByDiffXY(m_rightStick(0,0), -m_rightStick(0,1));

	GetDOS()->GetCamera()->MoveUp(m_leftTriggerValue + m_rightTriggerValue);

//Error:
	return r;
}

RESULT DreamGamepadCameraApp::Notify(SenseGamepadEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->gamepadEventType) {
	case SENSE_GAMEPAD_JOYSTICK_LEFT: {
		m_leftStick = pEvent->eventData / m_cameraStrafeSpeed;
	} break;

	case SENSE_GAMEPAD_JOYSTICK_RIGHT: {
		m_rightStick = pEvent->eventData / m_cameraRotateSpeed;
	} break;

	case SENSE_GAMEPAD_TRIGGER_LEFT: {
		m_leftTriggerValue = -(pEvent->eventData(0,0) / m_cameraUpScale);
	} break;

	case SENSE_GAMEPAD_TRIGGER_RIGHT: {
		m_rightTriggerValue = pEvent->eventData(0,0) / m_cameraUpScale;
	} break;

	}

//Error:
	return r;
}