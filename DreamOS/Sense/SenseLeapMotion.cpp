#include "SenseLeapMotion.h"

SenseLeapMotion::SenseLeapMotion() :
	m_pLeapController(nullptr),
	m_pVirtualObj(nullptr)
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

Success:
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

		SenseLeapMotionHand sHand(hand);
		sHand.toString();

		if (m_pVirtualObj != nullptr) {
			m_pVirtualObj->MoveTo(sHand.PalmPosition());
		}

		/*
		// Get the hand's normal vector and direction
		const Leap::Vector normal = hand.palmNormal();
		const Leap::Vector direction = hand.direction();

		// Calculate the hand's pitch, roll, and yaw angles
		std::cout << std::string(2, ' ') << "pitch: " << direction.pitch() * Leap::RAD_TO_DEG << " degrees, "
			<< "roll: " << normal.roll() * Leap::RAD_TO_DEG << " degrees, "
			<< "yaw: " << direction.yaw() * Leap::RAD_TO_DEG << " degrees" << std::endl;

		// Get the Arm bone
		Leap::Arm arm = hand.arm();

		std::cout << std::string(2, ' ') << "Arm direction: " << arm.direction()
			<< " wrist position: " << arm.wristPosition()
			<< " elbow position: " << arm.elbowPosition() << std::endl;

		// Get fingers
		const Leap::FingerList fingers = hand.fingers();
		for (auto fl = fingers.begin(); fl != fingers.end(); ++fl) {
			const Leap::Finger finger = *fl;

			std::cout << std::string(4, ' ') << FingerNames[finger.type()]
				<< " finger, id: " << finger.id()
				<< ", length: " << finger.length()
				<< "mm, width: " << finger.width() << std::endl;

			// Get finger bones
			for (int b = 0; b < 4; ++b) {
				Leap::Bone::Type boneType = static_cast<Leap::Bone::Type>(b);
				Leap::Bone bone = finger.bone(boneType);

				std::cout << std::string(6, ' ') << BoneNames[boneType]
					<< " bone, start: " << bone.prevJoint()
					<< ", end: " << bone.nextJoint()
					<< ", direction: " << bone.direction() << std::endl;
			}
		}
		*/
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
		DEBUG_LINEOUT("id: %s", leapDevices[i].toString());
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

		DEBUG_LINEOUT("  PNP ID: %s", leapDevice.pnpId());
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

	DEBUG_LINEOUT("[%d]", timestamp);
	DEBUG_LINEOUT(msg);
}
