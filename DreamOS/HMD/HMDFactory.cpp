#include "HMDFactory.h"

#include "HMD\Oculus\OVR.h"
#include "HMD\OpenVR\OpenVRDevice.h"

// TODO: Sandbox might be enough, don't need to pass HAL as well
HMD* HMDFactory::MakeHMD(HMD_TYPE type, SandboxApp *pParentSandbox, HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	HMD *pHMD = nullptr;

	switch (type) {
		case HMD_OVR: {
			pHMD = new OVRHMD(pParentSandbox);
			CRM(pHMD->InitializeHMD(halimp, wndWidth, wndHeight), "Failed to initialize HMD!");
		} break;

		case HMD_OPENVR: {
			pHMD = new OpenVRDevice(pParentSandbox);
			CRM(pHMD->InitializeHMD(halimp, wndWidth, wndHeight), "Failed to initialize HMD!");
		} break;

		default: {
			pHMD = nullptr;
			DEBUG_LINEOUT("HMD type %d not supported", type);
		} break;
	}

	return pHMD;
Error:
	if (pHMD != nullptr) {
		delete pHMD;
		pHMD = nullptr;
	}
	return nullptr;
}

