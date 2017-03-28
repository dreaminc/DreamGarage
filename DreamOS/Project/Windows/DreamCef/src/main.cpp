#include "windows.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"

#include "RESULT/EHM.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	RESULT r = R_PASS;

	CefMainArgs args(GetModuleHandle(NULL));

	int cefResult = CefExecuteProcess(args, nullptr, nullptr);
	CBM((cefResult >= 0), "CefExecuteProcess failed with error %d", cefResult);

Error:
	return (int)(r);
}