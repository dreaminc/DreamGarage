#ifndef CEF_IMP_H_
#define CEF_IMP_H_

#include "RESULT/EHM.h"
#include "CloudImp.h"

// DREAM OS
// DreamOS/Cloud/CEFImp.h
// The CEF implementation

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

// TODO: CEF will need to be moved to a cross platform implementation
// and this will be the top level
#include <windows.h>

#include "CEFApp.h"

class CEFImp : public CloudImp {
	CEFImp() :
		CloudImp()
	{
		// empty
	}

	~CEFImp() {
		// empty
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
		void* CEFSandboxInfo = NULL;

		int exitCode = CefExecuteProcess(CEFMainArgs, NULL, CEFSandboxInfo);
		CBM((exitCode < 0), "CEF sub-process has completed");

		m_CEFApp = CefRefPtr<CEFApp>(new CEFApp);

		// Initialize CEF.
		CefInitialize(CEFMainArgs, m_CEFSettings, m_CEFApp.get(), CEFSandboxInfo);

	Error:
		return r;
	}

private:
	CefRefPtr<CEFApp> m_CEFApp;
	CefSettings m_CEFSettings;
};

#endif