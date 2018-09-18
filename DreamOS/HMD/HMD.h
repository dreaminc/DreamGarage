#ifndef HMD_H_
#define HMD_H_

#include "./RESULT/EHM.h"

#include <time.h>       

// Dream OS
// DreamOS/HMD/HMD.h
// The HMD parent class 

// TODO: All "sense" related functionality should be woven into the Source node
// TODO: All "sink/display" related functionality should be pushed into the Sink node

#include "Primitives/Publisher.h"

#include "Primitives/matrix/ViewMatrix.h"
#include "Primitives/matrix/ProjectionMatrix.h"
#include "Primitives/Types/UID.h"
#include "Primitives/quaternion.h"
#include "Primitives/point.h"
#include "Primitives/composite.h"
#include "Primitives/hand.h"
#include "Primitives/HandType.h"

#include "Sense/SenseController.h"

#define HMD_NUM_EYES 2

class HALImp;
class SandboxApp;

class HMDSinkNode;
class HMDSourceNode;

typedef enum HMDEventType {
	HMD_EVENT_ORIENTATION,
	HMD_EVENT_POSITION,
	HMD_EVENT_BOTH,
	HMD_EVENT_INVALID
} HMD_EVENT_TYPE;

enum class HMDDeviceType {
	OCULUS,
	VIVE,
	META,
	NONE	
};

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
	virtual HMDSinkNode *GetHMDSinkNode() = 0;
	virtual HMDSourceNode *GetHMDSourceNode() = 0;
	virtual RESULT InitializeHMDSourceNode() = 0;
	virtual RESULT InitializeHMDSinkNode() = 0;

public:
	// Called by factory to initialize HMD
	//virtual RESULT InitializeHMD(HALImp *halimp) = 0;
	virtual RESULT InitializeHMD(HALImp *halimp, int wndWidth = 0, int wndHeight = 0, bool fHMDMirror = true) = 0;

	// Called to update/poll tracking info
	virtual RESULT UpdateHMD() = 0;

	virtual RESULT ReleaseHMD() = 0;

	virtual RESULT SetUpFrame() = 0;
	virtual RESULT BindFramebuffer(EYE_TYPE eye) = 0;
	virtual RESULT CommitSwapChain(EYE_TYPE eye) = 0;
	virtual RESULT SubmitFrame() = 0;

	// TODO: Rename this - it's not well named (currently named after OVR sample code)
	virtual RESULT SetAndClearRenderSurface(EYE_TYPE eye) = 0;
	virtual RESULT UnsetRenderSurface(EYE_TYPE eye) = 0;

	virtual RESULT RenderHMDMirror() = 0;

	inline quaternion GetHMDOrientation() { return m_qOrientation; }
	inline point GetHeadPointOrigin() { return m_ptOrigin; }
	inline vector GetHMDTrackerDeviation() { return GetHeadPointOrigin(); }

	virtual RESULT RecenterHMD() = 0;

	virtual ProjectionMatrix GetPerspectiveFOVMatrix(EYE_TYPE eye, float znear, float zfar) = 0;
	virtual ViewMatrix GetViewMatrix(EYE_TYPE eye) = 0;

	RESULT AttachHand(hand *pHand, HAND_TYPE type);
	hand* GetHand(HAND_TYPE type);

	SenseController* GetSenseController();
	virtual composite *GetSenseControllerObject(ControllerType controllerType) = 0;
	virtual HMDDeviceType GetDeviceType() = 0;

	// string is used instead of HMDDeviceType for server commands like settings
	virtual std::string GetDeviceTypeString() = 0;

	// Audio
	virtual RESULT GetAudioDeviceOutID(std::wstring &wstrAudioDeviceOutGUID) = 0;
	virtual RESULT GetAudioDeviceInGUID(std::wstring &wstrAudioDeviceInGUID) = 0;

	int GetEyeWidth() { return m_eyeWidth; }
	int GetEyeHeight() { return m_eyeHeight; }

protected:
	RESULT SetEyeWidth(int eyeWidth) {
		m_eyeWidth = eyeWidth;
		return R_PASS;
	}

	RESULT SetEyeHeight(int eyeHeight) {
		m_eyeHeight = eyeHeight;
		return R_PASS;
	}

protected:
	point m_ptOrigin;
	quaternion m_qOrientation;

	uint32_t m_eyeWidth;
	uint32_t m_eyeHeight;

	HALImp *m_pHALImp;	// TODO: This may not be needed if Sandbox parent is kept

	SandboxApp *m_pParentSandbox;

	hand* m_pLeftHand;
	hand* m_pRightHand;

	SenseController *m_pSenseController;

private:
	UID m_uid;
};

#endif // ! HMD_H_