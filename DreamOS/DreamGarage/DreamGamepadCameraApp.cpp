#include "DreamGamepadCameraApp.h"

#include "DreamOS.h"
#include "Core/Utilities.h"

#include "PhysicsEngine/ForceGeneratorFactory.h"
#include "PhysicsEngine/AirResistanceGenerator.h"

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
	
	AirResistanceGenerator* pAirResistanceForceGenerator = dynamic_cast<AirResistanceGenerator*>(ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_AIR_RESISTANCE));
	CN(pAirResistanceForceGenerator);
	m_pForceGenerators.emplace_back(pAirResistanceForceGenerator);

Error:
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
		m_fFirstRun = true;
	}
	else {
		if (m_fFirstRun) {
			m_msTimeLastUpdated = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
			m_fFirstRun = false;
		}

		double msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		float msTimeStep = msTimeNow - m_msTimeLastUpdated;
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
		
		if (m_pCamera->GetVelocity().magnitude() < 1.0f) {
			m_pCamera->Impulse(m_pCamera->GetRightVector() * (m_leftStick(0, 0) / m_cameraMoveSpeedScale));
			m_pCamera->Impulse(m_pCamera->GetLookVector() * (m_leftStick(0, 1) / m_cameraMoveSpeedScale));
			m_pCamera->Impulse(m_pCamera->GetUpVector() * (totalTriggerValue / m_cameraUpSpeedScale));
		}
		m_pCamera->RotateCameraByDiffXY(m_rightStick(0,0) / m_cameraRotateSpeed, -m_rightStick(0,1) / m_cameraRotateSpeed);

		m_pCamera->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, m_pForceGenerators);

		if (m_pCamera->GetVelocity().magnitude() < 0.001 && m_leftStick(0,0) < 1.5 && m_leftStick(0,0) > -1.5 && m_leftStick(0, 1) < 1.5 && m_leftStick(0, 1) > -1.5) {
			// could add a harder deceleration curve here, or set a min on the resistance value, but just setting 0 lol
			m_pCamera->SetVelocity(0,0,0);
		}
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
				m_cameraMoveSpeedScale -= 10.0f;
			}
			else if (m_rightStick(0, 0) > 0.0f || m_rightStick(0,1) > 0.0f) {
				m_cameraRotateSpeed -= 1.0f;
			}
			else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
				m_cameraUpSpeedScale -= 100.0f;
			}
			DEBUG_LINEOUT("DPAD UP");
		}
		else if (pEvent->gamepadButtonType == SENSE_GAMEPAD_DPAD_DOWN) {
			if (m_leftStick(0, 0) > 0.0f || m_leftStick(0, 1) > 0.0f) {
				m_cameraMoveSpeedScale += 10.0f;
			}
			else if (m_rightStick(0, 0) > 0.0f || m_rightStick(0, 1) > 0.0f) {
				m_cameraRotateSpeed += 1.0f;
			}
			else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
				m_cameraUpSpeedScale += 100.0f;
			}
			DEBUG_LINEOUT("DPAD DOWN");
		}
	} break;

	}

//Error:
	return r;
}
