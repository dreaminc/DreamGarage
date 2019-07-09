#ifndef OSX_SANDBOX_APP_H_
#define OSX_SANDBOX_APP_H_

#include "RESULT/EHM.h"
#include "Sandbox/SandboxApp.h"

// DREAM OS
// DreamOS/Sandbox/OSX/OSXSandboxApp.h
// Dream OS OS X Sandbox App

#include <stdlib.h>
#include <string.h>

#include "OSXApplication.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define DEFAULT_FULLSCREEN false

class OpenGLImp;
// TODO: Move to Sandbox
//class OSXKeyboard;
//class OSXMouse;

class OSXSandboxApp : public SandboxApp {
public:
    OSXSandboxApp(char* pszClassName);
    ~OSXSandboxApp();
    
public:	// Sandbox Interface
    RESULT ShowSandbox();
    RESULT ShutdownSandbox();
    RESULT RecoverDisplayMode();
    
private:
    //static long __stdcall StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
    //long __stdcall WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp);
    //RESULT SetDeviceContext(HDC hDC);
    
    // TODO: Move to Sandbox?
    RESULT SetDimensions(int pxWidth, int pxHeight);
    
public:
    //HDC GetDeviceContext();
    //HWND GetWindowHandle();
    
    /* TODO: Move to Sandbox, use interfaces etc
    RESULT RegisterImpKeyboardEvents();
    RESULT RegisterImpMouseEvents();
     */
    
private:
    OSXApplication *m_pOSXApplication;
    
private:
    // TODO: Move to Sandbox?
    bool m_fFullscreen;
    
    //TODO: Move to Sandbox
    char* m_pszClassName;
    char* m_pszWindowTitle;
    
    // TODO: Move to Sandbox
    int m_pxWidth;
    int m_pxHeight;
    int m_posX;
    int m_posY;
    
    //WNDCLASSEX m_wndclassex;
    //HWND m_hwndWindow;
    
    //HDC m_hDC;					// Private GDI Device Context
    //HINSTANCE m_hInstance;		// Holds The Instance Of The Application
    
private:
    // TODO: Generalize the implementation architecture - still pretty bogged down in Win32
    // TODO: Move to Sandbox?
    OpenGLImp *m_pOpenGLImp;
    
public:
    // TODO: Move to Sandbox
    //OSXKeyboard *m_pOSXKeyboard;
    //OSXMouse *m_pOSXMouse;
};

#endif // ! OSX_SANDBOX_APP_H_
