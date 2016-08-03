#ifndef WINDOWS_64_APP_H_
#define WINDOWS_64_APP_H_

#include "RESULT/EHM.h"
#include "Sandbox/SandboxApp.h"

#include "TimeManager/TimeManager.h"
#include "Profiler/Profiler.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Windows64App.h
// Dream OS Windows 64 Sandbox

#include <windows.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <HMD/HMD.h>

#define DEFAULT_WIDTH 1920 / 2
#define DEFAULT_HEIGHT 1080 / 2

#define DEFAULT_FULLSCREEN false

class OpenGLImp;
class Win64Keyboard;
class Win64Mouse;

#include "Sense/SenseLeapMotion.h"

class Windows64App : public SandboxApp {
public:
	Windows64App(TCHAR* pszClassName);
	~Windows64App();

public:	// Sandbox Interface
	RESULT InitializeSandbox();
	RESULT Show();
	RESULT Shutdown();
	RESULT RecoverDisplayMode();

public:
	RESULT InitializePathManager();
	RESULT InitializeOpenGLRenderingContext();
	RESULT InitializeCloudController();
	RESULT InitializeHAL();

private:
	static LRESULT __stdcall StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
	LRESULT __stdcall WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
	RESULT SetDeviceContext(HDC hDC);
	RESULT SetDimensions(int pxWidth, int pxHeight);

public:
	HDC GetDeviceContext();
	HWND GetWindowHandle();

	RESULT RegisterImpKeyboardEvents();
	RESULT RegisterImpMouseEvents();
	RESULT RegisterImpLeapMotionEvents();

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

private:
	TimeManager	*m_pTimeManager;

	Profiler	m_profiler;

public:
	std::unique_ptr<SenseLeapMotion> m_pSenseLeapMotion;
	Win64Keyboard *m_pWin64Keyboard;
	Win64Mouse *m_pWin64Mouse;
	HMD *m_pHMD;
};

#endif // ! WINDOWS_64_APP_H_
