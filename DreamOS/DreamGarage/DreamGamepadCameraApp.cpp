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
		//m_pCamera->SetMass(1.0);
		m_fFirstRun = true;
	}
	else {
		if (m_fFirstRun) {
			m_msTimeLastUpdated = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			m_fFirstRun = false;
		}

		double msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		double msTimeStep = msTimeNow - m_msTimeLastUpdated;
		m_msTimeLastUpdated = msTimeNow;
		
		if (m_fUpdateLeftStick) {
			m_leftStick(0,0) = 10 * m_pendLeftStick(0,0);
			m_leftStick(0, 1) = 10 * m_pendLeftStick(0, 1);
		}
		if (m_fUpdateRightStick) {
			m_rightStick(0, 0) = 100 * m_pendRightStick(0, 0);
			m_rightStick(0, 1) = 100 * m_pendRightStick(0, 1);
		}
		if (m_fUpdateLeftTrigger) {
			m_leftTriggerValue = m_pendLeftTriggerValue;
		}
		if (m_fUpdateRightTrigger) {
			m_rightTriggerValue = m_pendRightTriggerValue;
		}
		float totalTriggerValue = (m_leftTriggerValue + m_rightTriggerValue);

		// X
		// moving right
		if (m_leftStick(0, 0) > 1.5) {
			m_xVelocity += msTimeStep / (200.0 / m_leftStick(0, 0));	// between 100 and 500 feels alright
		}
		// moving left
		else if (m_leftStick(0,0) < -1.5) {
			m_xVelocity += msTimeStep / (200.0 / m_leftStick(0, 0));
		}
		else {
			if (m_xVelocity < 0) {
				m_xVelocity += msTimeStep / 100.0;
			}
			else if(m_xVelocity > 0) {
				m_xVelocity -= msTimeStep / 100.0f;
			}
		}

		// Z
		// moving forward
		if (m_leftStick(0, 1) > 1.5) {
			m_zVelocity += msTimeStep / (200.0 / m_leftStick(0, 1));
		}
		// moving backward
		else if (m_leftStick(0, 1) < -1.5) {
			m_zVelocity += msTimeStep / (200.0 / m_leftStick(0, 1));
		}
		else {
			if (m_zVelocity < 0) {
				m_zVelocity += msTimeStep / 100.0;
			}
			else if(m_zVelocity > 0) {
				m_zVelocity -= msTimeStep / 100.0f;
			}
		}

		// Y
		// moving Up
		if (totalTriggerValue > 0.01) {
			m_yVelocity += msTimeStep / (200.0 / totalTriggerValue);
		}
		// moving down
		else if (totalTriggerValue < -0.01) {
			m_yVelocity += msTimeStep / (200.0 / totalTriggerValue);
		}
		else {
			if (m_yVelocity < 0) {
				m_yVelocity += msTimeStep / 1000.0f;
			}
			else if (m_yVelocity > 0) {
				m_yVelocity -= msTimeStep / 1000.0f;
			}
		}

		// Rotation
		// looking left
		if (m_rightStick(0, 0) > 15.0) {
			m_lookXVelocity += msTimeStep / (200.0 / m_rightStick(0, 0));	// between 100 and 500 feels alright
		}
		// looking right
		else if (m_rightStick(0, 0) < -15.0) {
			m_lookXVelocity += msTimeStep / (200.0 / m_rightStick(0, 0));
		}
		else {
			if (m_lookXVelocity < 0) {
				m_lookXVelocity += msTimeStep / 10.0;
			}
			else if (m_lookXVelocity > 0) {
				m_lookXVelocity -= msTimeStep / 10.0f;
			}
		}
		// looking up
		if (m_rightStick(0, 1) > 15.0) {
			m_lookYVelocity += msTimeStep / (200.0 / m_rightStick(0, 1));
		}
		// looking down
		else if (m_rightStick(0, 1) < -15.0) {
			m_lookYVelocity += msTimeStep / (200.0 / m_rightStick(0, 1));
		}
		else {
			if (m_lookYVelocity < 0) {
				m_lookYVelocity += msTimeStep / 10.0;
			}
			else if (m_lookYVelocity > 0) {
				m_lookYVelocity -= msTimeStep / 10.0f;
			}
		}

		// cutoffs
		// Maxs
		if (m_xVelocity > m_xzMax || m_xVelocity < -m_xzMax) {
			util::Clamp(m_xVelocity, -m_xzMax, m_xzMax);
		}
		if (m_zVelocity > m_xzMax || m_zVelocity < -m_xzMax) {
			util::Clamp(m_zVelocity, -m_xzMax, m_xzMax);
		}
		if (m_yVelocity > m_yMax || m_yVelocity < -m_yMax) {
			util::Clamp(m_yVelocity, -m_yMax, m_yMax);
		}
		if (m_lookXVelocity > m_lookMaxVelocity || m_lookXVelocity < -m_lookMaxVelocity) {
			util::Clamp(m_lookXVelocity, -m_lookMaxVelocity, m_lookMaxVelocity);
		}
		if (m_lookYVelocity > m_lookMaxVelocity || m_lookYVelocity < -m_lookMaxVelocity) {
			util::Clamp(m_lookYVelocity, -m_lookMaxVelocity, m_lookMaxVelocity);
		}


		// Mins
		if (m_xVelocity < 0.001 && m_xVelocity > -0.001) {
			m_xVelocity = 0.0f;
		}
		if (m_zVelocity < 0.001 && m_zVelocity > -0.001) {
			m_zVelocity = 0.0f;
		}
		if (m_yVelocity < 0.001 && m_yVelocity > -0.001) {
			m_yVelocity = 0.0f;
		}
		if (m_lookXVelocity < 0.001 && m_lookXVelocity > -0.001) {
			m_lookXVelocity = 0.0f;
		}
		if (m_lookYVelocity < 0.001 && m_lookYVelocity > -0.001) {
			m_lookYVelocity = 0.0f;
		}

		m_pCamera->MoveStrafe(m_xVelocity / m_cameraStrafeSpeed);
		if (m_fLockY) {
			m_pCamera->MoveLockedY(m_zVelocity / m_cameraStrafeSpeed);
		}
		else {
			m_pCamera->MoveForward(m_zVelocity / m_cameraStrafeSpeed);
		}
		m_pCamera->RotateCameraByDiffXY(m_lookXVelocity / m_cameraRotateSpeed, -m_lookYVelocity / m_cameraRotateSpeed);

		m_pCamera->MoveUp(m_yVelocity / m_cameraUpScale);
		DEBUG_LINEOUT("Camera moving: vel:%0.8f stick:%0.8f", (double)m_lookXVelocity, (double)m_rightStick(0,0));	
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
		m_fUpdateRightStick = true;
		m_pendRightStick = pEvent->eventData;		
	} break;

	case SENSE_GAMEPAD_TRIGGER_LEFT: {
		m_fUpdateLeftTrigger = true;
		m_pendLeftTriggerValue = -(pEvent->eventData(0,0));
	} break;

	case SENSE_GAMEPAD_TRIGGER_RIGHT: {
		m_fUpdateRightTrigger = true;
		m_pendRightTriggerValue = pEvent->eventData(0, 0);
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
				m_cameraRotateSpeed -= 5.0f;
			}
			else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
				m_cameraUpScale -= 100.0f;
			}
			DEBUG_LINEOUT("DPAD UP");
		}
		else if (pEvent->gamepadButtonType == SENSE_GAMEPAD_DPAD_DOWN) {
			if (m_leftStick(0, 0) > 0.0f || m_leftStick(0, 1) > 0.0f) {
				m_cameraStrafeSpeed += 5.0f;
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
