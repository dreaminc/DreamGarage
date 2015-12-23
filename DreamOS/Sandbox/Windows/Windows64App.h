#ifndef WINDOWS_64_APP_H_
#define WINDOWS_64_APP_H_

#include "RESULT/EHM.h"
#include "Sandbox/SandboxApp.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

class Windows64App : public SandboxApp {
public:
	Windows64App(TCHAR* pszClassName);
	~Windows64App();

public:	// Sandbox Interface
	RESULT ShowSandbox();

private:
	static long __stdcall WndProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp);

private:
	TCHAR* m_pszClassName;
	TCHAR* m_pszWindowTitle;

	WNDCLASSEX m_wndclassex; 
	HWND m_hwndWindow;

	HDC m_hDC;					// Private GDI Device Context
	HINSTANCE m_hInstance;		// Holds The Instance Of The Application

};

#endif // ! WINDOWS_64_APP_H_
