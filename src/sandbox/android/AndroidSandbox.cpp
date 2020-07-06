#include "AndroidSandbox.h"

#include <string>
#include <netlistmgr.h>

#include "logger/DreamLogger.h"

#include "sandbox/PathManagerFactory.h"

// TODO: Shouldn't be here (no OGL)
#include "hal/ogl/OGLRenderingContextFactory.h"
#include "hal/ogl/OGLImp.h"

#include "cloud/CloudControllerFactory.h"

#include "os/DreamOS.h"

AndroidSandbox::AndroidSandbox(TCHAR* pszClassName) :
	m_pszClassName(pszClassName),
	m_pxWidth(DEFAULT_WIDTH),
	m_pxHeight(DEFAULT_HEIGHT),
	m_fFullscreen(DEFAULT_FULLSCREEN),
	m_wndStyle(WS_OVERLAPPEDWINDOW),
	m_hDC(nullptr),
	m_ThreadID(0),
	m_fnUIThreadCallback(nullptr)
{
	RESULT r = R_PASS;
	
Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

AndroidSandbox::~AndroidSandbox() {
	// empty
}

RESULT AndroidSandbox::InitializeKeyboard() {
	return R_NOT_IMPLEMENTED;
}

RESULT AndroidSandbox::InitializeMouse() {
	return R_NOT_IMPLEMENTED;
}

RESULT AndroidSandbox::InitializeGamepad() {
	return R_NOT_IMPLEMENTED;
}

RESULT AndroidSandbox::InitializeCredentialManager() {
	return R_NOT_IMPLEMENTED;
}

// TODO: Change arch so this is optional 
bool AndroidSandbox::IsSandboxInternetConnectionValid() {
	return false;
}

RESULT AndroidSandbox::SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION sandboxWindowPosition) {
	return R_NOT_IMPLEMENTED;
}

RESULT AndroidSandbox::InitializeCloudController() {
	RESULT r = R_PASS;

	m_pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, nullptr);
	CNM(m_pCloudController, "Cloud Controller failed to initialize");

Error:
	return r;
}

HDC AndroidSandbox::GetDeviceContext() {
	return nullptr;
}

HWND AndroidSandbox::GetWindowHandle() {
	return m_hwndWindow;
}

RESULT AndroidSandbox::InitializePathManager(DreamOS *pDOSHandle) {
	RESULT r = R_PASS;

	// Initialize Path Manager
	m_pDreamOSHandle = pDOSHandle;
	CNM(m_pDreamOSHandle, "DOS Handle not initialized");

	m_pPathManager = PathManagerFactory::MakePathManager(PATH_MANAGER_ANDROID, m_pDreamOSHandle);

	CNM(m_pPathManager, "Failed to allocated path manager");
	CVM(m_pPathManager, "Failed to initialize path manager");

	m_pPathManager->PrintPaths();

Error:
	return r;
}

// TODO: Should not be implementation specific 
RESULT AndroidSandbox::InitializeOGLRenderingContext() {
	RESULT r = R_PASS;

	m_pOGLRenderingContext = OGLRenderingContextFactory::MakeOGLRenderingContext(OPEN_GL_RC_ANDROID);
	CVM(m_pOGLRenderingContext, "Failed to initialize OpenGL Rendering Context");

	m_pOGLRenderingContext->SetParentApp(this);

Error:
	return r;
}

// This also kicks off the OpenGL implementation
RESULT AndroidSandbox::SetDeviceContext(HDC hDC) {
	return R_NOT_IMPLEMENTED;
}

RESULT AndroidSandbox::SetDimensions(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_pxWidth = pxWidth;
	m_pxHeight = pxHeight;

	CR(ResizeViewport(viewport(m_pxWidth, m_pxHeight)));

Error:
	return r;
}

RESULT AndroidSandbox::RegisterUIThreadCallback(std::function<void(int msg_id, void* data)> fnUIThreadCallback) {
	RESULT r = R_PASS;

	CB((m_fnUIThreadCallback == nullptr));
	m_fnUIThreadCallback = fnUIThreadCallback;

Error:
	return r;
}

RESULT AndroidSandbox::UnregisterUIThreadCallback() {
	RESULT r = R_PASS;

	CN(m_fnUIThreadCallback);
	m_fnUIThreadCallback = nullptr;

Error:
	return r;
}

RESULT AndroidSandbox::GetSandboxWindowSize(int &width, int &height) {
	width = m_pxWidth;
	height = m_pxHeight;

	return R_PASS;
}

RESULT AndroidSandbox::InitializeSandbox() {
	RESULT r = R_PASS;

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	// TODO: Is this still needed here?
	composite *pCameraFrameOfReferenceComposite = m_pHALImp->MakeComposite();
	GetCamera()->SetFrameOfReferenceComposite(pCameraFrameOfReferenceComposite);
	CRM(AddObject(pCameraFrameOfReferenceComposite), "Failed to add composite camera frame of reference");

	// TODO: Move ALL to Sandbox function
	CRM(InitializeKeyboard(), "Failed to initialize keyboard");
	CRM(RegisterImpKeyboardEvents(), "Failed to register keyboard events");

	CRM(InitializeMouse(), "Failed to initialize mouse");
	CRM(RegisterImpMouseEvents(), "Failed to register mouse events");
	
	if (GetSandboxConfiguration().fUseGamepad) {
		CRM(InitializeGamepad(), "Failed to initialize gamepad");
	}

	// This will only turn on Leap if connected at boot up
	if (GetSandboxConfiguration().fUseLeap) {
		CRM(InitializeLeapMotion(), "Failed to initialize leap motion");
	}

	CRM(RegisterImpControllerEvents(), "Failed to register vive controller events");

	CRM(ResizeViewport(m_viewport), "Failed to resize OpenGL Implemenation");
	
	// Android needed?
	//CBM(InitializeCredentialManager(), "Failed to initialize Credential Manager");

Error:
	return r;
}

RESULT AndroidSandbox::HandleMessages() {
	return R_NOT_IMPLEMENTED;
}

// Note this call will never return and will actually run the event loop
RESULT AndroidSandbox::Show() {
	RESULT r = R_PASS;

	// Show the window
	
	if (GetSandboxConfiguration().fHideWindow == true) {
		//ShowWindow(m_hwndWindow, SW_HIDE);
	}
	else {
		//ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	}

	//UpdateWindow(m_hwndWindow);

Error:
	return r;
}

inline RESULT AndroidSandbox::SwapDisplayBuffers() {
	//if (SwapBuffers(m_hDC))
	//	return R_PASS;
	//else
	//	return R_FAIL;

	return R_NOT_IMPLEMENTED;
}

bool AndroidSandbox::HandleMouseEvent(const MSG&  windowMessage) {
	bool fHandled = false;
	RESULT r = R_SUCCESS;

	LPARAM lp = windowMessage.lParam;
	WPARAM wp = windowMessage.wParam;

	CNM(m_pSenseMouse, "Mouse not valid");

	switch (windowMessage.message) {
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Left mouse button down!");
		m_pSenseMouse->UpdateMouseState((windowMessage.message == WM_LBUTTONUP) ? SENSE_MOUSE_LEFT_BUTTON_UP : SENSE_MOUSE_LEFT_BUTTON_DOWN, xPos, yPos, (int)(wp));
	} break;

	case WM_LBUTTONDBLCLK: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Left mouse button dbl click!");
		//m_pSenseMouse->UpdateMouseState(SENSE_MOUSE_LEFT_BUTTON, xPos, yPos, (int)(wp));
	} break;

	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Right mouse button down!");
		m_pSenseMouse->UpdateMouseState((windowMessage.message == WM_RBUTTONUP) ? SENSE_MOUSE_RIGHT_BUTTON_UP : SENSE_MOUSE_RIGHT_BUTTON_DOWN, xPos, yPos, (int)(wp));
	} break;

	case WM_RBUTTONDBLCLK: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;

		//DEBUG_LINEOUT("Right mouse button dbl click!");
		// TODO: Add this to the SenseMouse
	} break;

	case WM_MBUTTONUP:
	case WM_MBUTTONDOWN: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Middle mouse button down!");
		m_pSenseMouse->UpdateMouseState((windowMessage.message == WM_MBUTTONUP) ? SENSE_MOUSE_MIDDLE_BUTTON_UP : SENSE_MOUSE_MIDDLE_BUTTON_DOWN, xPos, yPos, (int)(wp));
	} break;

	case WM_MBUTTONDBLCLK: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;

		//DEBUG_LINEOUT("Middle mouse button dbl click!");
		// TODO: Add this to the SenseMouse
	} break;

	case WM_MOUSEWHEEL: {
		fHandled = true;
		int wheel = static_cast<int>((int16_t)((wp >> 16) & 0xFFFF) / 120.0f);
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Mousewheel %d!", wheel);
		//m_pWin64Mouse->UpdateMouseState(SENSE_MOUSE_WHEEL, xPos, yPos, (int)(wp));
		m_pSenseMouse->UpdateMouseState(SENSE_MOUSE_WHEEL, xPos, yPos, wheel);
	} break;

	case WM_MOUSEMOVE: {
		fHandled = true;
		int xPos = (lp >> 0) & 0xFFFF;
		int yPos = (lp >> 16) & 0xFFFF;
		//DEBUG_LINEOUT("Middle mouse button down!");
		m_pSenseMouse->UpdateMouseState(SENSE_MOUSE_MOVE, xPos, yPos, (int)(wp));
	} break;
	}

Success:
	return fHandled;

Error:
	return false;
}

bool AndroidSandbox::HandleKeyEvent(const MSG& windowMessage) {
	bool fHandled = false;
	RESULT r = R_SUCCESS;

	LPARAM lp = windowMessage.lParam;
	WPARAM wp = windowMessage.wParam;

	CNM(m_pSenseKeyboard, "No Keyboard");

	switch (windowMessage.message) {
	case WM_KEYUP: {
		fHandled = true;
		m_pSenseKeyboard->UpdateKeyState((SenseVirtualKey)(windowMessage.wParam), false);
	} break;

	case WM_KEYDOWN: {
		fHandled = true;
		m_pSenseKeyboard->UpdateKeyState((SenseVirtualKey)(windowMessage.wParam), true);
	} break;

	case WM_CHAR: {
		unsigned int lparam = windowMessage.lParam;
		unsigned char scanCode = (lparam >> 16);

		fHandled = true;
		m_pSenseKeyboard->NotifyTextTyping(static_cast<SenseVirtualKey>(MapVirtualKey(scanCode, MAPVK_VSC_TO_VK)), windowMessage.wParam, true);
	} break;
	}

Success:
	return fHandled;

Error:
	return r;
}

RESULT AndroidSandbox::ShutdownSandbox() {
	RESULT r = R_PASS;

	CR(SetSandboxRunning(false));

	// Release device context in use by rc
	// wglMakeCurrent(m_hDC, nullptr);

	if (m_pCloudController != nullptr) {
		m_pCloudController->Stop();
		delete m_pCloudController;
		m_pCloudController = nullptr;
	}

	DOSLOG(INFO, "Cloud controller shutdown");

	// Shutdown and delete GL Rendering Context
	if (m_pHALImp != nullptr) {
		CRM(m_pHALImp->Shutdown(), "Failed to shutdown HAL implemenation");
		delete m_pHALImp;
		m_pHALImp = nullptr;
	}
	DOSLOG(INFO, "HAL shutdown");

	//wglMakeCurrent(nullptr, nullptr);

Error:
	return r;
}

RESULT AndroidSandbox::RecoverDisplayMode() {
	return R_NOT_IMPLEMENTED;
}
