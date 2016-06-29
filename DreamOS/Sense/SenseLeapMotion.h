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

// TODO
typedef struct SenseLeapMotionEvent : SenseDevice::SenseDeviceEvent {
	//SK_SCAN_CODE KeyCode;
	//uint8_t KeyState;

	SenseLeapMotionEvent() :
		SenseDeviceEvent()
	{
		SenseEventSize = sizeof(SenseLeapMotionEvent);
		//KeyCode = key;
		//KeyState = state;
	}
} SENSE_LEAPMOTION_EVENT;

class SenseLeapMotion : public SenseDevice, public Publisher<int, SenseLeapMotionEvent>, public Leap::Listener, public valid {
public:
	SenseLeapMotion();
	~SenseLeapMotion();

private:
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

private:
	RESULT SetPause(bool fPauseState);

public:
	RESULT Pause();
	RESULT Resume();
private:
	std::unique_ptr<Leap::Controller> m_pLeapController;
};

#endif // ! SENSE_LEAPMOTION_H_