#ifndef WINDOWS_64_APP_H_
#define WINDOWS_64_APP_H_

#include "RESULT/EHM.h"
#include "Sandbox/SandboxApp.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define DEFAULT_FULLSCREEN false

class Windows64App : public SandboxApp {
public:
	Windows64App(TCHAR* pszClassName);
	~Windows64App();

public:	// Sandbox Interface
	RESULT ShowSandbox();
	RESULT ShutdownSandbox();
	RESULT RecoverDisplayMode();

private:
	static long __stdcall WndProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp);
	RESULT SetDeviceContext(HDC hDC);
	RESULT SetDimensions(int pxWidth, int pxHeight);

private:
	bool m_fFullscreen;
	long m_wndStyle;

	TCHAR* m_pszClassName;
	TCHAR* m_pszWindowTitle;

	int m_pxWidth;
	int m_pxHeight;
	int m_posX;
	int m_posY;

	WNDCLASSEX m_wndclassex; 
	HWND m_hwndWindow;

	HDC m_hDC;					// Private GDI Device Context
	HINSTANCE m_hInstance;		// Holds The Instance Of The Application

};

#endif // ! WINDOWS_64_APP_H_
