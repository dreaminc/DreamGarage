#include "SenseLeapMotion.h"

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

	/*
	for (int i = 0; i < NUM_SENSE_KEYBOARD_KEYS; i++) {
	RegisterEvent(i);
	}
	*/

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
}

void SenseLeapMotion::onConnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Connected");
}

void SenseLeapMotion::onDisconnect(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Disconnected");
}

void SenseLeapMotion::onExit(const Leap::Controller&) {
	DEBUG_LINEOUT("SenseLeapMotion: Exited");
}

void SenseLeapMotion::onFrame(const Leap::Controller&) {
	const Leap::Frame leapFrame = m_pLeapController->frame();

	//DEBUG_LINEOUT("Frame id:%d timestamp:%d hands:%d fingers:%d", leapFrame.id(), leapFrame.timestamp(), leapFrame.hands().count(), leapFrame.fingers().count());

	Leap::HandList hands = leapFrame.hands();

	for (auto hl = hands.begin(); hl != hands.end(); ++hl) {
		// Get the first hand
		const Leap::Hand hand = *hl;

		if ((hand.isLeft())) {
			if (m_pLeftHand != nullptr) {
				m_pLeftHand->SetFromLeapHand(hand);
			}
		}
		else {
			if (m_pRightHand != nullptr) {
				m_pRightHand->SetFromLeapHand(hand);
			}
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
