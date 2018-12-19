#include "DreamGamepadCameraApp.h"

#include "DreamOS.h"
#include "Core/Utilities.h"

#include "Primitives/camera.h"

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

	AirResistanceGenerator* pAirResistanceForceGenerator = dynamic_cast<AirResistanceGenerator*>(ForceGeneratorFactory::MakeForceGenerator(FORCE_GENERATOR_AIR_RESISTANCE));
	CN(pAirResistanceForceGenerator);
	m_pForceGenerators.emplace_back(pAirResistanceForceGenerator);

Error:
	return r;
}

RESULT DreamGamepadCameraApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	// empty

Error:
	return r;
}

RESULT DreamGamepadCameraApp::SetCamera(camera *pCamera, CameraControlType controlType) {
	RESULT r = R_PASS;

	CN(pCamera);
	m_pCamera = pCamera;
	
	CBR(m_controlType != controlType, R_SKIPPED);	// if already that control type then skip 
	CR(UnregisterFromEvents());

	if (controlType == CameraControlType::GAMEPAD) {
		for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
			CR(GetDOS()->RegisterSubscriber((SenseGamepadEventType)(i), this));
		}
	}
	else if (controlType == CameraControlType::SENSECONTROLLER) {
		for (int i = 0; i < SENSE_CONTROLLER_INVALID; i++) {
			CR(GetDOS()->RegisterSubscriber((SenseControllerEventType)(i), this));
		}
	}

	m_controlType = controlType;

Error:
	return r;
}

RESULT DreamGamepadCameraApp::UnregisterFromEvents() {
	RESULT r = R_PASS;

	if (m_controlType == CameraControlType::GAMEPAD) {
		for (int i = 0; i < SENSE_GAMEPAD_INVALID; i++) {
			CR(GetDOS()->UnregisterSubscriber((SenseGamepadEventType)(i), this));
		}
	}
	else if (m_controlType == CameraControlType::SENSECONTROLLER) {
		for (int i = 0; i < SENSE_CONTROLLER_INVALID; i++) {
			CR(GetDOS()->UnregisterSubscriber((SenseControllerEventType)(i), this));
		}
	}

	m_controlType = CameraControlType::INVALID;

Error:
	return r;
}

RESULT DreamGamepadCameraApp::RegisterGamepadCameraObserver(DreamGamepadCameraApp::observer *pObserver) {
	RESULT r = R_PASS;

	CBM(m_pObserver == nullptr, "DreamGamepadCameraApp already has an observer");
	m_pObserver = pObserver;

Error:
	return r;
}

DreamGamepadCameraApp::CameraControlType DreamGamepadCameraApp::GetCameraControlType() {
	return m_controlType;
}


RESULT DreamGamepadCameraApp::Shutdown(void *pContext) {
	return R_PASS;
}

RESULT DreamGamepadCameraApp::Update(void *pContext) {
	RESULT r = R_PASS;
	
	static bool fFirstRun = false; 
	if (fFirstRun) {
		m_msTimeLastUpdated = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
		fFirstRun = false;
	}

	double msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	float msTimeStep = msTimeNow - m_msTimeLastUpdated;
	m_msTimeLastUpdated = msTimeNow;
		
	if (m_fUpdateLeftStick) {
		m_ptLeftStick.x() = m_ptPendLeftStick.x();
		m_ptLeftStick.y() = m_ptPendLeftStick.y();
	}

	if (m_fUpdateRightStick) {
		if (abs(m_ptPendRightStick.x()) > abs(m_ptPendRightStick.y())) {
			m_ptRightStick.x() = m_ptPendRightStick.x();
			m_ptRightStick.y() = 0.0f;
		}
		else {
			m_ptRightStick.y() = m_ptPendRightStick.y();
			m_ptRightStick.x() = 0.0f;
		}
	}
	
	if (m_fUpdateLeftTrigger) {
		m_leftTriggerValue = m_pendLeftTriggerValue;
	}
	
	if (m_fUpdateRightTrigger) {
		m_rightTriggerValue = m_pendRightTriggerValue;
	}

	float totalTriggerValue = (m_leftTriggerValue + m_rightTriggerValue);
		
	m_pCamera->Impulse(m_pCamera->GetRightVector() * (m_ptLeftStick.x() / m_cameraMoveSpeedScale));
	m_pCamera->Impulse(m_pCamera->GetUpVector() * (totalTriggerValue / m_cameraUpSpeedScale));

	if (m_fLockY) {
		vector vLookVectorXZ = vector(m_pCamera->GetLookVector().x(), 0, m_pCamera->GetLookVector().z());
		m_pCamera->Impulse(vLookVectorXZ * (m_ptLeftStick.y() / m_cameraMoveSpeedScale));
	}
	else {
		m_pCamera->Impulse(m_pCamera->GetLookVector() * (m_ptLeftStick.y() / m_cameraMoveSpeedScale));
	}

	//* variable rotation
	// Rotation
	// looking left
	if (m_ptRightStick.x() > 0.15) {
		m_lookXVelocity += msTimeStep / (200.0 / m_ptRightStick.x());	// between 100 and 500 feels alright
	}
	// looking right
	else if (m_ptRightStick.x() < -0.15) {
		m_lookXVelocity += msTimeStep / (200.0 / m_ptRightStick.x());
	}
	else {
		if (m_lookXVelocity < 0) {
			m_lookXVelocity += msTimeStep / 500.0;
		}
		else if (m_lookXVelocity > 0) {
			m_lookXVelocity -= msTimeStep / 500.0f;
		}
	}
	// looking up
	if (m_ptRightStick.y() > 0.1) {
		m_lookYVelocity += msTimeStep / (200.0 / m_ptRightStick.y());
	}
	// looking down
	else if (m_ptRightStick.y() < -0.1) {
		m_lookYVelocity += msTimeStep / (200.0 / m_ptRightStick.y());
	}
	else {
		if (m_lookYVelocity < 0) {
			m_lookYVelocity += msTimeStep / 500.0;
		}
		else if (m_lookYVelocity > 0) {
			m_lookYVelocity -= msTimeStep / 500.0f;
		}
	}
	
	if (m_lookXVelocity > 1.0 || m_lookXVelocity < -1.0) {
		util::Clamp(m_lookXVelocity, -1.0f, 1.0f);
	}
	if (m_lookYVelocity > 1.0 || m_lookYVelocity < -1.0) {
		util::Clamp(m_lookYVelocity, -1.0f, 1.0f);
	}

	if (m_lookXVelocity < 0.01 && m_lookXVelocity > -0.01) {
		m_lookXVelocity = 0.0f;
	}
	if (m_lookYVelocity < 0.01 && m_lookYVelocity > -0.01) {
		m_lookYVelocity = 0.0f;
	}
	//DEBUG_LINEOUT_RETURN("Camera Rotating: x: %0.8f y: %0.8f", m_lookXVelocity, m_lookYVelocity);
	
	bool fAtRest = (m_lookXVelocity == 0.0f && m_lookYVelocity == 0.0f && m_pCamera->GetMomentum().magnitude() < 0.00000001);	// because double precision things
	if (!fAtRest) {
		m_fAtRest = false;
	}
	else if (!m_fAtRest && fAtRest) {
		//DEBUG_LINEOUT("Camera at rest at x: %0.2f y: %0.2f z: %0.2f", m_pCamera->GetPosition().x(), m_pCamera->GetPosition().y(), m_pCamera->GetPosition().z());
		m_fAtRest = true;
		if (m_pObserver != nullptr) {
			m_pObserver->OnCameraMoved();
		}
	}

	//*/

	if (m_pCamera != nullptr) {
		//	non-variable rotation
		//m_pCamera->RotateCameraByDiffXY(m_ptRightStick.x() / m_cameraRotateSpeed, -m_ptRightStick.y() / m_cameraRotateSpeed);
		m_pCamera->RotateCameraByDiffXY(m_lookXVelocity / m_cameraRotateSpeed, -m_lookYVelocity / m_cameraRotateSpeed);
		m_pCamera->IntegrateState<ObjectState::IntegrationType::RK4>(0.0f, msTimeStep, m_pForceGenerators);
	}

	//DEBUG_LINEOUT_RETURN("vel mag: %0.8f", m_pCamera->GetVelocity().magnitude());
	//DEBUG_LINEOUT_RETURN("Velocity: x: %0.8f y: %0.8f z: %0.8f", m_pCamera->GetVelocity().x(), m_pCamera->GetVelocity().y(), m_pCamera->GetVelocity().z());


Error:
	return r;
}

RESULT DreamGamepadCameraApp::Notify(SenseGamepadEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->gamepadEventType) {
		case SENSE_GAMEPAD_JOYSTICK_LEFT: {
			m_fUpdateLeftStick = true;
			m_ptPendLeftStick = pEvent->eventData;		
			//DEBUG_LINEOUT("pending x: %0.8f y: %0.8f", m_ptPendLeftStick.x(), m_ptPendLeftStick.y());
		} break;

		case SENSE_GAMEPAD_JOYSTICK_RIGHT: {
			m_fUpdateRightStick = true;
			m_ptPendRightStick = pEvent->eventData;		
		} break;

		case SENSE_GAMEPAD_TRIGGER_LEFT: {
			m_fUpdateLeftTrigger = true;
			m_pendLeftTriggerValue = -(pEvent->eventData.x());
		} break;

		case SENSE_GAMEPAD_TRIGGER_RIGHT: {
			m_fUpdateRightTrigger = true;
			m_pendRightTriggerValue = pEvent->eventData.x();
			//DEBUG_LINEOUT("pending x: %0.8f", (double)m_pendRightTriggerValue);
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
				if (m_ptLeftStick.x() > 0.0f || m_ptLeftStick.y() > 0.0f) {
					m_cameraMoveSpeedScale -= m_cameraMoveSpeedScale * 0.1;
				}
				else if (m_ptRightStick.x() > 0.0f || m_ptRightStick.y() > 0.0f) {
					m_cameraRotateSpeed -= m_cameraRotateSpeed * 0.1;
				}
				else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
					m_cameraUpSpeedScale -= m_cameraUpSpeedScale * 0.1;
				}

				DEBUG_LINEOUT("DPAD UP");
			}
			else if (pEvent->gamepadButtonType == SENSE_GAMEPAD_DPAD_DOWN) {
				if (m_ptLeftStick.x() > 0.0f || m_ptLeftStick.y() > 0.0f) {
					m_cameraMoveSpeedScale += m_cameraMoveSpeedScale * 0.1;
				}
				else if (m_ptRightStick.x() > 0.0f || m_ptRightStick.y() > 0.0f) {
					m_cameraRotateSpeed += m_cameraRotateSpeed * 0.1;
				}
				else if (m_leftTriggerValue > 0.0f || m_rightTriggerValue > 0.0f) {
					m_cameraUpSpeedScale += m_cameraUpSpeedScale * 0.1;
				}

				DEBUG_LINEOUT("DPAD DOWN");
			}
		} break;
	}

Error:
	return r;
}

RESULT DreamGamepadCameraApp::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->type) {
	case SENSE_CONTROLLER_TRIGGER_MOVE: {
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_fUpdateLeftTrigger = true;
			m_pendLeftTriggerValue = -(pEvent->state.triggerRange) * 255;
		}
		else if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
			m_fUpdateRightTrigger = true;
			m_pendRightTriggerValue = pEvent->state.triggerRange * 255;
			//DEBUG_LINEOUT("pending x: %0.8f", (double)m_pendRightTriggerValue);
		}
	} break;
	
	case SENSE_CONTROLLER_PAD_MOVE: {
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			m_fUpdateLeftStick = true;
			m_ptPendLeftStick = point2D(pEvent->state.ptTouchpad.x(), pEvent->state.ptTouchpad.y());
			//DEBUG_LINEOUT("pending x: %0.8f y: %0.8f", m_ptPendLeftStick.x(), m_ptPendLeftStick.y());
		}
		else if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
			m_fUpdateRightStick = true;
			m_ptPendRightStick = point2D(pEvent->state.ptTouchpad.x(), pEvent->state.ptTouchpad.y());
		}
	} break;
	}

Error:
	return r;
}
