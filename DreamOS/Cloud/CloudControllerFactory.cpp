#include "CloudControllerFactory.h"
#include "CEFImp.h"

#include <memory>

CloudController* CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_TYPE type, void *pContext = nullptr) {
	RESULT r = R_PASS;
	CloudController *pCloudController = nullptr;

	pCloudController = new CloudController();
	CN(pCloudController);

	// Initialize the User Object
	CR(pCloudController->InitializeUser());

	// TODO: Non-exclusive for clopudimp
	if(type & CLOUD_CONTROLLER_CEF) {

		// Create the CEF implementation			
		std::unique_ptr<CEFImp> pCEFImp(new CEFImp());
		CN(pContext);

		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(pContext);
		pCEFImp->CEFInitialize(hInstance);

		pCloudController->SetCloudImp(std::move(pCEFImp));
	} 

	// TODO: Add initialization here
	//CLOUD_CONTROLLER_WEBRTC = (1u << 1),
	//CLOUD_CONTROLLER_WEBSOCKET = (1u << 2),
	//CLOUD_CONTROLLER_CURL = (1u << 3),

//Success:
	return pCloudController;

Error:
	if (pCloudController != nullptr) {
		delete pCloudController;
		pCloudController = nullptr;
	}

	return nullptr;
}
