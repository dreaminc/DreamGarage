#include "SenseLeapMotion.h"

#include "DreamConsole/DreamConsole.h"

SenseLeapMotion::SenseLeapMotion() :
	m_pLeapController(nullptr),
	m_pLeftHand(nullptr),
	m_pRightHand(nullptr)
{
	RESULT r = R_PASS;

	m_pLeapController = std::make_unique<Leap::Controller>();
	CNM(m_pLeapController, "Failed to create leap motion controller");

	CBM((m_pLeapController->addListener(*this)), "Failed to add SenseLeapMotion as listener device");

	m_pLeapController->setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);

	OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Detected...");

	/*
	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
	RegisterEvent(i);
	}
	*/

	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::Leap, this);

//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

SenseLeapMotion::~SenseLeapMotion() {
	bool fFlag = m_pLeapController->removeListener(*this);
}

RESULT SenseLeapMotion::SetPause(bool fPauseState) {
	m_pLeapController->setPaused(fPauseState);
	return R_PASS;
}

RESULT SenseLeapMotion::Pause() {
	return SetPause(true);
}

RESULT SenseLeapMotion::Resume() {
	return SetPause(false);
}

// Leap Motion Callbacks
void SenseLeapMotion::onInit(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Initialized");
	OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Init...");
}

void SenseLeapMotion::onConnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Connected");
	OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Connected");
}

void SenseLeapMotion::onDisconnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Disconnected");
	OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Disconnected");
}

void SenseLeapMotion::onExit(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Exited");
	OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Exited");
}

void SenseLeapMotion::onFrame(const Leap::Controller&) {
	const Leap::Frame leapFrame = m_pLeapController->frame();

	//DEBUG_LINEOUT("Frame id:%d timestamp:%d hands:%d fingers:%d", leapFrame.id(), leapFrame.timestamp(), leapFrame.hands().count(), leapFrame.fingers().count());

	Leap::HandList hands = leapFrame.hands();
	quaternion baseRight = quaternion();
	baseRight *= quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)M_PI_2);

	quaternion baseLeft = quaternion();
	baseLeft *= quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, -(float)M_PI_2);
	
	bool fLeftHandTracked = false;
	bool fRightHandTracked = false;

	for (auto hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Leap::Hand hand = *hl;

		if ((hand.isLeft())) {
			if (m_pLeftHand != nullptr) {
				m_pLeftHand->SetFromLeapHand(hand);
				m_pLeftModel->SetPosition(m_pLeftHand->GetHandState().ptPalm);
				m_pLeftModel->SetOrientation(m_pLeftHand->GetHandState().qOrientation * baseLeft);

				if (!m_pLeftHand->IsVisible() && m_pLeftHand->IsOriented())
					m_pLeftModel->SetVisible(true);

				fLeftHandTracked = true;
			}
		}
		else {
			if (m_pRightHand != nullptr) {
				m_pRightHand->SetFromLeapHand(hand);
				m_pRightModel->SetPosition(m_pRightHand->GetHandState().ptPalm);
				m_pRightModel->SetOrientation(m_pRightHand->GetHandState().qOrientation * baseRight);

				if (!m_pRightHand->IsVisible() && m_pRightHand->IsOriented())
					m_pRightModel->SetVisible(true);
				fRightHandTracked = true;
			}
		}
	}
	
	if (!fLeftHandTracked) {
		if (m_pLeftHand != nullptr) {
			m_pLeftHand->OnLostTrack();
		}
		if (m_pLeftModel != nullptr) {
			m_pLeftModel->SetVisible(false);
		}
	}

	if (!fRightHandTracked) {
		if (m_pRightHand != nullptr) {
			m_pRightHand->OnLostTrack();
		}
		if (m_pRightModel != nullptr) {
			m_pRightModel->SetVisible(false);
		}
	}

}

void SenseLeapMotion::onFocusGained(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: OnFocus");
}

void SenseLeapMotion::onFocusLost(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Lost Focus");
}

void SenseLeapMotion::onDeviceChange(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Device Change");

	const Leap::DeviceList leapDevices = m_pLeapController->devices();

	for (int i = 0; i < leapDevices.count(); ++i) {
		DEBUG_LINEOUT("id: %s", leapDevices[i].toString().c_str());
		DEBUG_LINEOUT("  isStreaming: %s", (leapDevices[i].isStreaming() ? "true" : "false"));
		DEBUG_LINEOUT("  isSmudged: %s", (leapDevices[i].isSmudged() ? "true" : "false"));
		DEBUG_LINEOUT("  isLightingBad: %s", (leapDevices[i].isLightingBad() ? "true" : "false"));
	}
}

void SenseLeapMotion::onServiceConnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Service Connected");
}

void SenseLeapMotion::onServiceDisconnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Service Disconnected");
}

void SenseLeapMotion::onServiceChange(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Service Changed");
}

void SenseLeapMotion::onDeviceFailure(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Device Error");

	const Leap::FailedDeviceList leapDevices = m_pLeapController->failedDevices();

	for (auto dl = leapDevices.begin(); dl != leapDevices.end(); ++dl) {
		const Leap::FailedDevice leapDevice = *dl;

		DEBUG_LINEOUT("  PNP ID: %s", leapDevice.pnpId().c_str());
		DEBUG_LINEOUT("  Failure type: %d", leapDevice.failure());
	}
}

void SenseLeapMotion::onLogMessage(const Leap::Controller&, Leap::MessageSeverity severity, int64_t timestamp, const char* msg) {
	switch (severity) {
		case Leap::MESSAGE_CRITICAL: {
			DEBUG_LINEOUT("[Critical]");
		} break;
		
		case Leap::MESSAGE_WARNING: {
			DEBUG_LINEOUT("[Warning]");

		} break;
			
		case Leap::MESSAGE_INFORMATION: {
			DEBUG_LINEOUT("[Info]");
		} break;

		case Leap::MESSAGE_UNKNOWN: {
			DEBUG_LINEOUT("[Unknown]");
		} break;
	}

	DEBUG_LINEOUT("[%I64d]", timestamp);
	DEBUG_LINEOUT(msg);
}

bool SenseLeapMotion::IsConnected() {
	if (m_pLeapController != nullptr)
		return m_pLeapController->isConnected();
	else
		return false;
}

bool SenseLeapMotion::IsServiceConnected() {
	if (m_pLeapController != nullptr)
		return m_pLeapController->isServiceConnected();
	else
		return false;
}

bool SenseLeapMotion::HasFocus() {
	if (m_pLeapController != nullptr)
		return m_pLeapController->hasFocus();
	else
		return false;
}

RESULT SenseLeapMotion::Notify(CmdPromptEvent *event) {
	RESULT r =  R_PASS;
	if (event->GetArg(1).compare("swap") == 0) {
		if (m_pLeftHand != nullptr) {
			m_pLeftHand->SetVisible(!m_pLeftHand->IsVisible());
			m_pLeftHand->SetSkeleton(m_pLeftHand->IsVisible());
		}
		if (m_pRightHand != nullptr) {
			m_pRightHand->SetVisible(!m_pRightHand->IsVisible());
			m_pRightHand->SetSkeleton(m_pRightHand->IsVisible());
		}
		//std::dynamic_pointer_cast<DimObj>(m_pLeftModel->GetChildren()[0])->SetVisible(!m_pLeftHand->IsVisible());
		//std::dynamic_pointer_cast<DimObj>(m_pRightModel->GetChildren()[0])->SetVisible(!m_pRightHand->IsVisible());
		
		if (m_pLeftModel != nullptr)
			m_pLeftModel->SetVisible(!m_pLeftHand->IsVisible());

		if (m_pRightModel != nullptr)
			m_pRightModel->SetVisible(!m_pRightHand->IsVisible());


	}


	return r;
}
