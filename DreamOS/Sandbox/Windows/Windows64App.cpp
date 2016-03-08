#include "Windows64App.h"
#include "Sandbox/PathManagerFactory.h"
#include "HAL/opengl/OpenGLRenderingContextFactory.h"

#include "./HAL/opengl/OpenGLImp.h"

#include "Win64Keyboard.h"
#include "Win64Mouse.h"

Windows64App::Windows64App(TCHAR* pszClassName) :
	m_pszClassName(pszClassName),
	m_pxWidth(DEFAULT_WIDTH),
	m_pxHeight(DEFAULT_HEIGHT),
	m_fFullscreen(DEFAULT_FULLSCREEN),
	m_wndStyle(WS_OVERLAPPEDWINDOW),
	m_hDC(NULL)
{
	RESULT r = R_PASS;

	// Default title
	m_pszWindowTitle = _T("Dream OS Sandbox");

	m_hInstance = GetModuleHandle(0);

	m_wndclassex.cbSize = sizeof(WNDCLASSEX);
	m_wndclassex.style = CS_DBLCLKS;
	m_wndclassex.lpfnWndProc = StaticWndProc;
	m_wndclassex.cbClsExtra = NULL;
	m_wndclassex.cbWndExtra = NULL;
	m_wndclassex.hInstance = m_hInstance;
	m_wndclassex.hIcon = LoadIcon(0, IDI_APPLICATION);
	m_wndclassex.hCursor = LoadCursor(0, IDC_ARROW);
	m_wndclassex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	m_wndclassex.lpszMenuName = NULL;
	m_wndclassex.lpszClassName = m_pszClassName;
	m_wndclassex.hIconSm = LoadIcon(0, IDI_APPLICATION);

	if (!RegisterClassEx(&m_wndclassex)) {
		MessageBox(NULL, _T("Failed to register sandbox window class"), _T("Dream OS Sandbox Error"), NULL);
		return;	// TODO: Use assert EHM
	}

	// TODO: Improve this
	m_posX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_pxWidth / 2);
	m_posY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_pxHeight / 2);

	// Create a full-screen application if requested
	if (m_fFullscreen) {
		m_wndStyle = WS_POPUP;
		m_posX = 0;
		m_posY = 0;

		// Change resolution before the window is created
		// TODO: ?
		//SysSetDisplayMode(screenw, screenh, SCRDEPTH);
	}

	m_hwndWindow = CreateWindow(
		m_pszClassName,										// lpClassName
		m_pszWindowTitle,									// lpWindowName
		m_wndStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,		// dwStyle
		m_posX,												// X
		m_posY,												// Y
		m_pxWidth,											// Width
		m_pxHeight,											// Height
		NULL,												// hWndParent
		NULL,												// hMenu
		m_hInstance,										// hInstance
		this												// lpParam
	);

	// TODO: Move into Sandbox virtual function
	// Create the Keyboard and Mouse
	m_pWin64Keyboard = new Win64Keyboard(this);
	m_pWin64Mouse = new Win64Mouse(this);

	// Initialize Mouse 
	m_pWin64Mouse->CaptureMouse();
	m_pWin64Mouse->CenterMousePosition();

	// At this point WM_CREATE message is sent/received and rx-ed by WndProc

	Validate();
	return;

Error:
	Invalidate();
	return;
}

Windows64App::~Windows64App() {
	// empty stub for now
}

HDC Windows64App::GetDeviceContext() {
	return m_hDC;
}

HWND Windows64App::GetWindowHandle() {
	return m_hwndWindow;
}

RESULT Windows64App::InitializePathManager() {
	RESULT r = R_PASS;

	// Initialize Path Manager
	m_pPathManager = PathManagerFactory::MakePathManager(PATH_MANAGER_WIN32);
	CVM(m_pPathManager, "Failed to initialize path manager");

	m_pPathManager->PrintPaths();

Error:
	return r;
}

RESULT Windows64App::InitializeOpenGLRenderingContext() {
	RESULT r = R_PASS;

	m_pOpenGLRenderingContext = OpenGLRenderingContextFactory::MakeOpenGLRenderingContext(OPEN_GL_RC_WIN32);
	CVM(m_pOpenGLRenderingContext, "Failed to initialize OpenGL Rendering Context");

	m_pOpenGLRenderingContext->SetParentApp(this);

Error:
	return r;
}

// This also kicks off the OpenGL implementation
RESULT Windows64App::SetDeviceContext(HDC hDC) {
	m_hDC = hDC;
	return R_PASS;
}

RESULT Windows64App::SetDimensions(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_pxWidth = pxWidth;
	m_pxHeight = pxHeight;

	// OpenGL Resize the view after the window had been resized
	CRM(m_pOpenGLImp->Resize(m_pxWidth, m_pxHeight), "Failed to resize OpenGL Implemenation");

Error:
	return r;
}

long __stdcall Windows64App::StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
	Windows64App *pApp = NULL;

	// Get pointer to window
	if (msg == WM_CREATE) {
		pApp = (Windows64App*)((LPCREATESTRUCT)lp)->lpCreateParams;
		//SetWindowLongPtr(hWindow, GWL_USERDATA, (LONG_PTR)pApp);
		SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG_PTR)pApp);
	}
	else {
		//pApp = (Windows64App *)GetWindowLongPtr(hWindow, GWL_USERDATA);
		pApp = (Windows64App *)GetWindowLongPtr(hWindow, GWLP_USERDATA);
		if (!pApp) 
			return DefWindowProc(hWindow, msg, wp, lp);
	}

	//pApp->m_hwndWindow = hWindow;
	return pApp->WndProc(hWindow, msg, wp, lp);
}

long __stdcall Windows64App::WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_CREATE: {
			HDC hDC = GetDC(hWindow);

			if (hDC == NULL) {
				DEBUG_LINEOUT("Failed to capture Device Context");
				PostQuitMessage(0);
				return 0L;
			}

			SetDeviceContext(hDC);
		} break;

		case WM_DESTROY: {
			DEBUG_LINEOUT("Windows Sandbox being destroyed");
			PostQuitMessage(0);
			return 0L;
		} break;

		case WM_SIZE: {
			SetDimensions(LOWORD(lp), HIWORD(lp));
		} break;

		// Mouse
		/*
		// This needs to go into the event loop apparently
		// TODO: Alternative approaches welcome
		case WM_MOUSEMOVE: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Mouse move %d %d!", xPos, yPos);
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_MOVE, xPos, yPos, (int)(wp));
		} break;
		*/
		
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Left mouse button down!");
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_LEFT_BUTTON, xPos, yPos, (int)(wp));
		} break;

		case WM_LBUTTONDBLCLK: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Left mouse button dbl click!");
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_LEFT_BUTTON, xPos, yPos, (int)(wp));
		} break;
		
		case WM_RBUTTONUP:
		case WM_RBUTTONDOWN: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Right mouse button down!");
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_RIGHT_BUTTON, xPos, yPos, (int)(wp));
		} break;

		case WM_RBUTTONDBLCLK: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;

			//DEBUG_LINEOUT("Right mouse button dbl click!");
			// TODO: Add this to the SenseMouse
		} break;
		
		case WM_MBUTTONUP:
		case WM_MBUTTONDOWN: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Middle mouse button down!");
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_MIDDLE_BUTTON, xPos, yPos, (int)(wp));
		} break;
		
		case WM_MBUTTONDBLCLK: {
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;

			//DEBUG_LINEOUT("Middle mouse button dbl click!");
			// TODO: Add this to the SenseMouse
		} break;
			
		case WM_MOUSEWHEEL: {
			int wheel = ((int16_t)((wp >> 16) & 0xFFFF) / 120.0f);
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Mousewheel %d!", wheel);
			//m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_WHEEL, xPos, yPos, (int)(wp));
			m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_WHEEL, xPos, yPos, wheel);
		} break;

		// Keyboard
		// Update the Keyboard in WndProc rather than in the run loop
		// TODO: This should be different arch for native
		case WM_KEYDOWN: {
			m_pWin64Keyboard->UpdateKeyState((SK_SCAN_CODE)(wp), true);
		} break;

		case WM_KEYUP: {
			m_pWin64Keyboard->UpdateKeyState((SK_SCAN_CODE)(wp), false);
		} break;

		default: {
			// Empty stub
		} break;
	}

	// Fall through for all messages for now
	return DefWindowProc(hWindow, msg, wp, lp);
}

RESULT Windows64App::RegisterImpKeyboardEvents() {
	RESULT r = R_PASS;

	CR(m_pWin64Keyboard->RegisterSubscriber(VK_LEFT, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_UP, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_DOWN, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_RIGHT, m_pOpenGLImp));

	for (int i = 0; i < 26; i++) {
		CR(m_pWin64Keyboard->RegisterSubscriber((SK_SCAN_CODE)('A' + i), m_pOpenGLImp));
	}

Error:
	return r;
}

RESULT Windows64App::RegisterImpMouseEvents() {
	RESULT r = R_PASS;

	CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_MOVE, m_pOpenGLImp));
	CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON, m_pOpenGLImp));
	CR(m_pWin64Mouse->RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON, m_pOpenGLImp));

Error:
	return r;
}

#include "HAL/opengl/OGLVolume.h"

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT Windows64App::ShowSandbox() {
	RESULT r = R_PASS;

	// TODO: Use EHM for this
	if (!m_hwndWindow) {
		MessageBox(NULL, _T("Failed to create windows sandbox"), _T("Dream OS Sandbox"), NULL);
		return R_FAIL;
	}

	// Setup OpenGL and Resize Windows etc
	CNM(m_hDC, "Can't start Sandbox with NULL Device Context");
	//m_pOpenGLImp = new OpenGLImp(m_hDC);
	
	m_pOpenGLImp = new OpenGLImp(m_pOpenGLRenderingContext);
	CNM(m_pOpenGLImp, "Failed to create OpenGL Implementation");

	CRM(SetDimensions(m_posX, m_posY), "Failed to resize OpenGL Implemenation");

	DEBUG_LINEOUT("Launching Win64App Sandbox ...");

	// TODO: Move to Sandbox function
	CR(RegisterImpKeyboardEvents(), "Failed to register keyboard events");
	CR(RegisterImpMouseEvents(), "Failed to register mouse events");

	// Show the window
	ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hwndWindow);

	// TODO: Proper loading of the scene here?
	OGLVolume *pVolume = new OGLVolume(m_pOpenGLImp, 2.0f);
	m_pSceneGraph->PushObject(pVolume);

	// Launch main message loop
	MSG msg;
	bool fQuit = false;

	while (!fQuit) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				fQuit = true;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Update the mouse
		// TODO: This is wrong architecture, this should
		// be parallel 
		m_pWin64Mouse->UpdateMousePosition();

		if(m_pOpenGLImp != NULL)
			m_pOpenGLImp->Render(m_pSceneGraph);

		// Swap buffers
		SwapBuffers(m_hDC);

		if (GetAsyncKeyState(VK_ESCAPE))
			ShutdownSandbox();
	}

	return (RESULT)(msg.wParam);

Error:
	return r;
}

RESULT Windows64App::ShutdownSandbox() {
	RESULT r = R_PASS;

	// Release device context in use by rc
	wglMakeCurrent(m_hDC, NULL);

	// Shutdown and delete GL Rendering Context
	if (m_pOpenGLImp != NULL) {
		CRM(m_pOpenGLImp->ShutdownImplementaiton(), "Failed to shutdown opengl implemenation");
		delete m_pOpenGLImp;
		m_pOpenGLImp = NULL;
	}

	PostQuitMessage(0);		// make sure the window will be destroyed

	// If full screen, change back to original res
	if (m_fFullscreen)	
		RecoverDisplayMode();

Error:
	return r;
}

RESULT Windows64App::RecoverDisplayMode() {
	RESULT r = R_PASS;

	// TODO: What the hell is this?

Error:
	return r;
}
