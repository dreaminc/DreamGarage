#include "CEFBrowserManager.h"

#include "CEFBrowserController.h"
#include "CEFApp.h"

#include "CEFHandler.h"

RESULT CEFBrowserManager::Initialize() {
	RESULT r = R_PASS;

	m_ServiceThread = std::thread(&CEFBrowserManager::CEFManagerThread, this);
	std::unique_lock<std::mutex> lockCEFBrowserInitialization(m_mutex);

	m_condBrowserInit.wait(lockCEFBrowserInitialization);

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
		if (pWebBrowserController->PollNewDirtyFrames()) {

			/*
			[&](unsigned char *pBufferOutput, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) -> bool {
			pWebBrowserController.second.pTexture->Update(pBufferOutput, width, height, texture::PixelFormat::BGRA);
			// poll whole frame and stop iterations
			return false;
			*/
		
		}
	}

//Error:
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

	CefRefPtr<CEFApp> pCEFApp = CefRefPtr<CEFApp>(new CEFApp);

	CefString(&cefSettings.browser_subprocess_path) = "DreamCef.exe";
	CefString(&cefSettings.locale) = "en";
	cefSettings.remote_debugging_port = 8080;
	//cefSettings.multi_threaded_message_loop = true;

	CBM(CefInitialize(cefMainArgs, cefSettings, pCEFApp.get(), nullptr), "CefInitialize error");

	DEBUG_LINEOUT("CefInitialize completed successfully");
	m_state = state::INITIALIZED;
	m_condBrowserInit.notify_one();

	///*
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

RESULT CEFBrowserManager::Shutdown() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEF force shutdown");

	if (CEFHandler::instance()) {
		CEFHandler::instance()->CloseAllBrowsers(true);
	}

	CB((m_ServiceThread.joinable()));
	m_ServiceThread.join();

Error:
	DEBUG_LINEOUT("CEF Exited");
	CefShutdown();

	return r;
}