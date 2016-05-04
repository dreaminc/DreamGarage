#ifndef OVR_H_
#define OVR_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HMD/Oculus/OVR.h
// The Oculus Rift headset class - might sub class to other rift versions over time

// TODO: Should this go into Sense?

#include "HMD/HMD.h"

#include "External\OCULUS\v132\LibOVR\Include\OVR_CAPI.h"
//#pragma comment(lib, "External/OCULUS/v132/LibOVR/Lib/Windows/x64/Release/VS2015/LibOVR.lib")

#include <vector>
#include <algorithm>

class OVR : public HMD {
public:
	OVR() :
		m_ovrSession(nullptr)
	{
		// empty stub
	}

	~OVR() {
		// empty stub
	}

	RESULT InitializeHMD() {
		RESULT r = R_PASS;
		ovrGraphicsLuid luid;

		// Initializes LibOVR, and the Rift
		CRM((RESULT)ovr_Initialize(nullptr), "Failed to initialize libOVR.");

		// Attempt to create OVR session
		CRM((RESULT)ovr_Create(&m_ovrSession, &luid), "Failed to create OVR session");

		m_ovrHMDDescription = ovr_GetHmdDesc(m_ovrSession);

		// Get tracker descriptions
		unsigned int trackerCount = std::max<unsigned int>(1, ovr_GetTrackerCount(m_ovrSession)); 
		for (unsigned int i = 0; i < trackerCount; ++i)
			m_TrackerDescriptions.push_back(ovr_GetTrackerDesc(m_ovrSession, i));


	Error:
		return r;
	}

// TODO: Better way?
#define HMD_OVR_USE_PREDICTED_TIMNIG

	RESULT UpdateHMD() {
		RESULT r = R_PASS;

#ifdef HMD_OVR_USE_PREDICTED_TIMNIG
		double fTiming = ovr_GetPredictedDisplayTime(m_ovrSession, 0);
#else
		double fTiming = ovr_GetTimeInSeconds();
#endif
		ovrTrackingState trackingState = ovr_GetTrackingState(m_ovrSession, fTiming, true);

		if (trackingState.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked)) {
			//ovrPosef headPose = trackingState.HeadPose.ThePose;
			m_ptOrigin = point(reinterpret_cast<float*>(&(trackingState.HeadPose.ThePose.Position)));
			m_qOrientation = quaternion(*reinterpret_cast<quaternionXYZW*>(&(trackingState.HeadPose.ThePose.Orientation)));
		}

	Error:
		return r;
	}

	RESULT ReleaseHMD() {
		RESULT r = R_PASS;

		if (m_ovrSession != nullptr) {
			ovr_Destroy(m_ovrSession);
			m_ovrSession = nullptr;
		}

		ovr_Shutdown();

	Error:
		return r;
	}

public:
	ovrSession m_ovrSession;
	ovrHmdDesc m_ovrHMDDescription;
	std::vector<ovrTrackerDesc>   m_TrackerDescriptions;
};

#endif // ! HMD_IMP_H_