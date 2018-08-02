#include "windows.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include "RESULT/EHM.h"
#include "DreamLogger/DreamLogger.h"

#include "DreamCEFApp.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	RESULT r = R_PASS;

	// Enable High-DPI support on Windows 7 or newer.
	//CefEnableHighDPISupport();

	CefMainArgs cefMainArgs(hInstance);

	// Initialize Logging
	auto pLoggerInstance = DreamLogger::instanceNoPathMgr("DCEF");
	pLoggerInstance->Log(DreamLogger::Level::INFO, "Initialized DCEF logger");

	// Optional implementation of the CefApp interface.
	CefRefPtr<DreamCEFApp> pDreamCEFApp(new DreamCEFApp());
	return CefExecuteProcess(cefMainArgs, pDreamCEFApp.get(), nullptr);

	//return CefExecuteProcess(cefMainArgs, nullptr, nullptr);
}