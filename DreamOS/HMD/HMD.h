#ifndef HMD_H_
#define HMD_H_

#include "./RESULT/EHM.h"

#include <time.h>       

// Dream OS
// DreamOS/HMD/HMD.h
// The HMD parent class 

// TODO: Should this go into Sense?

#include "Primitives/Publisher.h"

#include "Primitives/Types/UID.h"
#include "Primitives/quaternion.h"
#include "Primitives/point.h"

#include "HAL/HALImp.h"

typedef enum HMDEventType {
	HMD_EVENT_ORIENTATION,
	HMD_EVENT_POSITION,
	HMD_EVENT_BOTH,
	HMD_EVENT_INVALID
} HMD_EVENT_TYPE;

enum EYE_TYPE;

typedef struct HMDEvent {
	HMDEventType EventType;
	clock_t HMDEventTickCount;
	point ptOrigin;
	quaternion qOrientation;

	inline void SetEventClockTick() {
		HMDEventTickCount = clock();
	}

	HMDEvent(point pt, quaternion q) :
		EventType(HMD_EVENT_BOTH)
	{		
		SetEventClockTick();
		ptOrigin = pt;
		qOrientation = q;
	}

	HMDEvent(point pt) :
		EventType(HMD_EVENT_POSITION)
	{
		SetEventClockTick();
		ptOrigin = pt;
	}

	HMDEvent(quaternion q) :
		EventType(HMD_EVENT_ORIENTATION)
	{
		SetEventClockTick();
		qOrientation = q;
	}
} HMD_EVENT;

class HMD : public Publisher<HMDEventType, HMDEvent> {
public:
	HMD() {
		// empty stub
	}

	~HMD() {
		// empty stub
	}

public:
	// Called by factory to initialize HMD
	virtual RESULT InitializeHMD(HALImp *halimp) = 0;

	// Called to update/poll tracking info
	virtual RESULT UpdateHMD() = 0;

	virtual RESULT BindFramebuffer(EYE_TYPE eye) = 0;
	virtual RESULT CommitSwapChain() = 0;
	virtual RESULT SubmitFrame() = 0;

	quaternion GetHMDOrientation() { return m_qOrientation; }
	point GetHMDOrigin() { return m_ptOrigin; }

	int GetEyeWidth() { return m_eyeWidth; }
	int GetEyeHeight() { return m_eyeHeight; }

protected:
	point m_ptOrigin;
	quaternion m_qOrientation;

	int m_eyeWidth;
	int m_eyeHeight;

private:
	UID m_uid;
};

#endif // ! HMD_H_