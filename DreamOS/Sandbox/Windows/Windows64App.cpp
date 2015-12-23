#include "Windows64App.h"

Windows64App::Windows64App(TCHAR* pszClassName) :
	m_pszClassName(pszClassName)
{
	// Default title
	m_pszWindowTitle = _T("Dream OS Sandbox");

	m_wndclassex.cbSize			= sizeof(WNDCLASSEX);
	m_wndclassex.style			= CS_DBLCLKS;
	m_wndclassex.lpfnWndProc	= WndProc;
	m_wndclassex.cbClsExtra		= NULL;
	m_wndclassex.cbWndExtra		= NULL;
	m_wndclassex.hInstance		= GetModuleHandle(0);
	m_wndclassex.hIcon			= LoadIcon(0, IDI_APPLICATION);
	m_wndclassex.hCursor		= LoadCursor(0, IDC_ARROW);
	m_wndclassex.hbrBackground	= HBRUSH(COLOR_WINDOW + 1);
	m_wndclassex.lpszMenuName	= NULL;
	m_wndclassex.lpszClassName	= m_pszClassName;
	m_wndclassex.hIconSm		= LoadIcon(0, IDI_APPLICATION);

	if(!RegisterClassEx(&m_wndclassex)) {
		MessageBox(NULL, _T("Failed to register sandbox window class"), _T("Dream OS Sandbox Error"), NULL);
		return;	// TODO: Use assert EHM
	}

	m_hwndWindow = CreateWindow(
		m_pszClassName,				// lpClassName
		m_pszWindowTitle,			// lpWindowName
		WS_OVERLAPPEDWINDOW,		// dwStyle
		CW_USEDEFAULT,				// X
		CW_USEDEFAULT,				// Y
		CW_USEDEFAULT,				// Width
		CW_USEDEFAULT,				// Height
		NULL,						// hWndParent
		NULL,						// hMenu
		GetModuleHandle(0),			// hInstance
		NULL						// lpParam
	);
}

Windows64App::~Windows64App() {
	// empty stub for now
}

long __stdcall Windows64App::WndProc(HWND window, unsigned int msg, WPARAM wp, LPARAM lp) {
	switch (msg) {
		case WM_DESTROY: {
			DEBUG_LINEOUT("Windows Sandbox being destroyed");
			PostQuitMessage(0);
			return 0L;
		} break;

		case WM_LBUTTONDOWN: {
			DEBUG_LINEOUT("Left mouse button down!");
		} break;

		default: {
			// Empty stub
		} break;
	}

	// Fall through for all messages for now
	return DefWindowProc(window, msg, wp, lp);
}

// Note this call will never return and will actually run the event loop
// TODO: Thread it?
RESULT Windows64App::ShowSandbox() {
	RESULT r = R_PASS;

	// TODO: Use EHM for this
	if (!m_hwndWindow) {
		MessageBox(NULL, _T("Failed to create windows sandbox"), _T("Dream OS Sandbox"), NULL);
		return R_FAIL;
	}

	// Show the window
	ShowWindow(m_hwndWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_hwndWindow);

	// Launch main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (RESULT)(msg.wParam);

Error:
	return r;
}
