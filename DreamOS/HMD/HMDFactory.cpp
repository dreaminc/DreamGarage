#include "HMDFactory.h"

#include "HMD\Oculus\OVR.h"
#include "HMD\OpenVR\OpenVRDevice.h"

HMD* HMDFactory::MakeHMD(HMD_TYPE type, HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	HMD *pHMD = nullptr;

	switch (type) {
		case HMD_OVR: {
			pHMD = new OVRHMD();
			CRM(pHMD->InitializeHMD(halimp, wndWidth, wndHeight), "Failed to initialize HMD!");
		} break;

		case HMD_OPENVR: {
			pHMD = new OpenVRDevice();
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

