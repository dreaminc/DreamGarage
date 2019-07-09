#ifndef OVR_TOUCH_CONTROLLER_H_
#define OVR_TOUCH_CONTROLLER_H_

#include <windows.h>

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HMD/Oculus/OVRTouchController.h
// The OVR touch controller

// Include the Oculus SDK
#include "OVR_CAPI_GL.h"

#include "Sense/SenseController.h"

#include <vector>

#define OVR_HAPTIC_FREQUENCY 320.0f

class OVRTouchController : public SenseController {
public:
	OVRTouchController(ovrSession ovrSession);
	~OVRTouchController();

	virtual RESULT Initialize() override;

	ovrControllerType GetOVRControllerType(ControllerType controllerType);

	virtual RESULT SubmitHapticImpulse(ControllerType controllerType, HapticCurveType shape, float amplitude, float msDuration, int cycles = 1) override;
	virtual RESULT SubmitHapticBuffer(ControllerType controllerType, HapticCurveType type, float amplitude, float freq, float msDuration) override;
	RESULT SubmitHapticBuffer(ovrControllerType controllerType, std::vector<uint8_t> hapticBuffer);

private:
	ovrSession m_ovrSession;
};

#endif // ! WIN64_KEYBOARD_H_