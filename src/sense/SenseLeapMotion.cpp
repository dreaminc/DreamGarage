#include "SenseLeapMotion.h"

#include "logger/DreamLogger.h"

#include "core/primitives/composite.h"

#include "LeapHand.h"

SenseLeapMotion::SenseLeapMotion() :
	m_pLeapController(nullptr),
	m_pLeftHand(nullptr),
	m_pRightHand(nullptr)
{
//Success:
	Validate();
	return;
}

RESULT SenseLeapMotion::InitLeapMotion() {
	RESULT r = R_PASS;

	m_pLeapController = std::make_unique<Leap::Controller>();
	CNM(m_pLeapController, "Failed to create leap motion controller");

	// this determines if a user has leap motion installed or not
	CBM((m_pLeapController->addListener(*this)), "Failed to add SenseLeapMotion as listener device");

	m_pLeapController->setPolicy(Leap::Controller::POLICY_ALLOW_PAUSE_RESUME);

	//OVERLAY_DEBUG_SET("LeapMotion", "Controller Leap Motion - Detected...");
	DOSLOG(INFO, "Leap Motion Controller Detected ... ");

Error:
	return r;
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
	DOSLOG(INFO, "LeapMotion", "Controller Leap Motion - Init...");
}

void SenseLeapMotion::onConnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Connected");
	DOSLOG(INFO, "LeapMotion", "Controller Leap Motion - Connected");
}

void SenseLeapMotion::onDisconnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Disconnected");
	DOSLOG(INFO, "LeapMotion", "Controller Leap Motion - Disconnected");
}

void SenseLeapMotion::onExit(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Exited");
	DOSLOG(INFO, "LeapMotion", "Controller Leap Motion - Exited");
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
				fLeftHandTracked = true;
			}
		}
		else {
			if (m_pRightHand != nullptr) {
				m_pRightHand->SetFromLeapHand(hand);
				fRightHandTracked = true;
			}
		}
	}
	
	if (!fLeftHandTracked) {
		if (m_pLeftHand != nullptr) {
			m_pLeftHand->OnLostTrack();
		}
	}

	if (!fRightHandTracked) {
		if (m_pRightHand != nullptr) {
			m_pRightHand->OnLostTrack();
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

RESULT SenseLeapMotion::AttachHand(LeapHand* pHand, HAND_TYPE handType) {
	RESULT r = R_PASS;

	CN(pHand);

	if (handType == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
	}
	else if (handType == HAND_TYPE::HAND_RIGHT) {
		m_pRightHand = pHand;
	}
	else {
		CBM((false), "Failed to attatch invalid hand type");
	}

Error:
	return r;
}

RESULT SenseLeapMotion::AttachModel(composite *pModel, HAND_TYPE handType) {
	RESULT r = R_PASS;

	CN(pModel);

	if (handType == HAND_TYPE::HAND_LEFT) {
		m_pLeftModel = pModel;
	}
	else if (handType == HAND_TYPE::HAND_RIGHT) {
		m_pRightModel = pModel;
	}
	else {
		CBM((false), "Failed to attatch model of invalid hand type");
	}

Error:
	return r;
}

hand* SenseLeapMotion::GetHand(HAND_TYPE handType) {
	if (handType == HAND_TYPE::HAND_LEFT) {
		return m_pLeftHand;
	}
	else if (handType == HAND_TYPE::HAND_RIGHT) {
		return m_pRightHand;
	}

	return nullptr;
}