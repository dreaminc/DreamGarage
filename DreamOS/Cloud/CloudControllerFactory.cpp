#include "CloudControllerFactory.h"
#include "CEFImp.h"

#include <memory>

CloudController* CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_TYPE type, void *pContext = nullptr) {
	RESULT r = R_PASS;
	CloudController *pCloudController = nullptr;

	pCloudController = new CloudController();
	CN(pCloudController);

	switch (type) {
		case CLOUD_CONTROLLER_CEF: {
			// Create the CEF implementation			
			std::unique_ptr<CEFImp> pCEFImp(new CEFImp());
			CN(pContext);

			HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(pContext);
			pCEFImp->CEFInitialize(hInstance);

			pCloudController->SetCloudImp(std::move(pCEFImp));

		} break;

		default: {
			pCloudController = nullptr;
			DEBUG_LINEOUT("Sandbox type %d not supported on this platform!", type);
		} break;
	}

//Success:
	return pCloudController;

Error:
	if (pCloudController != nullptr) {
		delete pCloudController;
		pCloudController = nullptr;
	}

	return nullptr;
}
