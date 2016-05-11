#include "HMDFactory.h"

#include "HMD\Oculus\OVR.h"

HMD* HMDFactory::MakeHMD(HMD_TYPE type, HALImp *halimp, int wndWidth, int wndHeight) {
	RESULT r = R_PASS;
	HMD *pHMD = nullptr;

	switch (type) {
		case HMD_OVR: {
			pHMD = new OVR();
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

