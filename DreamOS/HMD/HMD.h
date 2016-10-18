#ifndef HMD_H_
#define HMD_H_

#include "./RESULT/EHM.h"

#include <time.h>       

// Dream OS
// DreamOS/HMD/HMD.h
// The HMD parent class 

// TODO: Should this go into Sense?

#include "Primitives/Publisher.h"

#include "Primitives/matrix/ViewMatrix.h"
#include "Primitives/matrix/ProjectionMatrix.h"
#include "Primitives/Types/UID.h"
#include "Primitives/quaternion.h"
#include "Primitives/point.h"
#include "Primitives/composite.h"

#define HMD_NUM_EYES 2

class HALImp;
class SandboxApp;

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
	HMD(SandboxApp *pParentSandbox) :
		m_pHALImp(nullptr),
		m_pParentSandbox(pParentSandbox),
		m_eyeWidth(0),
		m_eyeHeight(0)
	{
		// empty stub
	}

	~HMD() {
		// empty stub
	}

public:
	// Called by factory to initialize HMD
	//virtual RESULT InitializeHMD(HALImp *halimp) = 0;
	virtual RESULT InitializeHMD(HALImp *halimp, int wndWidth = 0, int wndHeight = 0) = 0;

	// Called to update/poll tracking info
	virtual RESULT UpdateHMD() = 0;


	virtual RESULT SetUpFrame() = 0;
	virtual RESULT BindFramebuffer(EYE_TYPE eye) = 0;
	virtual RESULT CommitSwapChain(EYE_TYPE eye) = 0;
	virtual RESULT SubmitFrame() = 0;

	// TODO: Rename this - it's not well named (currently named after OVR sample code)
	virtual RESULT SetAndClearRenderSurface(EYE_TYPE eye) = 0;
	virtual RESULT UnsetRenderSurface(EYE_TYPE eye) = 0;

	virtual RESULT RenderHMDMirror() = 0;

	inline quaternion GetHMDOrientation() { return m_qOrientation; }
	inline point GetHMDOrigin() { return m_ptOrigin; }
	inline vector GetHMDTrackerDeviation() { return GetHMDOrigin(); }

	virtual ProjectionMatrix GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) = 0;
	virtual ViewMatrix GetViewMatrix(EYE_TYPE eye) = 0;

	int GetEyeWidth() { return m_eyeWidth; }
	int GetEyeHeight() { return m_eyeHeight; }

	point GetHeadPointOrigin() {
		return m_ptOrigin;
	}

protected:
	point m_ptOrigin;
	quaternion m_qOrientation;

	uint32_t m_eyeWidth;
	uint32_t m_eyeHeight;

	HALImp *m_pHALImp;	// TODO: This may not be needed if Sandbox parent is kept

	SandboxApp *m_pParentSandbox;

private:
	UID m_uid;
};

#endif // ! HMD_H_