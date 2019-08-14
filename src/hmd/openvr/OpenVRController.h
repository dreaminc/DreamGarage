#ifndef OPENVR_CONTROLLER_H_
#define OPENVR_CONTROLLER_H_
#ifndef OCULUS_PRODUCTION_BUILD

#include "core/ehm/EHM.h"

// Dream HMD OpenVR
// dos/src/hm/openvr/OpenVRController.h

// The OpenVR controller class

// Include the OpenVR SDK
#include <openvr.h>
#include <windows.h>

#include "Sense/SenseController.h"

#include <vector>

#define OVR_HAPTIC_FREQUENCY 320.0f

class OpenVRController : public SenseController {
public:
	OpenVRController(vr::IVRSystem *pIVRHMD);
	~OpenVRController();

	virtual RESULT Initialize() override;

	virtual RESULT SubmitHapticImpulse(ControllerType controllerType, HapticCurveType shape, float amplitude, float msDuration, int cycles = 1) override;
	virtual RESULT SubmitHapticBuffer(ControllerType controllerType, HapticCurveType type, float amplitude, float freq, float msDuration) override;
	
	//RESULT SubmitHapticBuffer(ovrControllerType controllerType, std::vector<uint8_t> hapticBuffer);

private:
	vr::IVRSystem *m_pIVRHMD = nullptr;
};

#endif	// ! OCULUS_PRODUCTION_BUILD
#endif	// ! OPENVR_CONTROLLER_H_