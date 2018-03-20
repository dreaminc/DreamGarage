#include "DreamDesktopApp.h"
#include "DreamOS.h"
#include "Core/Utilities.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "Cloud/Environment/EnvironmentAsset.h"

#include "DDCIPCMessage.h"
#include <windows.h>
#include <windowsx.h>


DreamDesktopApp::DreamDesktopApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamDesktopApp>(pDreamOS, pContext)
{
	// Empty - initialization by factory
}

DreamDesktopApp::~DreamDesktopApp() {
	RESULT r = R_PASS;

	CR(Shutdown());

Error:
	return;
}

RESULT DreamDesktopApp::OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) {
	RESULT r = R_PASS;

	INPUT inputStruct;
	inputStruct.type = INPUT_MOUSE;

	MOUSEINPUT mouseInputStruct;
	CBR(m_fDesktopDuplicationIsRunning, R_SKIPPED);
	CNR(m_hwndDreamHandle, R_SKIPPED);

	mouseInputStruct.dx = scrollPoint.x() * (_UI16_MAX / m_pxDesktopWidth);		// Windows desktop is mapped to a 65535 x 65535 
	mouseInputStruct.dy = scrollPoint.y() * (_UI16_MAX / m_pxDesktopHeight);

	mouseInputStruct.dwFlags = MOUSEEVENTF_WHEEL;
	// TODO: consistent scroll speed with browser scroll
	mouseInputStruct.mouseData = 120 * pxYDiff;

	inputStruct.mi = mouseInputStruct;
	SendInput(1, &inputStruct, sizeof(INPUT));	// this function is subject to User Interface Privilege Isolation (UIPI)- application is only permitted to inject input to applications that are running at an equal or lesser integrity level

Error:
	return r;
}

RESULT DreamDesktopApp::OnKeyPress(char chKey, bool fkeyDown) {
	RESULT r = R_PASS;

	INPUT inputStruct;
	inputStruct.type = INPUT_KEYBOARD;

	KEYBDINPUT keyboardInputStruct;
	CBR(m_fDesktopDuplicationIsRunning, R_SKIPPED);
	CNR(m_hwndDreamHandle, R_SKIPPED);

	// Set up generic keyboard event
	keyboardInputStruct.wScan = 0;
	keyboardInputStruct.dwExtraInfo = 0;
	keyboardInputStruct.wVk = chKey;		// Should be getting VK code from Sensekeyboard anyway
	keyboardInputStruct.dwFlags = 0;		// 0 for key press

	inputStruct.ki = keyboardInputStruct;
	SendInput(1, &inputStruct, sizeof(INPUT));	// this function is subject to User Interface Privilege Isolation (UIPI)- application is only permitted to inject input to applications that are running at an equal or lesser integrity level

	keyboardInputStruct.dwFlags = KEYEVENTF_KEYUP;	// key up for key release
	SendInput(1, &inputStruct, sizeof(INPUT));

Error:
	return r;
}

RESULT DreamDesktopApp::OnMouseMove(point mousePoint) {
	RESULT r = R_PASS;

	INPUT inputStruct;
	inputStruct.type = INPUT_MOUSE;

	MOUSEINPUT mouseInputStruct;
	CBR(m_fDesktopDuplicationIsRunning, R_SKIPPED);
	CNR(m_hwndDreamHandle, R_SKIPPED);

	mouseInputStruct.dx = mousePoint.x() * (_UI16_MAX / m_pxDesktopWidth);		// Windows desktop is mapped to a 65535 x 65535 
	mouseInputStruct.dy = mousePoint.y() * (_UI16_MAX / m_pxDesktopHeight);

	mouseInputStruct.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

	inputStruct.mi = mouseInputStruct;
	SendInput(1, &inputStruct, sizeof(INPUT));	// this function is subject to User Interface Privilege Isolation (UIPI)- application is only permitted to inject input to applications that are running at an equal or lesser integrity level

Error:
	return r;
}

RESULT DreamDesktopApp::OnClick(point ptDiff, bool fMouseDown) {
	RESULT r = R_PASS;

	INPUT inputStruct;
	inputStruct.type = INPUT_MOUSE;

	MOUSEINPUT mouseInputStruct;
	CBR(m_fDesktopDuplicationIsRunning, R_SKIPPED);
	CNR(m_hwndDreamHandle, R_SKIPPED);

	mouseInputStruct.dx = ptDiff.x() * (_UI16_MAX / m_pxDesktopWidth);		// Windows desktop is mapped to a 65535 x 65535 
	mouseInputStruct.dy = ptDiff.y() * (_UI16_MAX / m_pxDesktopHeight);

	if (fMouseDown) {
		mouseInputStruct.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_MOVE;
	}
	else {
		mouseInputStruct.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP | MOUSEEVENTF_MOVE;
	}
	
	inputStruct.mi = mouseInputStruct;
	SendInput(1, &inputStruct, sizeof(INPUT));	// this function is subject to User Interface Privilege Isolation (UIPI)- application is only permitted to inject input to applications that are running at an equal or lesser integrity level
												// number of structures in pInputs array
												// array of INPUT structures representing input event
												// size in BYTES of an INPUT structure
Error:
	return r;
}

RESULT DreamDesktopApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	int pxWidth = m_pxDesktopWidth;
	int pxHeight = m_pxDesktopHeight;
	m_aspectRatio = ((float)pxWidth / (float)pxHeight);

	std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

	SetAppName("DreamDesktopApp");
	SetAppDescription("A Shared Desktop View");

	// Initialize texture
	m_pDesktopTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4));

	GetComposite()->SetVisible(true);

	CRM(StartDuplicationProcess(), "Error starting duplication process");
	m_hwndDreamHandle = GetDOS()->GetDreamHWND();
	
	CNR(m_hwndDreamHandle, R_SKIPPED);	

Error:
	return r;
}

RESULT DreamDesktopApp::StartDuplicationProcess() {
	RESULT r = R_PASS;

	// Start duplication process
	STARTUPINFO startupinfoDesktopDuplication;
	PROCESS_INFORMATION processinfoDesktopDuplication;

	memset(&startupinfoDesktopDuplication, 0, sizeof(startupinfoDesktopDuplication));
	startupinfoDesktopDuplication.cb = sizeof(startupinfoDesktopDuplication);
	memset(&processinfoDesktopDuplication, 0, sizeof(processinfoDesktopDuplication));

	wchar_t pszFilePath[MAX_PATH];
	GetModuleFileName(NULL, pszFilePath, sizeof(pszFilePath));	// Gets path to .exe, including exe name
	std::wstring wstrFilePath = std::wstring(pszFilePath);

	size_t nIndex = wstrFilePath.find_last_of(L"\\");			// Finds index of .exe name, since path will look like "..\\DreamExample.exe"
	wstrFilePath.erase(wstrFilePath.begin() + nIndex, wstrFilePath.end());	// erases .exe name from the path

	std::wstring wstrFullpath = wstrFilePath + WSTRDREAMCAPTURELOCATION;	// adds DreamDesktopCapture.exe to the path

	const wchar_t *wPath = wstrFullpath.c_str();	// convert wstring to LPWSTR
	std::vector<wchar_t> vwszLocation(wstrFullpath.begin(), wstrFullpath.end());
	vwszLocation.push_back(0);
	LPWSTR lpwstrLocation = vwszLocation.data();

	bool fCreatedDuplicationProcess = false;
	
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobELI = { 0 };	// In case we want to add memory limits, and can track peak usage
	jobELI.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

	m_dreamJobHandle = CreateJobObject(nullptr, L"DreamJob");
	CNM(m_dreamJobHandle, "Failed to create job object");

	SetInformationJobObject(m_dreamJobHandle, JobObjectExtendedLimitInformation, &jobELI, sizeof(jobELI));

	CBR(m_hwndDesktopHandle == nullptr, R_SKIPPED);		// Desktop duplication shouldn't be running, but if it is, and we have a handle, don't start another.

	LPTSTR szCmdline = L" -output 0";

	fCreatedDuplicationProcess = CreateProcess(lpwstrLocation,
		szCmdline,						// Command line
		nullptr,							// Process handle not inheritable
		nullptr,							// Thread handle not inheritable
		false,								// Set handle inheritance to FALSE
		0,									// No creation flags
		nullptr,							// Use parent's environment block
		nullptr,							// Use parent's starting directory
		&startupinfoDesktopDuplication,     // Pointer to STARTUPINFO structure
		&processinfoDesktopDuplication		// Pointer to PROCESS_INFORMATION structure
	);

	CBM(fCreatedDuplicationProcess, "CreateProcess failed (%d)", GetLastError());

	AssignProcessToJobObject(m_dreamJobHandle, processinfoDesktopDuplication.hProcess);

Error:
	return r;
}

RESULT DreamDesktopApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamDesktopApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (m_hwndDesktopHandle == NULL) {	// duplication process may take a bit to load, so catch it when it's done
		m_hwndDesktopHandle = FindWindow(NULL, L"DreamDesktopDuplication");
	}
	CNR(m_hwndDesktopHandle, R_SKIPPED);	// duplication process isn't ready yet, so skip

	std::chrono::steady_clock::duration tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	float msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	if (msTimeNow - m_msTimeSinceLastSent > m_msTimeDelay && !m_fDesktopDuplicationIsRunning) {
		m_msTimeSinceLastSent = msTimeNow;
		CR(SendDesktopDuplicationIPCMessage(DDCIPCMessage::type::START));
	}

Error:
	return r;
}

RESULT DreamDesktopApp::SendDesktopDuplicationIPCMessage(DDCIPCMessage::type msgType) {
	RESULT r = R_PASS;

	DDCIPCMessage ddcMessage;
	COPYDATASTRUCT desktopCDS;
	CNR(m_hwndDesktopHandle, R_SKIPPED);

	ddcMessage.m_msgType = msgType;
	desktopCDS.dwData = (unsigned long)ddcMessage.m_msgType;
	desktopCDS.cbData = sizeof(ddcMessage);
	desktopCDS.lpData = &ddcMessage;
	
	if (msgType == DDCIPCMessage::type::STOP) {
		//SendMessageCallback(m_hwndDesktopHandle, WM_COPYDATA, (WPARAM)(HWND)m_hwndDreamHandle, (LPARAM)(LPVOID)&desktopCDS, nullptr, 0);
		//SendMessage(m_hwndDesktopHandle, WM_COPYDATA, (WPARAM)(HWND)m_hwndDreamHandle, (LPARAM)(LPVOID)&desktopCDS);
		PostMessage(m_hwndDesktopHandle, WM_CLOSE, 0, 0);
	}
	else {
		SendMessage(m_hwndDesktopHandle, WM_COPYDATA, (WPARAM)(HWND)m_hwndDreamHandle, (LPARAM)(LPVOID)&desktopCDS);
	}
	
	DWORD dwError = GetLastError();

	CBR(dwError == ERROR_SUCCESS, R_SKIPPED);

Error:
	return r;
}

RESULT DreamDesktopApp::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	m_assetID = pEnvironmentAsset->GetAssetID();
	return R_PASS;
}

RESULT DreamDesktopApp::Shutdown(void *pContext) {
	// TODO: clean up in here	

	return R_PASS;
}

DreamDesktopApp* DreamDesktopApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamDesktopApp *pDreamApp = new DreamDesktopApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamDesktopApp::SetPosition(point ptPosition) {
	RESULT r = R_PASS;

	GetComposite()->SetPosition(ptPosition);

	return r;
}

RESULT DreamDesktopApp::SetAspectRatio(float aspectRatio) {
	m_aspectRatio = aspectRatio;

	if (m_pDesktopQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamDesktopApp::SetDiagonalSize(float diagonalSize) {
	m_diagonalSize = diagonalSize;

	if (m_pDesktopQuad != nullptr)
		return UpdateViewQuad();

	return R_PASS;
}

RESULT DreamDesktopApp::SetNormalVector(vector vNormal) {
	m_vNormal = vNormal.Normal();

	if (m_pDesktopQuad != nullptr) {
		return UpdateViewQuad();
	}

	return R_PASS;
}

RESULT DreamDesktopApp::SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal) {
	GetComposite()->SetPosition(ptPosition);
	m_diagonalSize = diagonal;
	m_aspectRatio = aspectRatio;
	m_vNormal = vNormal.Normal();

	if (m_pDesktopQuad != nullptr) {
		return UpdateViewQuad();
	}

	return R_PASS;
}

RESULT DreamDesktopApp::OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;
	m_fDesktopDuplicationIsRunning = true;
	CNR(pMessageData, R_SKIPPED);
	m_frameDataBuffer_n = messageSize;

	if (m_pxDesktopHeight != pxHeight || m_pxDesktopWidth != pxWidth) {
		m_pxDesktopWidth = pxWidth;
		m_pxDesktopHeight = pxHeight;
		CRM(m_pDesktopTexture->UpdateDimensions(pxWidth, pxHeight), "Failed updating desktop texture dimensions");
		m_pParentApp->UpdateTextureForDesktop(m_pDesktopTexture, this);
	}

	m_pDesktopTexture->Update((unsigned char*)pMessageData, pxWidth, pxHeight, PIXEL_FORMAT::BGRA);

	CBR(GetSourceTexture().get() == GetDOS()->GetSharedContentTexture().get(), R_SKIPPED);
	GetDOS()->BroadcastSharedVideoFrame((unsigned char*)(pMessageData), pxWidth, pxHeight);

Error:
	/*
	if (m_pFrameDataBuffer != nullptr) {
		free(m_pFrameDataBuffer);
		m_pFrameDataBuffer = nullptr;
	}
	//*/

	return r;
}

RESULT DreamDesktopApp::InitializeWithParent(DreamUserControlArea *pParentApp) {
	m_pParentApp = pParentApp;
	return R_PASS;
}

float DreamDesktopApp::GetHeightFromAspectDiagonal() {
	return std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamDesktopApp::GetWidthFromAspectDiagonal() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

int DreamDesktopApp::GetHeight() {
	return m_pxDesktopHeight;
}

int DreamDesktopApp::GetWidth() {
	return m_pxDesktopWidth;
}

std::string DreamDesktopApp::GetTitle() {
	//TODO: temporary until app focus is known
	return "Windows Desktop";
}

vector DreamDesktopApp::GetNormal() {
	return m_vNormal;
}

point DreamDesktopApp::GetOrigin() {
	return GetComposite()->GetOrigin();
}

RESULT DreamDesktopApp::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pDesktopQuad->UpdateParams(GetWidth(), GetHeight(), GetNormal()));

	// Flip UV vertically
	///*
	if (r != R_SKIPPED) {
		m_pDesktopQuad->FlipUVVertical();
	}
	//*/

	CR(m_pDesktopQuad->SetDirty());

	CR(m_pDesktopQuad->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), GetNormal()));

Error:
	return r;
}

std::shared_ptr<texture> DreamDesktopApp::GetSourceTexture() {
	return m_pDesktopTexture;
}

RESULT DreamDesktopApp::SetScope(std::string strScope) {
	m_strScope = strScope;
	return R_PASS;
}

RESULT DreamDesktopApp::SetPath(std::string strPath) {
	m_strPath = strPath;
	return R_PASS;
}

long DreamDesktopApp::GetCurrentAssetID() {
	return m_assetID;
}

RESULT DreamDesktopApp::CloseSource() {
	RESULT r = R_PASS;

	CR(SendDesktopDuplicationIPCMessage(DDCIPCMessage::type::STOP));

Error:
	return r;
}

bool DreamDesktopApp::IsVisible() {
	return m_pDesktopQuad->IsVisible();
}

RESULT DreamDesktopApp::SetVisible(bool fVisible) {
	RESULT r = R_PASS;

	CR(m_pDesktopQuad->SetVisible(fVisible));

Error:
	return r;
}
