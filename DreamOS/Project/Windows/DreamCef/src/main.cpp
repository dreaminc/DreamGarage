#include "windows.h"

#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"


int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	CefMainArgs args(GetModuleHandle(NULL));

	int result = CefExecuteProcess(args, nullptr, nullptr);

	if (result >= 0) // child proccess has endend, so exit.
	{
		return result;
	}

	if (result == -1)
	{
		// parent process proccess.
	}

	return 0;
}