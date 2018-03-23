#include "RESULT/EHM.h"
#include <limits.h>
#include <vector>

#include "D3D11DesktopDuplicationDisplayManager.h"
#include "D3D11DesktopDuplicationManager.h"
#include "D3D11DesktopDuplicationOutputManager.h"
#include "D3D11DesktopDuplicationThreadManager.h"
#include "DDCIPCMessage.h"

#include <windows.h>
#include <windowsx.h>

// Below are lists of errors expect from Dxgi API calls when a transition event like mode change, PnpStop, PnpStart
// desktop switch, TDR or session disconnect/reconnect. In all these cases we want the application to clean up the threads that process
// the desktop updates and attempt to recreate them.
// If we get an error that is not on the appropriate list then we exit the application

// Globals
D3D11DesktopDuplicationOutputManager OutMgr;
HWND pWindowHandle;		// These can go to the .h if we want
HWND g_pDreamHandle;
bool g_fStartSending = false;

// These are the errors we expect from general Dxgi API due to a transition
HRESULT SystemTransitionsExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	static_cast<HRESULT>(WAIT_ABANDONED),
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIOutput1::DuplicateOutput due to a transition
HRESULT CreateDuplicationExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	static_cast<HRESULT>(E_ACCESSDENIED),
	DXGI_ERROR_UNSUPPORTED,
	DXGI_ERROR_SESSION_DISCONNECTED,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIOutputDuplication methods due to a transition
HRESULT FrameInfoExpectedErrors[] = {
	DXGI_ERROR_DEVICE_REMOVED,
	DXGI_ERROR_ACCESS_LOST,
	S_OK                                    // Terminate list with zero valued HRESULT
};

// These are the errors we expect from IDXGIAdapter::EnumOutputs methods due to outputs becoming stale during a transition
HRESULT EnumOutputsExpectedErrors[] = {
	DXGI_ERROR_NOT_FOUND,
	S_OK                                    // Terminate list with zero valued HRESULT
};


//
// Forward Declarations
//
DWORD WINAPI DDProc(_In_ void* Param);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool ProcessCmdline(_Out_ INT* outputToDuplicate);
void ShowHelp();

//
// Class for progressive waits
//
typedef struct {
	UINT    WaitTime;
	UINT    WaitCount;
} WAIT_BAND;

#define WAIT_BAND_COUNT 3
#define WAIT_BAND_STOP 0

class DynamicWait {
public:
	DynamicWait();
	~DynamicWait();

	void Wait();

private:

	static const WAIT_BAND   m_WaitBands[WAIT_BAND_COUNT];

	// Period in seconds that a new wait call is considered part of the same wait sequence
	static const UINT       m_WaitSequenceTimeInSeconds = 2;

	UINT                    m_CurrentWaitBandIdx;
	UINT                    m_WaitCountInCurrentBand;
	LARGE_INTEGER           m_QPCFrequency;
	LARGE_INTEGER           m_LastWakeUpTime;
	BOOL                    m_QPCValid;
};
const WAIT_BAND DynamicWait::m_WaitBands[WAIT_BAND_COUNT] = {
	{ 250, 20 },
	{ 2000, 60 },
	{ 5000, WAIT_BAND_STOP }   // Never move past this band
};

DynamicWait::DynamicWait() : m_CurrentWaitBandIdx(0), m_WaitCountInCurrentBand(0) {
	m_QPCValid = QueryPerformanceFrequency(&m_QPCFrequency);
	m_LastWakeUpTime.QuadPart = 0L;
}

DynamicWait::~DynamicWait() {
	// empty
}

void DynamicWait::Wait() {
	LARGE_INTEGER CurrentQPC = { 0 };

	// Is this wait being called with the period that we consider it to be part of the same wait sequence
	QueryPerformanceCounter(&CurrentQPC);
	if (m_QPCValid && (CurrentQPC.QuadPart <= (m_LastWakeUpTime.QuadPart + (m_QPCFrequency.QuadPart * m_WaitSequenceTimeInSeconds))))
	{
		// We are still in the same wait sequence, lets check if we should move to the next band
		if ((m_WaitBands[m_CurrentWaitBandIdx].WaitCount != WAIT_BAND_STOP) && (m_WaitCountInCurrentBand > m_WaitBands[m_CurrentWaitBandIdx].WaitCount))
		{
			m_CurrentWaitBandIdx++;
			m_WaitCountInCurrentBand = 0;
		}
	}
	else
	{
		// Either we could not get the current time or we are starting a new wait sequence
		m_WaitCountInCurrentBand = 0;
		m_CurrentWaitBandIdx = 0;
	}

	// Sleep for the required period of time
	Sleep(m_WaitBands[m_CurrentWaitBandIdx].WaitTime);

	// Record the time we woke up so we can detect wait sequences
	QueryPerformanceCounter(&m_LastWakeUpTime);
	m_WaitCountInCurrentBand++;
}

//
// Program entry point
//
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	HRESULT r = S_OK;
	int outputToDuplicate;

	// Synchronization
	HANDLE UnexpectedErrorEvent = nullptr;
	HANDLE ExpectedErrorEvent = nullptr;
	HANDLE TerminateThreadsEvent = nullptr;

	HCURSOR Cursor = nullptr;
	WNDCLASSEXW Wc;
	RECT WindowRect = { 0, 0, 800, 600 };
	D3D11DesktopDuplicationThreadManager ThreadMgr;
	RECT DeskBounds;
	UINT OutputCount;
	UINT texturepxWidth = 0;
	UINT texturepxHeight = 0;
	//UINT texturepxWidth = 0;
	//UINT texturepxHeight = 0;
	// Message loop (attempts to update screen when no other messages to process)
	MSG msg = { 0 };
	bool FirstTime = true;
	bool Occluded = true;
	DynamicWait DynamicWait;

	// Window
	pWindowHandle = nullptr;

	bool CmdResult = ProcessCmdline(&outputToDuplicate);
	if (!CmdResult) {
		ShowHelp();
		return 0;
	}

	// Event used by the threads to signal an unexpected error and we want to quit the app
	UnexpectedErrorEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	CNM(UnexpectedErrorEvent, "UnexpectedErrorEvent creation failed");

	// Event for when a thread encounters an expected error
	ExpectedErrorEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	CNM(ExpectedErrorEvent, "ExpectedErrorEvent creation failed");

	// Event to tell spawned threads to quit
	TerminateThreadsEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	CNM(TerminateThreadsEvent, "TerminateThreadsEvent creation failed");

	// Load simple cursor
	Cursor = LoadCursor(nullptr, IDC_ARROW);
	CNM(Cursor, "Cursor load failed");

	// Register class
	Wc.cbSize = sizeof(WNDCLASSEXW);
	Wc.style = CS_HREDRAW | CS_VREDRAW;
	Wc.lpfnWndProc = WndProc;
	Wc.cbClsExtra = 0;
	Wc.cbWndExtra = 0;
	Wc.hInstance = hInstance;
	Wc.hIcon = nullptr;
	Wc.hCursor = Cursor;
	Wc.hbrBackground = nullptr;
	Wc.lpszMenuName = nullptr;
	Wc.lpszClassName = L"ddasample";
	Wc.hIconSm = nullptr;
	CBM(RegisterClassExW(&Wc), "Window class registration failed");

	// Create window
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);
	pWindowHandle = CreateWindowW(L"ddasample", L"DreamDesktopDuplication",
		WS_OVERLAPPEDWINDOW,
		0, 0,
		WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	CNM(pWindowHandle, "Window creation failed");

	DestroyCursor(Cursor);

	ShowWindow(pWindowHandle, nCmdShow);
	UpdateWindow(pWindowHandle);

	while (WM_QUIT != msg.message) {
		DUPL_RETURN Ret = DUPL_RETURN_SUCCESS;
		// Data to send to dream
		BYTE *pBuffer = nullptr;
		UINT pxWidth = 0;
		UINT pxHeight = 0;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == OCCLUSION_STATUS_MSG) {
				// Present may not be occluded now so try again
				Occluded = false;
			}
			else {
				// Process window messages
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (WaitForSingleObjectEx(UnexpectedErrorEvent, 0, FALSE) == WAIT_OBJECT_0) {
			// Unexpected error occurred so exit the application
			break;
		}
		else if (FirstTime || WaitForSingleObjectEx(ExpectedErrorEvent, 0, FALSE) == WAIT_OBJECT_0) {
			if (!FirstTime) {
				// Terminate other threads
				SetEvent(TerminateThreadsEvent);
				ThreadMgr.WaitForThreadTermination();
				ResetEvent(TerminateThreadsEvent);
				ResetEvent(ExpectedErrorEvent);

				// Clean up
				ThreadMgr.Clean();
				OutMgr.CleanRefs();

				// As we have encountered an error due to a system transition we wait before trying again, using this dynamic wait
				// the wait periods will get progressively long to avoid wasting too much system resource if this state lasts a long time
				DynamicWait.Wait();
			}
			else {
				// First time through the loop so nothing to clean up
				FirstTime = false;
			}

			// Re-initialize
			Ret = OutMgr.InitOutput(pWindowHandle, outputToDuplicate, &OutputCount, &DeskBounds);
			if (Ret == DUPL_RETURN_SUCCESS) {
				HANDLE SharedHandle = OutMgr.GetSharedHandle();
				if (SharedHandle) {
					Ret = ThreadMgr.Initialize(outputToDuplicate, OutputCount, UnexpectedErrorEvent, ExpectedErrorEvent, TerminateThreadsEvent, SharedHandle, &DeskBounds);
				}
				else {
					DisplayMsg(L"Failed to get handle of shared surface", L"Error", S_OK);
					Ret = DUPL_RETURN_ERROR_UNEXPECTED;
				}
			}

			// We start off in occluded state and we should immediate get a occlusion status window message
			Occluded = true;
		}

		else {
			// Nothing else to do, so try to present to write out to window if not occluded
			if (!Occluded) {
				Ret = OutMgr.UpdateApplicationWindow(ThreadMgr.GetPointerInfo(), &Occluded, &pBuffer, pxWidth, pxHeight);
			}
		}
		//if((pBuffer != nullptr) && (pBuffer[0] != '\0')) {
		if (g_fStartSending && (pBuffer != nullptr) /*&& (pBuffer [0] != '\0')*/) {
			//MessageBox(g_pDreamHandle, L"Start sending", L"Status", MB_OK);
			DDCIPCMessage ddcMessage;

			if (pxHeight == texturepxHeight && pxWidth == texturepxWidth) {
				ddcMessage.m_msgType = DDCIPCMessage::type::FRAME;
				COPYDATASTRUCT desktopCDS;

				desktopCDS.dwData = (unsigned long)ddcMessage.m_msgType;
				desktopCDS.cbData = (pxHeight * pxWidth * 4);
				desktopCDS.lpData = pBuffer;

				SendMessage(g_pDreamHandle, WM_COPYDATA, (WPARAM)(HWND)pWindowHandle, (LPARAM)(LPVOID)&desktopCDS);
			}

			else {
				texturepxWidth = pxWidth;
				texturepxHeight = pxHeight;

				ddcMessage.pxHeight = pxHeight;
				ddcMessage.pxWidth = pxWidth;

				COPYDATASTRUCT desktopCDS;

				desktopCDS.dwData = (unsigned long)DDCIPCMessage::type::RESIZE;
				desktopCDS.cbData = sizeof(DDCIPCMessage);
				desktopCDS.lpData = &ddcMessage;

				SendMessage(g_pDreamHandle, WM_COPYDATA, (WPARAM)(HWND)pWindowHandle, (LPARAM)(LPVOID)&desktopCDS);
			}
		}

		if (pBuffer != nullptr) {
			delete[] pBuffer;
			pBuffer = nullptr;
			//memset(&m_pDataBuffer, 0, m_pDataBuffer_n);
		}

		// Check if for errors
		if (Ret != DUPL_RETURN_SUCCESS) {
			if (Ret == DUPL_RETURN_ERROR_EXPECTED) {
				// Some type of system transition is occurring so retry
				SetEvent(ExpectedErrorEvent);
			}
			else {
				// Unexpected error so exit
				break;
			}
		}
	}

	// Make sure all other threads have exited
	if (SetEvent(TerminateThreadsEvent)) {
		ThreadMgr.WaitForThreadTermination();
	}

	// Clean up
	CloseHandle(UnexpectedErrorEvent);
	CloseHandle(ExpectedErrorEvent);
	CloseHandle(TerminateThreadsEvent);

	if (msg.message == WM_QUIT) {
		// For a WM_QUIT message we should return the wParam value
		return static_cast<INT>(msg.wParam);
	}
Error:
	ProcessFailure(nullptr, L"Error starting Desktop Duplication process", L"Error", E_FAIL);
	return 0;
}

//
// Shows help
//
void ShowHelp() {
	DisplayMsg(L"The following optional parameters can be used -\n  /output [all | n]\t\tto duplicate all outputs or the nth output\n  /?\t\t\tto display this help section",
		L"Proper usage", S_OK);
}

//
// Process command line parameters
//
bool ProcessCmdline(_Out_ INT* outputToDuplicate) {
	*outputToDuplicate = -1;
	// __argv and __argc are global vars set by system
	for (UINT i = 1; i < static_cast<UINT>(__argc); ++i) {
		if ((strcmp(__argv[i], "-output") == 0) ||
			(strcmp(__argv[i], "/output") == 0)) {
			if (++i >= static_cast<UINT>(__argc)) {
				return false;
			}

			if (strcmp(__argv[i], "all") == 0) {
				*outputToDuplicate = -1;
			}
			else {
				*outputToDuplicate = atoi(__argv[i]);
			}
			continue;
		}
		else {
			//return false;
			*outputToDuplicate = 0;	// if no command line args, use main monitor
		}
	}

	*outputToDuplicate = 0;		// Use for testing, will duplicate only main monitor
	return true;
}


BOOL OnCopyData(HWND hWnd, WPARAM wParam, LPARAM lParam) {

	g_pDreamHandle = (HWND)wParam;
	HWND duplicationHandle = hWnd;
	DDCIPCMessage ddcMessage;
	PCOPYDATASTRUCT pDataStruct;

	pDataStruct = (PCOPYDATASTRUCT)lParam;

	switch (pDataStruct->dwData) {	// Handle based on DDCIPCMessage type
	case(0UL): {	// type is PING
		DDCIPCMessage ddcMessage;
		ddcMessage.m_msgType = DDCIPCMessage::type::ACK;
		COPYDATASTRUCT desktopCDS;

		desktopCDS.dwData = (unsigned long)ddcMessage.m_msgType;
		desktopCDS.cbData = sizeof(ddcMessage);
		desktopCDS.lpData = &ddcMessage;

		SendMessage(g_pDreamHandle, WM_COPYDATA, (WPARAM)(HWND)pWindowHandle, (LPARAM)(LPVOID)&desktopCDS);
		return true;
	} break;
	case(2UL): {	// type is START
		g_fStartSending = true;
		//MessageBox(g_pDreamHandle, L"Start sending", L"Status", MB_OK);
		//DisplayMsg(L"Start", L"Start", S_OK);

		return true;
	} break;
	case(3UL): {	// type is STOP
		g_fStartSending = false;
		PostQuitMessage(0);
		return true;
	} break;

	default:
		return false;
	}
}

//
// Window message processor
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DESTROY: {
		PostQuitMessage(0);
	} break;
	case WM_CLOSE: {
		PostQuitMessage(0);
	} break;
	case WM_SIZE: {
		// Tell output manager that window size has changed
		OutMgr.WindowResize();
	} break;
	case WM_COPYDATA: {
		OnCopyData(hWnd, wParam, lParam);
	} break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
// Entry point for new duplication threads
//
DWORD WINAPI DDProc(_In_ void* Param) {
	HRESULT r = S_OK;
	// Classes
	D3D11DesktopDuplicationDisplayManager DispMgr;
	D3D11DesktopDuplicationManager DuplMgr;

	// D3D objects
	ID3D11Texture2D* pSharedSurfaceTexture = nullptr;
	IDXGIKeyedMutex* pKeyMutex = nullptr;

	// Data passed in from thread creation
	THREAD_DATA* pThreadData = reinterpret_cast<THREAD_DATA*>(Param);

	// Get desktop
	DUPL_RETURN Ret = DUPL_RETURN_ERROR_EXPECTED;
	HDESK CurrentDesktop = nullptr;
	CurrentDesktop = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	CBR(CurrentDesktop, R_PASS);	// We do not have access to the desktop so request a retry

	// Attach desktop to this thread
	bool DesktopAttached = SetThreadDesktop(CurrentDesktop) != 0;
	CloseDesktop(CurrentDesktop);
	CurrentDesktop = nullptr;
	CBR(DesktopAttached, R_PASS);	// We do not have access to the desktop so request a retry

	// New display manager
	DispMgr.InitD3D(&pThreadData->DxRes);

	// Obtain handle to sync shared Surface
	CRM(pThreadData->DxRes.Device->OpenSharedResource(pThreadData->TexSharedHandle, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pSharedSurfaceTexture)), "Opening shared texture failed");

	CRM(pSharedSurfaceTexture->QueryInterface(__uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&pKeyMutex)), "Failed to get keyed mutex interface in spawned thread");

	// Make duplication manager
	Ret = DuplMgr.InitDupl(pThreadData->DxRes.Device, pThreadData->Output);
	if (Ret != DUPL_RETURN_SUCCESS) {
		goto Error;
	}

	// Get output description
	DXGI_OUTPUT_DESC DesktopDesc;
	RtlZeroMemory(&DesktopDesc, sizeof(DXGI_OUTPUT_DESC));
	DuplMgr.GetOutputDesc(&DesktopDesc);

	// Main duplication loop
	bool WaitToProcessCurrentFrame = false;
	FRAME_DATA CurrentData;

	while ((WaitForSingleObjectEx(pThreadData->TerminateThreadsEvent, 0, FALSE) == WAIT_TIMEOUT)) {
		if (!WaitToProcessCurrentFrame) {
			// Get new frame from desktop duplication
			bool TimeOut;
			Ret = DuplMgr.GetFrame(&CurrentData, &TimeOut);
			if (Ret != DUPL_RETURN_SUCCESS) {
				// An error occurred getting the next frame drop out of loop which
				// will check if it was expected or not
				break;
			}

			// Check for timeout
			if (TimeOut) {
				// No new frame at the moment
				continue;
			}
		}

		// We have a new frame so try and process it
		// Try to acquire keyed mutex in order to access shared surface
		r = pKeyMutex->AcquireSync(0, 1000);
		if (r == static_cast<HRESULT>(WAIT_TIMEOUT)) {
			// Can't use shared surface right now, try again later
			WaitToProcessCurrentFrame = true;
			continue;
		}
		else {	// check if failed
			CR(r);
		}

		// We can now process the current frame
		WaitToProcessCurrentFrame = false;

		// Get mouse info
		Ret = DuplMgr.GetMouse(pThreadData->PtrInfo, &(CurrentData.FrameInfo), pThreadData->OffsetX, pThreadData->OffsetY);
		if (Ret != DUPL_RETURN_SUCCESS) {
			DuplMgr.DoneWithFrame();
			pKeyMutex->ReleaseSync(1);
			break;
		}

		// Process new frame
		Ret = DispMgr.ProcessFrame(&CurrentData, pSharedSurfaceTexture, pThreadData->OffsetX, pThreadData->OffsetY, &DesktopDesc);
		if (Ret != DUPL_RETURN_SUCCESS) {
			DuplMgr.DoneWithFrame();
			pKeyMutex->ReleaseSync(1);
			break;
		}

		// Release acquired keyed mutex
		CRM(pKeyMutex->ReleaseSync(1), "Unexpected error releasing the keyed mutex");

		// Release frame back to desktop duplication
		Ret = DuplMgr.DoneWithFrame();
		if (Ret != DUPL_RETURN_SUCCESS) {
			break;
		}
	}

Error:
	if (Ret != DUPL_RETURN_SUCCESS) {
		if (Ret == DUPL_RETURN_ERROR_EXPECTED) {
			// The system is in a transition state so request the duplication be restarted
			SetEvent(pThreadData->ExpectedErrorEvent);
		}
		else {
			// Unexpected error so exit the application
			SetEvent(pThreadData->UnexpectedErrorEvent);
		}
	}

	if (RFAILED()) {
		// Generic unknown failure
		Ret = ProcessFailure(pThreadData->DxRes.Device, L"Unexpected error acquiring pKeyMutex", L"Error", r, SystemTransitionsExpectedErrors);
		DuplMgr.DoneWithFrame();
	}

	if (pSharedSurfaceTexture) {
		pSharedSurfaceTexture->Release();
		pSharedSurfaceTexture = nullptr;
	}

	if (pKeyMutex) {
		pKeyMutex->Release();
		pKeyMutex = nullptr;
	}
	return 0;
}

_Post_satisfies_(return != DUPL_RETURN_SUCCESS)
DUPL_RETURN ProcessFailure(_In_opt_ ID3D11Device* pDevice, _In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr, _In_opt_z_ HRESULT* ExpectedErrors) {
	HRESULT TranslatedHr;

	// On an error check if the DX device is lost
	if (pDevice) {
		HRESULT DeviceRemovedReason = pDevice->GetDeviceRemovedReason();

		switch (DeviceRemovedReason) {
		case DXGI_ERROR_DEVICE_REMOVED:
		case DXGI_ERROR_DEVICE_RESET:
		case static_cast<HRESULT>(E_OUTOFMEMORY) : {
			// Our device has been stopped due to an external event on the GPU so map them all to
			// device removed and continue processing the condition
			TranslatedHr = DXGI_ERROR_DEVICE_REMOVED;
		} break;

		case S_OK: {
			// pDevice is not removed so use original error
			TranslatedHr = hr;
		} break;

		default: {
			// pDevice is removed but not a error we want to remap
			TranslatedHr = DeviceRemovedReason;
		}
		}
	}
	else {
		TranslatedHr = hr;
	}

	// Check if this error was expected or not
	if (ExpectedErrors) {
		HRESULT* CurrentResult = ExpectedErrors;

		while (*CurrentResult != S_OK) {
			if (*(CurrentResult++) == TranslatedHr) {
				return DUPL_RETURN_ERROR_EXPECTED;
			}
		}
	}

	// Error was not expected so display the message box
	DisplayMsg(Str, Title, TranslatedHr);

	return DUPL_RETURN_ERROR_UNEXPECTED;
}

//
// Displays a message
//
void DisplayMsg(_In_ LPCWSTR Str, _In_ LPCWSTR Title, HRESULT hr) {
	if (SUCCEEDED(hr)) {
		MessageBoxW(nullptr, Str, Title, MB_OK);
		return;
	}

	const UINT StringLen = (UINT)(wcslen(Str) + sizeof(" with HRESULT 0x########."));
	wchar_t* OutStr = new wchar_t[StringLen];
	if (!OutStr) {
		return;
	}

	INT LenWritten = swprintf_s(OutStr, StringLen, L"%s with 0x%X.", Str, hr);
	if (LenWritten != -1) {
		MessageBoxW(nullptr, OutStr, Title, MB_OK);
	}

	delete[] OutStr;
}
