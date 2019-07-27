#ifndef CEF_IMP_H_
#define CEF_IMP_H_

#include "core/ehm/EHM.h"

// TODO: The Cloud CEF files may be old and deprecated
// Dream Cloud CEF
// DreamOS/Cloud/CEFImp.h

// The CEF implementation

// TODO: CEF will need to be moved to a cross platform implementation
// and this will be the top level
#include <windows.h>

#include "CloudImp.h"

#include "webbrowser/cefbrowser/CEFApp.h"
// TODO: Wtf?
#include "CEFURLRequestController.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

class CEFImp : public CloudImp {
public:
	CEFImp() :
		CloudImp()
	{
		// empty
	}

	~CEFImp() {
		CEFShutdown();
	}

	RESULT CEFDoMessageLoopWork() {
		CefDoMessageLoopWork();
		return R_PASS;
	}

	RESULT CEFRunMessageLoop() {
		// Run the CEF message loop. This will block until CefQuitMessageLoop() is called.
		CefRunMessageLoop();
		return R_PASS;
	}

	RESULT CEFShutdown() {
		// Shut down CEF
		CefShutdown();
		return R_PASS;
	}

	RESULT CEFInitialize(HINSTANCE hInstance) {
		RESULT r = R_PASS;

		CefMainArgs CEFMainArgs(hInstance);
		void* CEFSandboxInfo = nullptr;

		int exitCode = CefExecuteProcess(CEFMainArgs, nullptr, CEFSandboxInfo);
		CBM((exitCode < 0), "CEF sub-process has completed");

		m_CEFApp = CefRefPtr<CEFApp>(new CEFApp);

		// Initialize CEF.
		CB(CefInitialize(CEFMainArgs, m_CEFSettings, m_CEFApp.get(), CEFSandboxInfo));

		DEBUG_LINEOUT("CEF Successfully initialized");

	Error:
		return r;
	}

	// Will simply update the message loop as needed
	RESULT Update() {
		RESULT r = R_PASS;

		CR(CEFDoMessageLoopWork());

	Error:
		return r;
	}

	RESULT CreateNewURLRequest(std::wstring& strURL) {
		RESULT r = R_PASS;

		CRM(m_CEFURLRequestController.CreateNewURLRequest(strURL), "Failed to create CEF URL request for %S", strURL.c_str());

	Error:
		return r;
	}

	// TODO: This should be a bit more robust
	bool IsConnected() {
		return false;
	}

private:
	CefRefPtr<CEFApp> m_CEFApp;
	CefSettings m_CEFSettings;

	CEFURLRequestController m_CEFURLRequestController;
};

#endif