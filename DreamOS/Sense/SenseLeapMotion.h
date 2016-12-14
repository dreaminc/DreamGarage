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
#include "Primitives/Subscriber.h"
#include "DreamConsole/DreamConsole.h"

#include "Primitives/valid.h"

#include "Primitives/VirtualObj.h"
#include "Primitives/composite.h"

#include "SenseLeapMotionHand.h"

typedef enum SenseLeapMotionEventType {
	SENSE_LEAPMOTION_EVENT_HAND_LEFT,
	SENSE_LEAPMOTION_EVENT_HAND_RIGHT,
	SENSE_LEAPMOTION_EVENT_INVALID,
} SENSE_LEAPMOTION_EVENT_TYPE;

typedef struct SenseLeapMotionEvent : SenseDevice::SenseDeviceEvent {
	SenseLeapMotionHand SLMHand;

	SenseLeapMotionEvent(SenseLeapMotionHand slmHand) :
		SenseDeviceEvent(),
		SLMHand(slmHand)
	{
		SenseEventSize = sizeof(SenseLeapMotionEvent);
	}
} SENSE_LEAPMOTION_EVENT;

class SenseLeapMotion : public SenseDevice, public Publisher<int, SenseLeapMotionEvent>, public Subscriber<CmdPromptEvent>, public Leap::Listener, public valid {
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

	RESULT AttachHand(hand *pHand, hand::HAND_TYPE handType) {
		//pHand->SetVisible(false);
		if(handType == hand::HAND_TYPE::HAND_LEFT)
			m_pLeftHand = pHand;
		else if (handType == hand::HAND_TYPE::HAND_RIGHT)
			m_pRightHand = pHand;
		else
			return R_FAIL;

		return R_PASS;
	}

	RESULT AttachModel(composite *pModel, hand::HAND_TYPE handType) {
		if(handType == hand::HAND_TYPE::HAND_LEFT)
			m_pLeftModel = pModel;
		else if (handType == hand::HAND_TYPE::HAND_RIGHT)
			m_pRightModel = pModel;
		else
			return R_FAIL;

		return R_PASS;
	}

	hand *GetHand(hand::HAND_TYPE handType) {
		if (handType == hand::HAND_TYPE::HAND_LEFT)
			return m_pLeftHand;
		else if (handType == hand::HAND_TYPE::HAND_RIGHT)
			return m_pRightHand;

		return nullptr;
	}

	bool IsConnected();
	bool IsServiceConnected();
	bool HasFocus();

private:
	RESULT SetPause(bool fPauseState);
	RESULT Notify(CmdPromptEvent *event);

public:
	RESULT Pause();
	RESULT Resume();

private:
	std::unique_ptr<Leap::Controller> m_pLeapController;
	//VirtualObj *m_pVirtualObj;	// temp
	hand *m_pLeftHand;
	hand *m_pRightHand;

	composite *m_pLeftModel;
	composite *m_pRightModel;
};

#endif // ! SENSE_LEAPMOTION_H_