#ifndef OPENVR_CONTROLLER_H_
#define OPENVR_CONTROLLER_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HMD/OpenVR/OpenVRController.h
// The OpenVR controller class

#ifndef OCULUS_PRODUCTION_BUILD
// Include the OpenVR SDK
#include <openvr.h>

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
#endif
#endif // ! OPENVR_CONTROLLER_H_