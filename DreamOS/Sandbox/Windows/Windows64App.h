#ifndef WINDOWS_64_APP_H_
#define WINDOWS_64_APP_H_

#include "RESULT/EHM.h"
#include "Sandbox/SandboxApp.h"

// DREAM OS
// DreamOS/Sandbox/Windows/Windows64App.h
// Dream OS Windows 64 Sandbox

#include <windows.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <HMD/HMD.h>

#include <functional>

#define DEFAULT_WIDTH 1920 / 2
#define DEFAULT_HEIGHT 1080 / 2

#define DEFAULT_FULLSCREEN false

class OpenGLImp;
class Win64Keyboard;
class Win64Mouse;
class Win64CredentialManager;

class Windows64App : public SandboxApp {
public:
	enum WindowMessages {
		UI_THREAD_CALLBACK = WM_APP + 1
	};

public:
	Windows64App(TCHAR* pszClassName);
	~Windows64App();

public:	// Sandbox Interface
	virtual RESULT InitializeSandbox() override;
	virtual RESULT Show() override;
	virtual RESULT ShutdownSandbox() override;
	virtual RESULT RecoverDisplayMode() override;
	virtual RESULT HandleMessages() override;
	virtual RESULT SwapDisplayBuffers() override;

	virtual RESULT SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION sandboxWindowPosition) override;
	virtual long GetTickCount() override;
	virtual RESULT GetStackTrace() override;

	virtual	RESULT GetSandboxWindowSize(int &width, int &height) override;

	virtual RESULT InitializeCredentialManager() override;

	virtual RESULT SetKeyValue(std::wstring wstrKey, std::string strField, CredentialManager::type credType, bool fOverwrite) override;
	virtual RESULT GetKeyValue(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType) override;
	virtual RESULT RemoveKeyValue(std::wstring wstrKey, CredentialManager::type credType) override;

	virtual bool IsSandboxInternetConnectionValid() override;

public:
	virtual RESULT InitializePathManager(DreamOS *pDOSHandle) override;	
	RESULT InitializeOpenGLRenderingContext();
	RESULT InitializeCloudController();
	//RESULT InitializeHAL();
	RESULT InitializeKeyboard();
	RESULT InitializeMouse();
	RESULT InitializeGamepad();
	RESULT InitializeLeapMotion();	

private:
	static LRESULT __stdcall StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
	LRESULT __stdcall WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
	RESULT SetDeviceContext(HDC hDC);
	RESULT SetDimensions(int pxWidth, int pxHeight);

	// Handle a mouse event from a window's message. Return true if the message is handled, and false otherwise.
	bool HandleMouseEvent(const MSG&	windowMassage);
	// Handle a key event from a window's message. Return true if the message is handled, and false otherwise.
	bool HandleKeyEvent(const MSG&	windowMassage);

public:
	HDC GetDeviceContext();
	virtual HWND GetWindowHandle() override;

	RESULT RegisterUIThreadCallback(std::function<void(int msg_id, void* data)> m_fnUIThreadCallback);
	RESULT UnregisterUIThreadCallback();

private:
	bool m_fSentFrame = false;
	bool m_fFullscreen;
	long m_wndStyle;

	TCHAR* m_pszClassName;

	int m_pxWidth;
	int m_pxHeight;
	int m_posX;
	int m_posY;

	WNDCLASSEX m_wndclassex; 
	HWND m_hwndWindow;
	DWORD m_ThreadID;

	HDC m_hDC;					// Private GDI Device Context
	HINSTANCE m_hInstance;		// Holds The Instance Of The Application

	// DesktopCapture vars
	unsigned int m_desktoppxWidth = 0;
	unsigned int m_desktoppxHeight = 0;
	unsigned long m_pDesktopFrameData_n = 0;

private:
	std::function<void(int msg_id, void* data)> m_fnUIThreadCallback;

public:
	//std::unique_ptr<SenseLeapMotion> m_pSenseLeapMotion;
	//Win64Keyboard *m_pWin64Keyboard;
	//Win64Mouse *m_pWin64Mouse;
	//HMD *m_pHMD;
};

#endif // ! WINDOWS_64_APP_H_
