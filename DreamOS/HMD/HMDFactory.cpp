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
			CN(pHMD);
			CRM(pHMD->InitializeHMD(halimp, wndWidth, wndHeight), "Failed to initialize HMD!");
		} break;

		case HMD_OPENVR: {
			pHMD = new OpenVRDevice(pParentSandbox);
			CN(pHMD);
			CRM(pHMD->InitializeHMD(halimp, wndWidth, wndHeight), "Failed to initialize HMD!");
		} break;

		case HMD_ANY_AVAILABLE: {
			// Oculus first
			CB((pHMD == nullptr));
			pHMD = new OVRHMD(pParentSandbox);
			CN(pHMD);

			if (pHMD->InitializeHMD(halimp, wndWidth, wndHeight) == R_PASS) {
				break;
			}
			else {
				if (pHMD != nullptr) {
					delete pHMD;
					pHMD = nullptr;
				}
			}

			// OPENVR Second
			CB((pHMD == nullptr));
			pHMD = new OpenVRDevice(pParentSandbox);
			CN(pHMD);

			if (pHMD->InitializeHMD(halimp, wndWidth, wndHeight) == R_PASS) {
				break;
			}
			else {
				if (pHMD != nullptr) {
					delete pHMD;
					pHMD = nullptr;
				}
			}
			
			CBM((false), "Failed to find an available HMD");

		} break;

		default: {
			pHMD = nullptr;
			DEBUG_LINEOUT("HMD type %d not supported", type);
		} break;
	}

	CR(pHMD->InitializeHMDSinkNode());

	return pHMD;
Error:
	if (pHMD != nullptr) {
		delete pHMD;
		pHMD = nullptr;
	}
	return nullptr;
}

