#include "Win64Sandbox.h"

#include <string>
#include <netlistmgr.h>

#include "logger/DreamLogger.h"

#include "sandbox/win64/resource.h"

#include "sandbox/PathManagerFactory.h"

// TODO: Shouldn't be here (no OGL)
#include "hal/ogl/OGLRenderingContextFactory.h"
#include "hal/ogl/OGLImp.h"

#include "cloud/CloudControllerFactory.h"

#include "os/DreamOS.h"

#include "dreamdesktop/DDCIPCMessage.h"

#include "sense/SenseLeapMotion.h"

#include "sense/win64/Win64Keyboard.h"
#include "sense/win64/Win64Mouse.h"
#include "sense/win64/Win64GamepadController.h"

#include "Win64CredentialManager.h"
#include "Win64NamedPipeClient.h"
#include "Win64NamedPipeServer.h"

Win64Sandbox::Win64Sandbox(TCHAR* pszClassName) :
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
		MessageBox(nullptr, L"Failed to register sandbox window class", L"Dream OS Sandbox Error", NULL);
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

	// Get hardware ID from profile
	HW_PROFILE_INFO hwProfInfo;
	GetCurrentHwProfile(&hwProfInfo);
	m_strHardwareID = hwProfInfo.szHwProfileGuid;

	// At this point WM_CREATE message is sent/received and rx-ed by WndProc

	SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);	// High priority is where things like Task List reside, ignoring load on the operating system.
																		// Use extreme care when using the high-priority class, because a high-priority class application can use nearly all available CPU time.
	//TODO: use this label
Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

Win64Sandbox::~Win64Sandbox() {
	// empty
}

RESULT Win64Sandbox::InitializeKeyboard() {
	RESULT r = R_PASS;

	m_pSenseKeyboard = new Win64Keyboard(this);
	CNM(m_pSenseKeyboard, "Failed to allocate keyboard");

Error:
	return r;
}

RESULT Win64Sandbox::InitializeMouse() {
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

RESULT Win64Sandbox::InitializeGamepad() {
	RESULT r = R_PASS;

	m_pSenseGamepad = new Win64GamepadController(this);
	CNM(m_pSenseGamepad, "Failed to allocate gamepad");

Error:
	return r;
}

RESULT Win64Sandbox::InitializeLeapMotion() {
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

RESULT Win64Sandbox::InitializeCredentialManager() {
	RESULT r = R_PASS;

	m_pCredentialManager = std::unique_ptr<Win64CredentialManager>(new Win64CredentialManager());
	CN(m_pCredentialManager);

Error:
	return r;
}

RESULT Win64Sandbox::SetKeyValue(std::wstring wstrKey, std::string strCred, CredentialManager::type credType, bool fOverwrite) {
	RESULT r = R_PASS;

	CR(m_pCredentialManager->SetKeyValue(wstrKey, strCred, credType, fOverwrite));

Error:
	return r;
}

RESULT Win64Sandbox::GetKeyValue(std::wstring wstrKey, std::string& strOut, CredentialManager::type credType) {
	RESULT r = R_PASS;

	CR(m_pCredentialManager->GetKeyValue(wstrKey, strOut, credType));

Error:
	return r;
}

RESULT Win64Sandbox::RemoveKeyValue(std::wstring wstrKey, CredentialManager::type credType) {
	RESULT r = R_PASS;

	CR(m_pCredentialManager->RemoveKeyValue(wstrKey, credType));

Error:
	return r;
}

bool Win64Sandbox::IsSandboxInternetConnectionValid() {
	INetworkListManager* pNetworkListManager = nullptr;
	if (SUCCEEDED(CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (LPVOID*)&pNetworkListManager))) {
		// Creating the object was successful.	
		VARIANT_BOOL fIsConnected = 0;	// 0 == false, -1 == true;
		pNetworkListManager->get_IsConnectedToInternet(&fIsConnected);
		// The function call succeeded.	
		if (fIsConnected == VARIANT_TRUE) {
			CoUninitialize();
			return true;
		}
		else {
			CoUninitialize();
			return false;
		}
	}
	// Uninitialize COM.	
	// (This should be called on application shutdown.)	
	CoUninitialize();
	return false;
}

// Sandbox Objects
std::shared_ptr<NamedPipeClient> Win64Sandbox::MakeNamedPipeClient(std::wstring strPipename) {
	RESULT r = R_PASS;

	std::shared_ptr<NamedPipeClient> pRetPipeClient = nullptr;

	pRetPipeClient = std::make_shared<Win64NamedPipeClient>(strPipename);
	CN(pRetPipeClient);

	CR(pRetPipeClient->Initialize());

	return pRetPipeClient;

Error:
	if (pRetPipeClient != nullptr) {
		pRetPipeClient = nullptr;
	}

	return nullptr;
}

std::shared_ptr<NamedPipeServer> Win64Sandbox::MakeNamedPipeServer(std::wstring strPipename) {
	RESULT r = R_PASS;

	std::shared_ptr<NamedPipeServer> pRetPipeServer = nullptr;

	pRetPipeServer = std::make_shared<Win64NamedPipeServer>(strPipename);
	CN(pRetPipeServer);

	CR(pRetPipeServer->Initialize());

	return pRetPipeServer;

Error:
	if (pRetPipeServer != nullptr) {
		pRetPipeServer = nullptr;
	}

	return nullptr;
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
	m_pHALImp = new OGLImp(m_pOpenGLRenderingContext);
	CNM(m_pHALImp, "Failed to create HAL Implementation");
	CVM(m_pHALImp, "HAL Implementation Invalid");

	CR(m_pHALImp->SetCamera(m_pCamera));

	CR(m_pHALImp->InitializeHAL());
	CR(m_pHALImp->InitializeRenderPipeline());

Error:
	return r;
}
*/

RESULT Win64Sandbox::SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION sandboxWindowPosition) {
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

RESULT Win64Sandbox::InitializeCloudController() {
	RESULT r = R_PASS;

	m_pCloudController = CloudControllerFactory::MakeCloudController(CLOUD_CONTROLLER_NULL, (void*)(m_hInstance));
	CNM(m_pCloudController, "Cloud Controller failed to initialize");

	// TODO: Remove this code
	//CR(RegisterUIThreadCallback(m_pCloudController->GetUIThreadCallback()));

Error:
	return r;
}

HDC Win64Sandbox::GetDeviceContext() {
	return m_hDC;
}

HWND Win64Sandbox::GetWindowHandle() {
	return m_hwndWindow;
}

RESULT Win64Sandbox::InitializePathManager(DreamOS *pDOSHandle) {
	RESULT r = R_PASS;

	// Initialize Path Manager
	m_pDreamOSHandle = pDOSHandle;
	CNM(m_pDreamOSHandle, "DOS Handle not initialized");

	m_pPathManager = PathManagerFactory::MakePathManager(PATH_MANAGER_WIN32, m_pDreamOSHandle);

	CNM(m_pPathManager, "Failed to allocated path manager");
	CVM(m_pPathManager, "Failed to initialize path manager");

	m_pPathManager->PrintPaths();

Error:
	return r;
}

// TODO: Should not be implementation specific 
RESULT Win64Sandbox::InitializeOGLRenderingContext() {
	RESULT r = R_PASS;

	m_pOGLRenderingContext = OGLRenderingContextFactory::MakeOGLRenderingContext(OPEN_GL_RC_WIN32);
	CVM(m_pOGLRenderingContext, "Failed to initialize OpenGL Rendering Context");

	m_pOGLRenderingContext->SetParentApp(this);

Error:
	return r;
}

// This also kicks off the OpenGL implementation
RESULT Win64Sandbox::SetDeviceContext(HDC hDC) {
	m_hDC = hDC;
	return R_PASS;
}

RESULT Win64Sandbox::SetDimensions(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_pxWidth = pxWidth;
	m_pxHeight = pxHeight;

	CR(ResizeViewport(viewport(m_pxWidth, m_pxHeight)));

Error:
	return r;
}

LRESULT __stdcall Win64Sandbox::StaticWndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
	Win64Sandbox *pApp = nullptr;

	// Get pointer to window
	if (msg == WM_CREATE) {
		pApp = (Win64Sandbox*)((LPCREATESTRUCT)lp)->lpCreateParams;
		//SetWindowLongPtr(hWindow, GWL_USERDATA, (LONG_PTR)pApp);
		SetWindowLongPtr(hWindow, GWLP_USERDATA, (LONG_PTR)pApp);
	}

	else {
		//pApp = (Windows64App *)GetWindowLongPtr(hWindow, GWL_USERDATA);
		pApp = (Win64Sandbox *)GetWindowLongPtr(hWindow, GWLP_USERDATA);
		if (!pApp)
			return DefWindowProc(hWindow, msg, wp, lp);
	}

	//pApp->m_hwndWindow = hWindow;
	return pApp->WndProc(hWindow, msg, wp, lp);
}

LRESULT __stdcall Win64Sandbox::WndProc(HWND hWindow, unsigned int msg, WPARAM wp, LPARAM lp) {
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
			/*
			void* pMessageData;
			pMessageData = (unsigned char*)malloc(messageSize);
			memcpy(pMessageData, pDataStruct->lpData, messageSize);
			*/
			m_pDreamOSHandle->OnDesktopFrame(messageSize, pDataStruct->lpData, m_desktoppxHeight, m_desktoppxWidth);

			//free(pMessageData);
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

RESULT Win64Sandbox::RegisterUIThreadCallback(std::function<void(int msg_id, void* data)> fnUIThreadCallback) {
	RESULT r = R_PASS;

	CB((m_fnUIThreadCallback == nullptr));
	m_fnUIThreadCallback = fnUIThreadCallback;

Error:
	return r;
}

RESULT Win64Sandbox::UnregisterUIThreadCallback() {
	RESULT r = R_PASS;

	CN(m_fnUIThreadCallback);
	m_fnUIThreadCallback = nullptr;

Error:
	return r;
}

long Win64Sandbox::GetTickCount() {
	return static_cast<long>(GetTickCount());
}

RESULT Win64Sandbox::GetStackTrace() {
	return R_NOT_IMPLEMENTED;
}

RESULT Win64Sandbox::GetSandboxWindowSize(int &width, int &height) {
	width = m_pxWidth;
	height = m_pxHeight;

	return R_PASS;
}

RESULT Win64Sandbox::InitializeSandbox() {
	RESULT r = R_PASS;

	// TODO: Use EHM for this
	if (!m_hwndWindow) {
		MessageBox(NULL, L"Failed to create windows sandbox", L"Dream OS Sandbox", NULL);
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

	CBM(InitializeCredentialManager(), "Failed to initialize Credential Manager");

Error:
	return r;
}

RESULT Win64Sandbox::HandleMessages() {
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

	if (m_pSenseGamepad != nullptr) {
		m_pSenseGamepad->UpdateGamepad();
	}

Error:
	return r;
}

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT Win64Sandbox::Show() {
	RESULT r = R_PASS;

	// Show the window
	//CBM(ShowWindow(m_hwndWindow, SW_SHOWDEFAULT), "Failed to show win64app window");
	//CBM(UpdateWindow(m_hwndWindow), "Failed to update win64app window");
	if (GetSandboxConfiguration().fHideWindow == true) {
		ShowWindow(m_hwndWindow, SW_HIDE);
	}
	else {
		ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	}

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

inline RESULT Win64Sandbox::SwapDisplayBuffers() {
	if (SwapBuffers(m_hDC))
		return R_PASS;
	else
		return R_FAIL;
}

bool Win64Sandbox::HandleMouseEvent(const MSG&  windowMessage) {
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

bool Win64Sandbox::HandleKeyEvent(const MSG& windowMessage) {
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

RESULT Win64Sandbox::ShutdownSandbox() {
	RESULT r = R_PASS;

	CR(SetSandboxRunning(false));

	// Release device context in use by rc
	wglMakeCurrent(m_hDC, nullptr);

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

	wglMakeCurrent(nullptr, nullptr);

	PostQuitMessage(0);     // make sure the window will be destroyed

							// If full screen, change back to original res
	if (m_fFullscreen) {
		RecoverDisplayMode();
	}

Error:
	return r;
}

RESULT Win64Sandbox::RecoverDisplayMode() {
	RESULT r = R_PASS;

	// TODO: What the hell is this?

	return r;
}
