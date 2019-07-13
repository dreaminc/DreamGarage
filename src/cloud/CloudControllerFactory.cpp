#include "CloudControllerFactory.h"

//#include "CEFImp.h"
#include "webrtc/WebRTCImp.h"

#include "Sandbox/CommandLineManager.h"

#include "CloudController.h"

#include <memory>

CloudController* CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_IMP_TYPE type, void *pContext = nullptr) {
	RESULT r = R_PASS;
	CloudController *pCloudController = nullptr;

	pCloudController = new CloudController();
	CN(pCloudController);
	CR(pCloudController->Initialize());

	// TODO: Create a collection of cloud implementations
	// or the various ones that can be used

	/*
	if(type & CLOUD_CONTROLLER_CEF) {

		// Create the CEF implementation			
		std::unique_ptr<CEFImp> pCEFImp = std::make_unique<CEFImp>();
		CN(pContext);

		HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(pContext);
		pCEFImp->CEFInitialize(hInstance);
		
		pCloudController->SetCloudImp(std::move(pCEFImp));
	}
	*/

	if(type & CLOUD_CONTROLLER_WEBRTC) {		
		// Create the CEF implementation			

		// TODO: this is dead code, peer controller now owns WebRTCImp and the rest
		std::unique_ptr<WebRTCImp> pWebRTCImp = std::make_unique<WebRTCImp>(pCloudController);
		CN(pWebRTCImp);
		CR(pWebRTCImp->Initialize());

		// TOOD: TEST CODE:
		

		/*
		if (pCommandLineManager->GetNumCommandLineArguments() < 2) {
			pWebRTCImp->StartLogin("localhost", 8888);
		}
		else {
			std::string strIPAddress = pCommandLineManager->GetCommandLineArgument(1);
			pWebRTCImp->StartLogin(strIPAddress, 8888);
		}
		*/

		pCloudController->SetCloudImp(std::move(pWebRTCImp));
	}

	// default: {
		// pCloudController = nullptr;
		// DEBUG_LINEOUT("CloudControllerFactory: Cloud controller type %d not supported on this platform!", type);
	// } break;

	// TODO: Add initialization here
	//CLOUD_CONTROLLER_WEBRTC = (1u << 1),
	//CLOUD_CONTROLLER_WEBSOCKET = (1u << 2),
	//CLOUD_CONTROLLER_CURL = (1u << 3),

	/*
	// Auto Login Handling
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	if (m_pCommandLineManager->GetParameterValue("login").compare("auto") == 0) {
		// auto login
		pCloudController->Start();
	}
	*/

//Success:
	return pCloudController;

Error:
	if (pCloudController != nullptr) {
		delete pCloudController;
		pCloudController = nullptr;
	}

	return nullptr;
}
