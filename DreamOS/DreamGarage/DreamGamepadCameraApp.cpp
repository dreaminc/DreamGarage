#include "DreamGamepadCameraApp.h"

#include "DreamOS.h"
#include "Core/Utilities.h"

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
	
	if (m_pCamera == nullptr) {
		m_pCamera = GetDOS()->GetCamera();
		m_pCamera->SetMass(1.0);
		m_msTimeLastUpdated = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}
	else {
		double msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		double msTimeStep = msTimeNow - m_msTimeLastUpdated;
		m_msTimeLastUpdated = msTimeNow;
		
		if (m_fUpdateLeftStick) {
			m_leftStick = m_pendLeftStick;
		}
		// X
		// moving right
		if (m_leftStick(0, 0) > 0.15) {
			m_xVelocity += msTimeStep / (100.0 / m_leftStick(0, 0));	// between 100 and 500 feels alright
		}
		// moving left
		else if (m_leftStick(0,0) < -0.15) {
			m_xVelocity += msTimeStep / (100.0 / m_leftStick(0, 0));
		}
		else {
			if (m_xVelocity < 0) {
				m_xVelocity += msTimeStep / 200.0;
			}
			else {
				m_xVelocity -= msTimeStep / 200.0f;
			}
		}

		// cutoffs
		if (m_xVelocity < 0.001 && m_xVelocity > -0.001) {
			m_xVelocity = 0.0f;
		}

		m_pCamera->MoveStrafe(m_xVelocity / m_cameraStrafeSpeed);
		m_pCamera->MoveForward(m_leftStick(0, 1) / m_cameraStrafeSpeed);
		m_pCamera->RotateCameraByDiffXY(m_rightStick(0, 0), -m_rightStick(0, 1));

		m_pCamera->MoveUp(m_leftTriggerValue + m_rightTriggerValue);
		DEBUG_LINEOUT_RETURN("Camera moving: vel:%0.8f stick:%0.8f", (double)m_xVelocity, (double)m_leftStick(0, 0));	
	}

Error:
	return r;
}

RESULT DreamGamepadCameraApp::Notify(SenseGamepadEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->gamepadEventType) {
	case SENSE_GAMEPAD_JOYSTICK_LEFT: {
		m_fUpdateLeftStick = true;
		m_pendLeftStick = pEvent->eventData;
		
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

	case SENSE_GAMEPAD_BUTTON_DOWN: {
		if (pEvent->gamepadButtonType == SENSE_GAMEPAD_SHOULDER_RIGHT) {
			m_fLockY = true;
		}
	} break;

	case SENSE_GAMEPAD_BUTTON_UP: {
		if (pEvent->gamepadButtonType == SENSE_GAMEPAD_SHOULDER_RIGHT) {
			m_fLockY = false;	
		}

		if (pEvent->gamepadButtonType == SENSE_GAMEPAD_DPAD_UP) {
			if (m_leftStick(0, 0) > 0.0f || m_leftStick(0,1) > 0.0f) {
				m_cameraStrafeSpeed -= 10.0f;
			}
			else if (m_rightStick(0, 0) > 0.0f || m_rightStick(0,1) > 0.0f) {
				m_cameraRotateSpeed -= 0.05f;
			}
			else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
				m_cameraUpScale -= 100.0f;
			}
			DEBUG_LINEOUT("DPAD UP");
		}
		else if (pEvent->gamepadButtonType == SENSE_GAMEPAD_DPAD_DOWN) {
			if (m_leftStick(0, 0) > 0.0f || m_leftStick(0, 1) > 0.0f) {
				m_cameraStrafeSpeed += 10.0f;
			}
			else if (m_rightStick(0, 0) > 0.0f || m_rightStick(0, 1) > 0.0f) {
				m_cameraRotateSpeed += 0.05f;
			}
			else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
				m_cameraUpScale += 100.0f;
			}
			DEBUG_LINEOUT("DPAD DOWN");
		}
	} break;

	}

//Error:
	return r;
}
