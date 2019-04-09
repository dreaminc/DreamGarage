#ifndef OCULUS_PRODUCTION_BUILD
#include "OpenVRController.h"
#include "Core/Utilities.h"

OpenVRController::OpenVRController(vr::IVRSystem *pIVRHMD) :
	SenseController(),
	m_pIVRHMD(pIVRHMD)
{
	// empty
}

OpenVRController::~OpenVRController() {
	// empty
}

RESULT OpenVRController::Initialize() {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

// TODO: Implement these
RESULT OpenVRController::SubmitHapticImpulse(ControllerType controllerType, HapticCurveType shape, float amplitude, float msDuration, int cycles) {
	RESULT r = R_PASS;

	CN(m_pIVRHMD);

	vr::ETrackedControllerRole unDeviceType;

	switch (controllerType) {
	case CONTROLLER_LEFT: unDeviceType = vr::TrackedControllerRole_LeftHand; break;
	case CONTROLLER_RIGHT: unDeviceType = vr::TrackedControllerRole_RightHand; break;
	default: unDeviceType = vr::TrackedControllerRole_Invalid; break;
	}

	vr::TrackedDeviceIndex_t unControllerDeviceIndex = m_pIVRHMD->GetTrackedDeviceIndexForControllerRole(unDeviceType);

	// pulse is in micro-seconds
	// TODO: add more haptic resolution (axes)

	unsigned short usDuration = (unsigned short)(msDuration * 1000.0f);
	util::Clamp<unsigned short>(usDuration, 100, 3999);	// undocumented max range
	uint32_t axisIndex = vr::k_EButton_Axis0 - vr::k_EButton_Axis0;		// Add different button axes

	// TODO: this function is deprecated, for more control the lower function needs to be supported
	m_pIVRHMD->TriggerHapticPulse(unControllerDeviceIndex, axisIndex, usDuration);
	
	//float sDuration = msDuration / 1000.0f;
	//float freq = 1.0f / sDuration;
	//vr::VRInput()->TriggerHapticVibrationAction(unControllerDeviceIndex, 0, sDuration, freq, amplitude, vr::k_ulInvalidInputValueHandle);

Error:
	return r;
}

RESULT OpenVRController::SubmitHapticBuffer(ControllerType controllerType, HapticCurveType type, float amplitude, float freq, float msDuration) {
	//return R_NOT_IMPLEMENTED;
	return SubmitHapticImpulse(controllerType, SenseController::HapticCurveType::INVALID, 1.0f, msDuration);
}

#endif
