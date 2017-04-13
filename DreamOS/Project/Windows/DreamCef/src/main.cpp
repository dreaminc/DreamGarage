#include "windows.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include "RESULT/EHM.h"

#include "WebBrowser/CEFBrowser/CEFApp.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	RESULT r = R_PASS;

	// Enable High-DPI support on Windows 7 or newer.
	//CefEnableHighDPISupport();

	CefMainArgs cefMainArgs(hInstance);

	// Optional implementation of the CefApp interface.
	//CefRefPtr<CEFApp> pCEFApp(new CEFApp);

	//return CefExecuteProcess(cefMainArgs, pCEFApp.get(), nullptr);
	return CefExecuteProcess(cefMainArgs, nullptr, nullptr);
}