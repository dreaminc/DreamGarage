#include "Logger/Logger.h"
#include "Project/Windows/DreamOS/resource.h"
#include "Windows64App.h"
#include "Sandbox/PathManagerFactory.h"
#include "HAL/opengl/OpenGLRenderingContextFactory.h"
#include "Cloud/CloudControllerFactory.h"

#include "./HAL/opengl/OpenGLImp.h"

#include "Win64Keyboard.h"
#include "Win64Mouse.h"
#include <HMD/HMDFactory.h>

#include <string>

#include "DreamConsole/DreamConsole.h"

Windows64App::Windows64App(TCHAR* pszClassName) :
	m_pszClassName(pszClassName),
	m_pxWidth(DEFAULT_WIDTH),
	m_pxHeight(DEFAULT_HEIGHT),
	m_fFullscreen(DEFAULT_FULLSCREEN),
	m_wndStyle(WS_OVERLAPPEDWINDOW),
	m_hDC(nullptr),
	m_pHMD(nullptr),
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

	m_ThreadID = ::GetCurrentThreadId();
	CB((m_ThreadID != 0));

	m_hwndWindow = CreateWindow(
		m_pszClassName,										// lpClassName
		title.c_str(),									// lpWindowName
		m_wndStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,		// dwStyle
		m_posX,												// X
		m_posY,												// Y
		m_pxWidth,											// Width
		m_pxHeight,											// Height
		nullptr,												// hWndParent
		nullptr,												// hMenu
		m_hInstance,										// hInstance
		this												// lpParam
	);

	// TODO: Move into Sandbox virtual function
	// Create the Keyboard and Mouse
	/*
	m_pWin64Keyboard = new Win64Keyboard(this);
	m_pWin64Mouse = new Win64Mouse(this);

	// Initialize Mouse 
	m_pWin64Mouse->CaptureMouse();
	m_pWin64Mouse->CenterMousePosition();
	*/

	// Sense Leap Motion Device (TODO: temporarily here!)
	//m_pSenseLeapMotion = std::make_unique<SenseLeapMotion>();
	//CN(m_pSenseLeapMotion);

	// At this point WM_CREATE message is sent/received and rx-ed by WndProc

	// Initialize Time Manager
	m_pTimeManager = new TimeManager();
	CNM(m_pTimeManager, "Failed to allocate Time Manager");
	CVM(m_pTimeManager, "Failed to validate Time Manager");
//TODO: use this label
//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

Windows64App::~Windows64App() {
	if (m_pTimeManager != nullptr) {
		delete m_pTimeManager;
		m_pTimeManager = nullptr;
	}
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
	// This effects the window responsivenes to drag, resize and focus event.
	//CRM(m_pSenseMouse->CaptureMouse(), "Failed to capture mouse");
	//CRM(m_pSenseMouse->CenterMousePosition(), "Failed to center mouse position");

Error:
	return r;
}

RESULT Windows64App::InitializeLeapMotion() {
	RESULT r = R_PASS;

	m_pSenseLeapMotion = std::unique_ptr<SenseLeapMotion>(new SenseLeapMotion());
	CNM(m_pSenseLeapMotion, "Failed to allocate leap motion");

Error:
	return r;
}

RESULT Windows64App::InitializeHAL() {
	RESULT r = R_PASS;

	// Setup OpenGL and Resize Windows etc
	CNM(m_hDC, "Can't start Sandbox with NULL Device Context");

	// Create and initialize OpenGL Imp
	// TODO: HAL factory pattern
	m_pHALImp = new OpenGLImp(m_pOpenGLRenderingContext);
	CNM(m_pHALImp, "Failed to create HAL Implementation");
	CVM(m_pHALImp, "HAL Implementation Invalid");

Error:
	return r;
}

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
	CRM(m_pHALImp->Resize(m_pxWidth, m_pxHeight), "Failed to resize OpenGL Implemenation");

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
	switch (msg) {
		case WM_CREATE: {
			HDC hDC = GetDC(hWindow);

			if (hDC == nullptr) {
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

		default: {
			// Empty stub
		} break;
	}

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

	// HMD
	// TODO: This should go into (as well as the above) into the Sandbox
	// This needs to be done after GL set up

	//m_pHMD = HMDFactory::MakeHMD(HMD_OVR, this, m_pHALImp, m_pxWidth, m_pxHeight);
	//m_pHMD = HMDFactory::MakeHMD(HMD_OPENVR, this, m_pHALImp, m_pxWidth, m_pxHeight);
	
	// Move this into Sandbox config
	if (m_fCheckHMD) {
		m_pHMD = HMDFactory::MakeHMD(HMD_ANY_AVAILABLE, this, m_pHALImp, m_pxWidth, m_pxHeight);
	
		if (m_pHMD != nullptr) {
			CRM(m_pHALImp->SetHMD(m_pHMD), "Failed to initialize stereo frame buffers");
		}
	}

	composite *pCameraFrameOfReferenceComposite = m_pHALImp->MakeComposite();
	m_pHALImp->GetCamera()->SetFrameOfReferenceComposite(pCameraFrameOfReferenceComposite);
	CRM(AddObject(pCameraFrameOfReferenceComposite), "Failed to add composite camera frame of reference");

	// TODO: Move ALL to Sandbox function
	CRM(InitializeKeyboard(), "Failed to initialize keyboard");
	CRM(RegisterImpKeyboardEvents(), "Failed to register keyboard events");

	CRM(InitializeMouse(), "Failed to initialize mouse");
	CRM(RegisterImpMouseEvents(), "Failed to register mouse events");

	// This will only turn on Leap if connected at boot up
	CRM(InitializeLeapMotion(), "Failed to initialize leap motion");
	if (m_pSenseLeapMotion != nullptr && m_pSenseLeapMotion->IsConnected()) {
		CRM(RegisterImpLeapMotionEvents(), "Failed to register leap motion events");
	}

	CRM(SetDimensions(m_pxWidth, m_pxHeight), "Failed to resize OpenGL Implemenation");

Error:
	return r;
}

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT Windows64App::Show() {
	RESULT r = R_PASS;

	// Show the window
	ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hwndWindow);
	
	// Launch main message loop
	MSG msg;
	bool fQuit = false;

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	// TODO: This should be moved to the sandbox
	while (!fQuit) {
		if (PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE)) {
			bool fHandled = false;

			if (msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) {
				fHandled = HandleMouseEvent(msg);
			}
			else if (msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST) {
				fHandled = HandleKeyEvent(msg);
			}
			else if (WM_QUIT == msg.message)
				break;

			if (!fHandled)
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

#ifdef CEF_ENABLED
		// Update Network
		CR(m_pCloudController->Update());
#endif

		// Time Manager
		CR(m_pTimeManager->Update());

		// Update Callback
		if (m_fnUpdateCallback != nullptr) {
			CR(m_fnUpdateCallback());
		}

		// Update the mouse
		// TODO: This is wrong architecture, this should
		// be parallel 
		// TODO: Update Sense etc
		//m_pWin64Mouse->UpdateMousePosition();


		if (m_pHMD != nullptr) {
			m_pHMD->UpdateHMD();
		}

		// Update Scene 
		CR(m_pSceneGraph->UpdateScene());

		// Update HMD
		if (m_pHMD != nullptr) {
			m_pHALImp->SetCameraOrientation(m_pHMD->GetHMDOrientation());
			m_pHALImp->SetCameraPositionDeviation(m_pHMD->GetHMDTrackerDeviation());
		}

		//m_pOpenGLImp->RenderStereo(m_pSceneGraph);
		//m_pOpenGLImp->Render(m_pSceneGraph);

		///*
		// Send to the HMD
		// TODO reorganize Render functions
		// need to be re-architected so that the HMD functions are called after all of the 
		// GL functions per eye.
		if (m_pHMD != nullptr) {
			//m_pHALImp->RenderStereoFramebuffers(m_pSceneGraph);
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_LEFT);
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_RIGHT);
			m_pHMD->SubmitFrame();
			m_pHMD->RenderHMDMirror();
		}
		else {
			// Render Scene
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_MONO);
		}
		//*/
	
		// Swap buffers
		SwapBuffers(m_hDC);

		DreamConsole::GetConsole()->OnFrameRendered();

		if (GetAsyncKeyState(VK_ESCAPE) && !DreamConsole::GetConsole()->IsInForeground()) {
			Shutdown();
			fQuit = true;
		}
	}

	return (RESULT)(msg.wParam);
Error:
	return r;
}

bool Windows64App::HandleMouseEvent(const MSG&	windowMassage) {
	bool fHandled = false;

	LPARAM lp = windowMassage.lParam;
	WPARAM wp = windowMassage.wParam;

	switch (windowMassage.message) {
		case WM_LBUTTONUP:
		case WM_LBUTTONDOWN: {
			fHandled = true;
			int xPos = (lp >> 0) & 0xFFFF;
			int yPos = (lp >> 16) & 0xFFFF;
			//DEBUG_LINEOUT("Left mouse button down!");
			m_pSenseMouse->UpdateMouseState((windowMassage.message == WM_LBUTTONUP) ? SENSE_MOUSE_LEFT_BUTTON_UP : SENSE_MOUSE_LEFT_BUTTON_DOWN, xPos, yPos, (int)(wp));
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
			m_pSenseMouse->UpdateMouseState((windowMassage.message == WM_RBUTTONUP) ? SENSE_MOUSE_RIGHT_BUTTON_UP : SENSE_MOUSE_RIGHT_BUTTON_DOWN, xPos, yPos, (int)(wp));
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
			m_pSenseMouse->UpdateMouseState((windowMassage.message == WM_MBUTTONUP) ? SENSE_MOUSE_MIDDLE_BUTTON_UP : SENSE_MOUSE_MIDDLE_BUTTON_DOWN, xPos, yPos, (int)(wp));
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

bool Windows64App::HandleKeyEvent(const MSG& windowMassage) {
	bool fHandled = false;

	LPARAM lp = windowMassage.lParam;
	WPARAM wp = windowMassage.wParam;

	switch (windowMassage.message) {
		case WM_KEYUP: {
			fHandled = true;
			m_pSenseKeyboard->UpdateKeyState((SK_SCAN_CODE)(windowMassage.wParam), false);
		} break;

		case WM_KEYDOWN: {
			fHandled = true;
			m_pSenseKeyboard->UpdateKeyState((SK_SCAN_CODE)(windowMassage.wParam), true);

			/*
			if ((SK_SCAN_CODE)(wp) == (SK_SCAN_CODE)('L')) {
				HUD_OUT("Key 'L' is pressed");

				if (m_pCloudController != nullptr) {
					// Attempt to connect to the first peer in the list
					m_pCloudController->LoginUser();
				}
			}
			*/

			// TODO: Clean this up / remove it eventually (if anything, put it into the handler)
			/*
			// DEBUG: Bypass for connect to cloud
			if ((SK_SCAN_CODE)(wp) == (SK_SCAN_CODE)('H')) {
				if (m_pCloudController != nullptr) {
					// Attempt to connect to the first peer in the list
					//m_pCloudController->SendDataChannelStringMessage(NULL, std::string("hi"));

					m_pCloudController->SendUpdateHeadMessage(NULL, point(1, 2, 3), quaternion(1, 2, 3, 4));
				}
			}
			else if ((SK_SCAN_CODE)(wp) == (SK_SCAN_CODE)('L')) {
				HUD_OUT("Key 'L' is pressed");

				if (m_pCloudController != nullptr) {
					// Attempt to connect to the first peer in the list
					m_pCloudController->LoginUser();
				}
			}
			else if ((SK_SCAN_CODE)(wp) == (SK_SCAN_CODE)('P')) {
				if (m_pCloudController != nullptr) {
					// Attempt to connect to the first peer in the list
					m_pCloudController->PrintEnvironmentPeerList();
				}
			}
			else if ((SK_SCAN_CODE)(wp) == (SK_SCAN_CODE)('I')) {
				if (m_pCloudController != nullptr) {
					// Attempt to connect to the first peer in the list
					m_pCloudController->AddIceCandidates();
				}
			}
			else if ((SK_SCAN_CODE)(wp) == SK_SCAN_CODE::SK_LEFT) {
				SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION::LEFT);
			}
			else if ((SK_SCAN_CODE)(wp) == SK_SCAN_CODE::SK_RIGHT) {
				SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION::RIGHT);
			}
			else if ((SK_SCAN_CODE)(wp) == SK_SCAN_CODE::SK_UP) {
				SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION::TOP);
			}
			else if ((SK_SCAN_CODE)(wp) == SK_SCAN_CODE::SK_DOWN) {
				SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION::BOTTOM);
			}
			*/
		} break;
	}
	
	return fHandled;
}

RESULT Windows64App::Shutdown() {
	RESULT r = R_PASS;

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

	PostQuitMessage(0);		// make sure the window will be destroyed

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


