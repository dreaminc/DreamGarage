#include "Windows64App.h"

#include "DreamLogger/DreamLogger.h"

#include "Project/Windows/DreamOS/resource.h"
#include "Sandbox/PathManagerFactory.h"
#include "HAL/opengl/OpenGLRenderingContextFactory.h"
#include "Cloud/CloudControllerFactory.h"

#include "./HAL/opengl/OpenGLImp.h"

#include "DreamOS.h"
#include "DDCIPCMessage.h"

#include "Win64Keyboard.h"
#include "Win64Mouse.h"

#include <string>

Windows64App::Windows64App(TCHAR* pszClassName) :
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

	// for now, the title includes the running folder for the purposes of debugging the updater.
	// TODO: once we make Dream versioning we will take this out
	WCHAR tmp[MAX_PATH];

	std::wstring title(tmp, GetModuleFileNameW(NULL, tmp, MAX_PATH));
	if (title.find_last_of(L"/\\") != std::wstring::npos) {

		title = title.substr(0, title.find_last_of(L"/\\"));

		if (title.find_last_of(L"/\\") != std::wstring::npos)
			title = title.substr(title.find_last_of(L"/\\") + 1);
	}

	title = L"Dream " + title;

	m_hInstance = GetModuleHandle(0);

	m_wndclassex.cbSize = sizeof(WNDCLASSEX);
	m_wndclassex.style = CS_DBLCLKS;
	m_wndclassex.lpfnWndProc = (WNDPROC)(StaticWndProc);
	m_wndclassex.cbClsExtra = NULL;
	m_wndclassex.cbWndExtra = NULL;
	m_wndclassex.hInstance = m_hInstance;
	m_wndclassex.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	m_wndclassex.hCursor = LoadCursor(0, IDC_ARROW);
	m_wndclassex.hbrBackground = HBRUSH(COLOR_WINDOW + 1);
	m_wndclassex.lpszMenuName = nullptr;
	m_wndclassex.lpszClassName = m_pszClassName;
	m_wndclassex.hIconSm = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));

	if (!RegisterClassEx(&m_wndclassex)) {
		MessageBox(nullptr, _T("Failed to register sandbox window class"), _T("Dream OS Sandbox Error"), NULL);
		return; // TODO: Use assert EHM
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

	m_ThreadID = ::GetCurrentThreadId();
	CB((m_ThreadID != 0));

	m_hwndWindow = CreateWindow(
		m_pszClassName,                                     // lpClassName
		title.c_str(),                                  // lpWindowName
		m_wndStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,     // dwStyle
		m_posX,                                             // X
		m_posY,                                             // Y
		m_pxWidth,                                          // Width
		m_pxHeight,                                         // Height
		nullptr,                                                // hWndParent
		nullptr,                                                // hMenu
		m_hInstance,                                        // hInstance
		this                                                // lpParam
	);

	// At this point WM_CREATE message is sent/received and rx-ed by WndProc

	//TODO: use this label
	//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

Windows64App::~Windows64App() {
	// empty
}

RESULT Windows64App::InitializeKeyboard() {
	RESULT r = R_PASS;

	m_pSenseKeyboard = new Win64Keyboard(this);
	CNM(m_pSenseKeyboard, "Failed to allocate keyboard");

Error:
	return r;
}

RESULT Windows64App::InitializeMouse() {
	RESULT r = R_PASS;

	m_pSenseMouse = new Win64Mouse(this);
	CNM(m_pSenseMouse, "Failed to allocate mouse");

	// Initialize Mouse
	// Remove mouse capture.
	// This effects the window responsiveness to drag, resize and focus event.
	//CRM(m_pSenseMouse->CaptureMouse(), "Failed to capture mouse");
	//CRM(m_pSenseMouse->CenterMousePosition(), "Failed to center mouse position");

Error:
	return r;
}

RESULT Windows64App::InitializeLeapMotion() {
	RESULT r = R_PASS;

	m_pSenseLeapMotion = std::unique_ptr<SenseLeapMotion>(new SenseLeapMotion());
	CNM(m_pSenseLeapMotion, "Failed to allocate leap motion");

	if (R_PASS == m_pSenseLeapMotion->InitLeapMotion()) {
		// Leap Motion successfully initialized
		CRM(RegisterImpLeapMotionEvents(), "Failed to register leap motion events");
	}

Error:
	return r;
}

/*
// TODO: Move this to sandbox
RESULT Windows64App::InitializeHAL() {
	RESULT r = R_PASS;

	// Setup OpenGL and Resize Windows etc
	CNM(m_hDC, "Can't start Sandbox with NULL Device Context");
	CNM(m_pCamera, "HAL depends on camera being set up");

	// Create and initialize OpenGL Imp
	// TODO: HAL factory pattern
	m_pHALImp = new OpenGLImp(m_pOpenGLRenderingContext);
	CNM(m_pHALImp, "Failed to create HAL Implementation");
	CVM(m_pHALImp, "HAL Implementation Invalid");

	CR(m_pHALImp->SetCamera(m_pCamera));

	CR(m_pHALImp->InitializeHAL());
	CR(m_pHALImp->InitializeRenderPipeline());

Error:
	return r;
}
*/

RESULT Windows64App::SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION sandboxWindowPosition) {
	RESULT r = R_PASS;

	switch (sandboxWindowPosition) {
	case SANDBOX_WINDOW_POSITION::LEFT: {
		m_posX = 0;
		m_posY = m_posY;
	} break;

	case SANDBOX_WINDOW_POSITION::RIGHT: {
		m_posX = GetSystemMetrics(SM_CXSCREEN) - m_pxWidth;
		m_posY = m_posY;
	} break;

	case SANDBOX_WINDOW_POSITION::TOP: {
		m_posX = m_posX;
		m_posY = 0;
	} break;

	case SANDBOX_WINDOW_POSITION::BOTTOM: {
		m_posX = m_posX;
		m_posY = GetSystemMetrics(SM_CYSCREEN) - m_pxHeight;
	} break;

	case SANDBOX_WINDOW_POSITION::CENTER: {
		m_posX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (m_pxWidth / 2);
		m_posY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (m_pxHeight / 2);
	} break;
	}

	CBM(SetWindowPos(m_hwndWindow, HWND_TOP, m_posX, m_posY, 0, 0, (UINT)(SWP_NOSIZE)), "Failed to position window");

Error:
	return r;
}

RESULT Windows64App::InitializeCloudController() {
	RESULT r = R_PASS;

	m_pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, (void*)(m_hInstance));
	CNM(m_pCloudController, "Cloud Controller failed to initialize");

	// TODO: Remove this code
	//CR(RegisterUIThreadCallback(m_pCloudController->GetUIThreadCallback()));

Error:
	return r;
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

	CNM(m_pPathManager, "Failed to allocated path manager");
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

	CR(ResizeViewport(viewport(m_pxWidth, m_pxHeight)));

Error:
	return r;
}

LRESULT __stdcall Windows64App::StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
	Windows64App *pApp = nullptr;

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

LRESULT __stdcall Windows64App::WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
	RESULT r = R_PASS;
	switch (msg) {
	case WM_CREATE: {
		HDC hDC = GetDC(hWindow);

		if (hDC == nullptr) {
			DEBUG_LINEOUT("Failed to capture Device Context");
			//PostQuitMessage(0);
			Shutdown();
			return 0L;
		}

		SetDeviceContext(hDC);
	} break;

	case WM_DESTROY: {
		DEBUG_LINEOUT("Windows Sandbox being destroyed");
		//PostQuitMessage(0);
		Shutdown();
		return 0L;
	} break;

	case WM_SIZE: {
		SetDimensions(LOWORD(lp), HIWORD(lp));
	} break;

	case WM_COPYDATA: {
		PCOPYDATASTRUCT pDataStruct;
		pDataStruct = (PCOPYDATASTRUCT)lp;

		if (pDataStruct->dwData == (unsigned long)DDCIPCMessage::type::FRAME) {
			unsigned long messageSize = pDataStruct->cbData;
			void* pMessageData;
			pMessageData = (unsigned char*)malloc(messageSize);
			memcpy(pMessageData, pDataStruct->lpData, messageSize);

			m_pDreamOSHandle->OnDesktopFrame(messageSize, pMessageData, m_desktoppxHeight, m_desktoppxWidth);

			free(pMessageData);
		}
		else if (pDataStruct->dwData == (unsigned long)DDCIPCMessage::type::RESIZE) {
			DDCIPCMessage *pMessageData;
			pMessageData = (DDCIPCMessage*)(pDataStruct->lpData);
			CNR(pMessageData, R_SKIPPED);

			m_desktoppxWidth = pMessageData->pxWidth;
			m_desktoppxHeight = pMessageData->pxHeight;
			m_pDesktopFrameData_n = m_pxWidth * m_pxHeight * 4;
		}
		else if (pDataStruct->dwData == (unsigned long)DDCIPCMessage::type::RESIZE) {
			DDCIPCMessage *pMessageData;
			pMessageData = (DDCIPCMessage*)(pDataStruct->lpData);
			CNR(pMessageData, R_SKIPPED);

			m_desktoppxWidth = pMessageData->pxWidth;
			m_desktoppxHeight = pMessageData->pxHeight;
			m_pDesktopFrameData_n = m_pxWidth * m_pxHeight * 4;
		}

		return true;
	} break;

	default: {
		// Empty stub
	} break;
	}

Error:
	// Fall through for all messages for now
	return DefWindowProc(hWindow, msg, wp, lp);
}

RESULT Windows64App::RegisterUIThreadCallback(std::function<void(int msg_id, void* data)> fnUIThreadCallback) {
	RESULT r = R_PASS;

	CB((m_fnUIThreadCallback == nullptr));
	m_fnUIThreadCallback = fnUIThreadCallback;

Error:
	return r;
}

RESULT Windows64App::UnregisterUIThreadCallback() {
	RESULT r = R_PASS;

	CN(m_fnUIThreadCallback);
	m_fnUIThreadCallback = nullptr;

Error:
	return r;
}

long Windows64App::GetTickCount() {
	return static_cast<long>(GetTickCount());
}

RESULT Windows64App::GetSandboxWindowSize(int &width, int &height) {
	width = m_pxWidth;
	height = m_pxHeight;

	return R_PASS;
}

RESULT Windows64App::InitializeSandbox() {
	RESULT r = R_PASS;

	// TODO: remove
	std::shared_ptr<DimObj> pSphere = nullptr;

	// TODO: Use EHM for this
	if (!m_hwndWindow) {
		MessageBox(NULL, _T("Failed to create windows sandbox"), _T("Dream OS Sandbox"), NULL);
		return R_FAIL;
	}

	/*
	// TODO: Move to Sandbox function
	CRM(RegisterImpKeyboardEvents(), "Failed to register keyboard events");
	CRM(RegisterImpMouseEvents(), "Failed to register mouse events");

	CRM(SetDimensions(m_pxWidth, m_pxHeight), "Failed to resize OpenGL Implementation");
	*/

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	composite *pCameraFrameOfReferenceComposite = m_pHALImp->MakeComposite();
	GetCamera()->SetFrameOfReferenceComposite(pCameraFrameOfReferenceComposite);
	CRM(AddObject(pCameraFrameOfReferenceComposite), "Failed to add composite camera frame of reference");

	// TODO: Move ALL to Sandbox function
	CRM(InitializeKeyboard(), "Failed to initialize keyboard");
	CRM(RegisterImpKeyboardEvents(), "Failed to register keyboard events");

	CRM(InitializeMouse(), "Failed to initialize mouse");
	CRM(RegisterImpMouseEvents(), "Failed to register mouse events");

	// This will only turn on Leap if connected at boot up
	if (GetSandboxConfiguration().fUseLeap) {
		CRM(InitializeLeapMotion(), "Failed to initialize leap motion");
	}

	CRM(RegisterImpControllerEvents(), "Failed to register vive controller events");

	CRM(ResizeViewport(m_viewport), "Failed to resize OpenGL Implemenation");

Error:
	return r;
}

RESULT Windows64App::HandleMessages() {
	RESULT r = R_PASS;

	MSG msg;

	if (PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE)) {
		if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
			HandleMouseEvent(msg);
		}
		else if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) {
			HandleKeyEvent(msg);
		}
		else if (WM_QUIT == msg.message) {
			Shutdown();
			CBR(false, (RESULT)(msg.wParam));
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

Error:
	return r;
}

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT Windows64App::Show() {
	RESULT r = R_PASS;

	// Show the window
	//CBM(ShowWindow(m_hwndWindow, SW_SHOWDEFAULT), "Failed to show win64app window");
	//CBM(UpdateWindow(m_hwndWindow), "Failed to update win64app window");

	ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hwndWindow);

	// TODO: Move this into it's own function
	HANDLE hCloseSplashScreenEvent = CreateEvent(NULL,        // no security
		TRUE,       // manual-reset event
		FALSE,      // not signaled
		(LPTSTR)L"CloseSplashScreenEvent"); // event name

	BOOL fResult = SetEvent(hCloseSplashScreenEvent);
	DOSLOG(INFO, "[Windows64App] signaling splash to close %v", (fResult ? "OK" : "FAIL"));

	CloseHandle(hCloseSplashScreenEvent);

	//return (RESULT)(msg.wParam);

	//Error:
	return r;
}

inline RESULT Windows64App::SwapDisplayBuffers() {
	if (SwapBuffers(m_hDC))
		return R_PASS;
	else
		return R_FAIL;
}

bool Windows64App::HandleMouseEvent(const MSG&  windowMessage) {
	bool fHandled = false;

	LPARAM lp = windowMessage.lParam;
	WPARAM wp = windowMessage.wParam;

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

	return fHandled;
}

bool Windows64App::HandleKeyEvent(const MSG& windowMessage) {
	bool fHandled = false;

	LPARAM lp = windowMessage.lParam;
	WPARAM wp = windowMessage.wParam;

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

	return fHandled;
}

RESULT Windows64App::ShutdownSandbox() {
	RESULT r = R_PASS;

	CR(SetSandboxRunning(false));

	// Release device context in use by rc
	wglMakeCurrent(m_hDC, nullptr);

	if (m_pCloudController != nullptr) {
		m_pCloudController->Stop();
		delete m_pCloudController;
		m_pCloudController = nullptr;
	}

	// Shutdown and delete GL Rendering Context
	if (m_pHALImp != nullptr) {
		CRM(m_pHALImp->Shutdown(), "Failed to shutdown HAL implemenation");
		delete m_pHALImp;
		m_pHALImp = nullptr;
	}

	wglMakeCurrent(nullptr, nullptr);

	PostQuitMessage(0);     // make sure the window will be destroyed

							// If full screen, change back to original res
	if (m_fFullscreen) {
		RecoverDisplayMode();
	}

Error:
	return r;
}

RESULT Windows64App::RecoverDisplayMode() {
	RESULT r = R_PASS;

	// TODO: What the hell is this?

	return r;
}
