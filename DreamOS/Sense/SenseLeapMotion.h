#ifndef SENSE_LEAPMOTION_H_
#define SENSE_LEAPMOTION_H_

#include <string>
#include <memory>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sense/SenseLeapMotion.h
// Sense Leap Motion Device
// This is a device wrapper for the Leap Motion hand tracking device

#include "Leap.h"

#include "SenseDevice.h"
#include "Primitives/Publisher.h"

#include "Primitives/valid.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/hand.h"

#include "Primitives/VirtualObj.h"

typedef enum SenseLeapMotionEventType {
	SENSE_LEAPMOTION_EVENT_HAND_LEFT,
	SENSE_LEAPMOTION_EVENT_HAND_RIGHT,
	SENSE_LEAPMOTION_EVENT_INVALID,
} SENSE_LEAPMOTION_EVENT_TYPE;

typedef enum SenseLeapMotionHandType {
	SENSE_LEAPMOTION_HAND_LEFT,
	SENSE_LEAPMOTION_HAND_RIGHT,
	SENSE_LEAPMOTION_HAND_INVALID
} SENSE_LEAPMOTION_HAND_TYPE;

class SenseLeapMotionHand {
public:
	SenseLeapMotionHand(const Leap::Hand hand) {
		InitializeFromLeapHand(hand);
	}

	~SenseLeapMotionHand() {
		// empty
	}

	bool IsLeftHand() {
		return (m_handType == SENSE_LEAPMOTION_HAND_LEFT);
	}

	bool IsRightHand() {
		return (m_handType == SENSE_LEAPMOTION_HAND_RIGHT);
	}

	RESULT InitializeFromLeapHand(const Leap::Hand hand) {
		RESULT r = R_PASS;

		m_handType = (hand.isLeft()) ? SENSE_LEAPMOTION_HAND_LEFT : SENSE_LEAPMOTION_HAND_RIGHT;

		m_leapHandID = hand.id();
		Leap::Vector leapPalmPosition = hand.palmPosition();
		leapPalmPosition /= 1000.0f;	// Leap outputs in mm, and our engine is in meters

		m_ptPalmPosition = point(leapPalmPosition.x, leapPalmPosition.y, leapPalmPosition.z);

	Error:
		return r;
	}

	const char *HandTypeString() {
		if (m_handType == SENSE_LEAPMOTION_HAND_LEFT)
			return "left";
		else if (m_handType == SENSE_LEAPMOTION_HAND_RIGHT)
			return "right";
		else
			return "invalid";
	}

	RESULT toString() {
		RESULT r = R_PASS;

		DEBUG_LINEOUT("%s hand id:%d position:%s", HandTypeString(), m_leapHandID, m_ptPalmPosition.toString().c_str());

	Error:
		return r;
	}

	point PalmPosition() {
		return m_ptPalmPosition;
	}

private:
	SENSE_LEAPMOTION_HAND_TYPE m_handType;
	point m_ptPalmPosition;
	int32_t m_leapHandID;
};

typedef struct SenseLeapMotionEvent : SenseDevice::SenseDeviceEvent {
	SenseLeapMotionHand SLMHand;

	SenseLeapMotionEvent(SenseLeapMotionHand slmHand) :
		SenseDeviceEvent(),
		SLMHand(slmHand)
	{
		SenseEventSize = sizeof(SenseLeapMotionEvent);
	}
} SENSE_LEAPMOTION_EVENT;

class SenseLeapMotion : public SenseDevice, public Publisher<int, SenseLeapMotionEvent>, public Leap::Listener, public valid {
public:
	SenseLeapMotion();
	~SenseLeapMotion();

private:
	const std::string FingerNames[5] = { "Thumb", "Index", "Middle", "Ring", "Pinky" };
	const std::string BoneNames[5] = { "Metacarpal", "Proximal", "Middle", "Distal" };

	// Leap Motion Callbacks
	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller&);
	virtual void onDisconnect(const Leap::Controller&);
	virtual void onExit(const Leap::Controller&);
	virtual void onFrame(const Leap::Controller&);
	virtual void onFocusGained(const Leap::Controller&);
	virtual void onFocusLost(const Leap::Controller&);
	virtual void onDeviceChange(const Leap::Controller&);
	virtual void onServiceConnect(const Leap::Controller&);
	virtual void onServiceDisconnect(const Leap::Controller&);
	virtual void onServiceChange(const Leap::Controller&);
	virtual void onDeviceFailure(const Leap::Controller&);
	virtual void onLogMessage(const Leap::Controller&, Leap::MessageSeverity severity, int64_t timestamp, const char* msg);


	// TODO: debug func
public:
	/*
	RESULT AttachVirtualObj(VirtualObj *pVirtualObj) {
		m_pVirtualObj = pVirtualObj;
		return R_PASS;
	}
	*/

	RESULT AttachHand(hand *pHand) {
		m_pLeftHand = pHand;
		return R_PASS;
	}

private:
	RESULT SetPause(bool fPauseState);

public:
	RESULT Pause();
	RESULT Resume();

private:
	std::unique_ptr<Leap::Controller> m_pLeapController;
	//VirtualObj *m_pVirtualObj;	// temp
	hand *m_pLeftHand;
	hand *m_pRightHand;
};

#endif // ! SENSE_LEAPMOTION_H_