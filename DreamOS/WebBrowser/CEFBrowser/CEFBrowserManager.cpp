#include "CEFBrowserManager.h"

#include "CEFBrowserController.h"

#include "CEFHandler.h"

#include "CEFApp.h"

#include "Cloud/Environment/EnvironmentAsset.h"

#include "Sandbox/PathManager.h"

CEFBrowserManager::CEFBrowserManager() {
	// empty
}

CEFBrowserManager::~CEFBrowserManager() {
	RESULT r = R_PASS;

	CRM(Shutdown(), "WebBrowserManager failed to shutdown properly");
	//CR(r);

Error:
	return;
}

RESULT CEFBrowserManager::Initialize() {
	RESULT r = R_PASS;

	/*
	m_ServiceThread = std::thread(&CEFBrowserManager::CEFManagerThread, this);
	std::unique_lock<std::mutex> lockCEFBrowserInitialization(m_mutex);
	m_condBrowserInit.wait(lockCEFBrowserInitialization);
	*/

	CR(CEFManagerThread());

	CBM((m_state == CEFBrowserManager::state::INITIALIZED), "CEFBrowserManager not correctly initialized");

// Success:
	return r;

Error:
	Shutdown();
	return r;
}
RESULT CEFBrowserManager::Update() {
	RESULT r = R_PASS;

	for (auto& pWebBrowserController : m_webBrowserControllers) {
		
		// TODO: optimize with actual dirty rects copy
		
		int numFramesProcessed = 0;
		CR(pWebBrowserController->PollNewDirtyFrames(numFramesProcessed));

		CR(pWebBrowserController->PollPendingAudioPackets(numFramesProcessed));
	}

Error:
	return r;
}

RESULT CEFBrowserManager::OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) {
	RESULT r = R_PASS;

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->OnAfterCreated());

Error:
	return r;
}

RESULT CEFBrowserManager::OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: GetViewRect");

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->OnGetViewRect(cefRect));

Error:
	return r;
}

RESULT CEFBrowserManager::OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int frames, int channels, int bitsPerSample, const void* pDataBuffer) {
	RESULT r = R_PASS;
	

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	DEBUG_LINEOUT("CEFBrowserManager: OnAudioData %S", pCEFBrowser->GetFocusedFrame()->GetURL().c_str());

	CR(pCEFBrowserController->OnAudioData(pCEFBrowser, frames, channels, bitsPerSample, pDataBuffer));

Error:
	return r;
}

RESULT CEFBrowserManager::OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;
	//DEBUG_LINEOUT("CEFBrowserManager: OnPaint");

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->OnPaint(type, dirtyRects, pBuffer, width, height));

Error:
	return r;
}

RESULT CEFBrowserManager::OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnLoadingStateChanged");

	std::string strCurrentURL = pCEFBrowser->GetFocusedFrame()->GetURL();

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->OnLoadingStateChanged(fLoading, fCanGoBack, fCanGoForward, strCurrentURL));

Error:
	return r;
}

RESULT CEFBrowserManager::OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFBrowserManager: OnLoadStart");

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->OnLoadStart(pCEFFrame, transition_type));

Error:
	return r;
}

RESULT CEFBrowserManager::OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFBrowserManager: OnLoadEnd");

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCEFBrowser);
	CN(pCEFBrowserController);

	// TODO: add frame
	CR(pCEFBrowserController->OnLoadEnd(pCEFFrame, httpStatusCode));

Error:
	return r;
}

RESULT CEFBrowserManager::OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFBrowserManager: OnFocusedNodeChanged");

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(cefBrowserID);
	CN(pCEFBrowserController);

	// TODO: add frame
	CR(pCEFBrowserController->OnFocusedNodeChanged(cefBrowserID, cefFrameID, pCEFDOMNode));

Error:
	return r;
}

std::shared_ptr<CEFBrowserController> CEFBrowserManager::GetCEFBrowserController(int cefBrowserID) {
	for (auto &pWebBrowserController : m_webBrowserControllers) {
		std::shared_ptr<CEFBrowserController> pCEFBrowserController = std::dynamic_pointer_cast<CEFBrowserController>(pWebBrowserController);

		if (pCEFBrowserController != nullptr) {
			if (pCEFBrowserController->GetCEFBrowser()->GetIdentifier() == cefBrowserID) {
				return pCEFBrowserController;
			}
		}
	}

	return nullptr;
}

std::shared_ptr<CEFBrowserController> CEFBrowserManager::GetCEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) {
	for (auto &pWebBrowserController : m_webBrowserControllers) {
		std::shared_ptr<CEFBrowserController> pCEFBrowserController = std::dynamic_pointer_cast<CEFBrowserController>(pWebBrowserController);

		if (pCEFBrowserController != nullptr) {
			if (pCEFBrowserController->GetCEFBrowser()->IsSame(pCEFBrowser))  {
				return pCEFBrowserController;
			}
		}
	}

	return nullptr;
}

RESULT CEFBrowserManager::GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL) {
	RESULT r = R_PASS;

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCefBrowser->GetIdentifier());
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->GetResourceHandlerType(resourceHandlerType, strCEFURL));

Error:
	return r;
}

RESULT CEFBrowserManager::CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) {
	RESULT r = R_PASS;

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCefBrowser->GetIdentifier());
	CN(pCEFBrowserController);

	CR(pCEFBrowserController->CheckForHeaders(headermap, strURL));

Error:
	return r;
}

RESULT CEFBrowserManager::HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) {
	RESULT r = R_PASS;

	std::string strType;
	std::string strMethod;

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = GetCEFBrowserController(pCefBrowser->GetIdentifier());
	CN(pCEFBrowserController);

	strType = pMessageArguments->GetString(0);
	strMethod = pMessageArguments->GetString(1);

	//TODO: implement the other ones
	if (strType == "Form") {
		if (strMethod == "success") {
			CR(pCEFBrowserController->HandleDreamFormSuccess());
		}
		else if (strMethod == "cancel") {
			CR(pCEFBrowserController->HandleDreamFormCancel());
		}
		else if (strMethod == "setCredentials") {
			std::string strRefresh = pMessageArguments->GetString(2);
			std::string strAccess = pMessageArguments->GetString(3);
			CR(pCEFBrowserController->HandleDreamFormSetCredentials(strRefresh, strAccess));
		}
		else if (strMethod == "setEnvironmentId") {
			int environmentId = pMessageArguments->GetInt(2);
			CR(pCEFBrowserController->HandleDreamFormSetEnvironmentId(environmentId));
		}
	}
	else if (strType == "Browser") {
		if (strMethod == "canTabNext") {
			bool fTabNext = pMessageArguments->GetBool(2);
			CR(pCEFBrowserController->HandleCanTabNext(fTabNext));
		}
		else if (strMethod == "canTabPrevious") {
			bool fTabPrevious = pMessageArguments->GetBool(2);
			CR(pCEFBrowserController->HandleCanTabPrevious(fTabPrevious));
		}
		else if (strMethod == "isInputFocused") {
			bool fInputFocused = pMessageArguments->GetBool(2);
			CR(pCEFBrowserController->HandleIsInputFocused(fInputFocused));
		}
	}


Error:
	return r;

}

RESULT CEFBrowserManager::CEFManagerThread() {
	RESULT r = R_PASS;

	// Initialize CEF.

	// Enable High-DPI support on Windows 7 or newer.
	//CefEnableHighDPISupport();

	HINSTANCE hInstance = GetModuleHandle(nullptr);
	CefMainArgs cefMainArgs(hInstance);

	// Provide CEF with command-line arguments.
	int exitCode = CefExecuteProcess(cefMainArgs, nullptr, nullptr);
	DEBUG_LINEOUT("CefExecuteProcess returned %d", exitCode);

	// Specify CEF global settings here.
	CefSettings cefSettings;

	//CefRefPtr<CEFApp> pCEFApp = CefRefPtr<CEFApp>(m_pCEFApp.get());
	//CefRefPtr<CEFApp> pCEFApp = CefRefPtr<CEFApp>(new CEFApp);

	CefString(&cefSettings.browser_subprocess_path) = "DreamCef.exe";
	CefString(&cefSettings.locale) = "en";
	
	std::wstring wstrAppDataPath;
	PathManager::instance()->GetDreamPath(wstrAppDataPath, DREAM_PATH_TYPE::DREAM_PATH_ROAMING);
	wstrAppDataPath = wstrAppDataPath + L"CEFCache\\";
	CefString(&cefSettings.cache_path) = wstrAppDataPath;
	// CEF will create the Directory(s) if necessary

	cefSettings.remote_debugging_port = 8080;
	cefSettings.background_color = CefColorSetARGB(255, 255, 255, 255);
	
#ifdef _DEBUG
	cefSettings.single_process = true;
#endif

	cefSettings.multi_threaded_message_loop = true;
	cefSettings.windowless_rendering_enabled = true;

	CefRefPtr<CEFApp> pCEFApp = CefRefPtr<CEFApp>(CEFApp::instance());
	CN(pCEFApp);

	CR(pCEFApp->RegisterCEFAppObserver(this));

	CBM(CefInitialize(cefMainArgs, cefSettings, pCEFApp, nullptr), "CefInitialize error");

	DEBUG_LINEOUT("CefInitialize completed successfully");
	m_state = state::INITIALIZED;
	m_condBrowserInit.notify_one();

	/*
	DEBUG_LINEOUT("CEF Run message loop");
	CefRunMessageLoop();
	//*/

	DEBUG_LINEOUT("CEF thread complete...");

	// Success:
	return r;

Error:
	DEBUG_LINEOUT("CEF Initialization Error, shutting down");

	Shutdown();
	return r;
}

std::shared_ptr<WebBrowserController> CEFBrowserManager::MakeNewBrowser(int width, int height, const std::string& strURL) {
	RESULT r = R_PASS;
	std::shared_ptr<WebBrowserController> pWebBrowserController = nullptr;
	DEBUG_LINEOUT("CEFApp: MakeNewBrowser");

	CefRefPtr<CEFApp> pCEFApp = CefRefPtr<CEFApp>(CEFApp::instance());
	CN(pCEFApp);

	pWebBrowserController = pCEFApp->CreateBrowser(width, height, strURL);
	CN(pWebBrowserController);

// Success:
	return pWebBrowserController;

Error:
	if (pWebBrowserController != nullptr) {
		pWebBrowserController = nullptr;
	}

	return nullptr;
}

RESULT CEFBrowserManager::Shutdown() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEF force shutdown");

	//CR(ClearAllBrowserControllers());

	if (CEFHandler::instance()) {
		CEFHandler::instance()->CloseAllBrowsers(true);
	}

	while (CEFHandler::instance()->IsBrowserRunning()) {
		// empty stub
	}

	/*
	if (m_ServiceThread.joinable()) {
		m_ServiceThread.join();
	}
	*/

	DEBUG_LINEOUT("CEF Exited");

	try {
		CefShutdown();
	}
	catch (...) {
		DEBUG_LINEOUT("CEF hit exception");
	}

//Error:
	return r;
}