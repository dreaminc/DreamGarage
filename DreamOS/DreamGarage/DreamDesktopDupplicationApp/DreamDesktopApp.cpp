#include "DreamDesktopApp.h"
#include "DreamOS.h"
#include "Core/Utilities.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "Cloud/Environment/EnvironmentAsset.h"

#include "DDCIPCMessage.h"
#include <windows.h>
#include <windowsx.h>
#include "HAL\opengl\OGLTexture.h"


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
	mouseInputStruct.mouseData = pxYDiff;

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
	//*
	keyboardInputStruct.wScan = chKey;
	keyboardInputStruct.dwExtraInfo = 0;
	keyboardInputStruct.wVk = 0;		// Should be getting VK code from Sensekeyboard anyway
	keyboardInputStruct.dwFlags = KEYEVENTF_UNICODE;		// 0 for key press
	//*/

	if (chKey == SVK_RETURN || chKey == SVK_BACK) {
		keyboardInputStruct.wScan = 0;
		keyboardInputStruct.dwExtraInfo = 0;
		keyboardInputStruct.wVk = chKey;
		keyboardInputStruct.dwFlags = 0;
	}

	inputStruct.ki = keyboardInputStruct;

	SendInput(1, &inputStruct, sizeof(INPUT));	// this function is subject to User Interface Privilege Isolation (UIPI)- application is only permitted to inject input to applications that are running at an equal or lesser integrity level

	keyboardInputStruct.dwFlags |= KEYEVENTF_KEYUP;	// key up for key release
//	SendInput(1, &inputStruct, sizeof(INPUT));

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

	GetDOS()->RegisterWindows64Observer(this);

	// Initialize texture
	m_pDesktopTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4);
	m_pLoadingScreenTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszLoadingScreen));

	//*	// Testing quad
	auto pComposite = GetDOS()->AddComposite();
	pComposite->InitializeOBB();

	auto pView = pComposite->AddUIView(GetDOS());
	pView->InitializeOBB();

	m_pDesktopQuad = pView->AddQuad(.938f * 4.0, .484f * 4.0, 1, 1, nullptr, vector::kVector());
	m_pDesktopQuad->SetPosition(0.0f, 0.0f, 0.0f);
	m_pDesktopQuad->FlipUVVertical();

	m_pDesktopQuad->SetDiffuseTexture(m_pDesktopTexture.get());
	//*/

	CR(dynamic_cast<OGLTexture*>(m_pDesktopTexture.get())->EnableOGLPBOUnpack());

	CR(GetComposite()->SetVisible(true));

Error:
	return r;
}

RESULT DreamDesktopApp::StartDuplicationProcess() {
	RESULT r = R_PASS;

	// Start duplication process
	STARTUPINFOW startupinfoDesktopDuplication;
	PROCESS_INFORMATION processinfoDesktopDuplication;

	memset(&startupinfoDesktopDuplication, 0, sizeof(startupinfoDesktopDuplication));
	startupinfoDesktopDuplication.cb = sizeof(startupinfoDesktopDuplication);
	memset(&processinfoDesktopDuplication, 0, sizeof(processinfoDesktopDuplication));

	wchar_t pszFilePath[MAX_PATH];
	GetModuleFileNameW(NULL, pszFilePath, sizeof(pszFilePath));	// Gets path to .exe, including exe name
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
	
	JOBOBJECT_CPU_RATE_CONTROL_INFORMATION jobCRCI = { 0 };
	jobCRCI.ControlFlags = JOB_OBJECT_CPU_RATE_CONTROL_ENABLE | JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;
	jobCRCI.CpuRate = 1500;	// percent used * 100;

	m_dreamJobHandle = CreateJobObjectW(nullptr, L"DreamDesktopJob");
	CNM(m_dreamJobHandle, "Failed to create job object");

	SetInformationJobObject(m_dreamJobHandle, JobObjectExtendedLimitInformation, &jobELI, sizeof(jobELI));
	SetInformationJobObject(m_dreamJobHandle, JobObjectCpuRateControlInformation, &jobCRCI, sizeof(jobCRCI));

	CBR(m_hwndDesktopHandle == nullptr, R_SKIPPED);		// Desktop duplication shouldn't be running, but if it is, and we have a handle, don't start another.

	fCreatedDuplicationProcess = CreateProcessW(
		lpwstrLocation,
		(LPWSTR)(L" -output 0"),									// Command line
		nullptr,													// Process handle not inheritable
		nullptr,													// Thread handle not inheritable
		false,														// Set handle inheritance to FALSE
		IDLE_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP,				// Creation flags
		nullptr,													// Use parent's environment block
		nullptr,													// Use parent's starting directory
		(LPSTARTUPINFOW)(&startupinfoDesktopDuplication),			// Pointer to STARTUPINFO structure
		(LPPROCESS_INFORMATION)(&processinfoDesktopDuplication)		// Pointer to PROCESS_INFORMATION structure
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

	m_hwndDreamHandle = GetDOS()->GetDreamHWND();

	CNR(m_hwndDreamHandle, R_SKIPPED);

	if (m_hwndDesktopHandle == nullptr) {	// duplication process may take a bit to load, so catch it when it's done
		m_hwndDesktopHandle = FindWindowW(NULL, L"DreamDesktopDuplication");
	}
	CNR(m_hwndDesktopHandle, R_SKIPPED);	// duplication process isn't ready yet, so skip

	std::chrono::steady_clock::duration tNow;
	tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	
	float msTimeNow;
	msTimeNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	
	if (msTimeNow - m_msTimeSinceLastSent > m_msMessageTimeDelay && !m_fDesktopDuplicationIsRunning) {
		m_msTimeSinceLastSent = msTimeNow;
		CR(SendDesktopDuplicationIPCMessage(DDCIPCMessage::type::START));
	}

	// Once duplication process is running, we want to grab the title of the foreground window for title
	// This method solves for when the user alt-tabs using their actual keyboard, since we don't have that event
	if (m_fDesktopDuplicationIsRunning && msTimeNow - m_msTimeSinceLastSent > m_msTitleTimeDelay) {
		m_msTimeSinceLastSent = msTimeNow;
		std::string strTitle = GetTitle();
		if (m_strTitle != strTitle) {
			m_strTitle = strTitle;
			m_pParentApp->UpdateControlBarText(m_strTitle);
		}
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
	
	DWORD dwError;
	dwError = GetLastError();

	CBR(dwError == ERROR_SUCCESS, R_SKIPPED);

Error:
	return r;
}

RESULT DreamDesktopApp::SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	m_assetID = pEnvironmentAsset->GetAssetID();
	m_strContentType = pEnvironmentAsset->GetContentType();
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

RESULT DreamDesktopApp::HandleWindows64CopyData(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;

	CNR(pMessageData, R_SKIPPED);
	
	/*
	if (!m_fDesktopDuplicationIsRunning) {
		m_pxDesktopWidth = pxWidth;
		m_pxDesktopHeight = pxHeight;
		CRM(m_pDesktopTexture->UpdateDimensions(pxWidth, pxHeight), "Failed updating desktop texture dimensions");
		std::shared_ptr<DreamContentSource> pContentSource = std::dynamic_pointer_cast<DreamContentSource>(GetDOS()->GetDreamAppFromUID(GetAppUID()));
		CNM(pContentSource, "Failed getting Desktop as a content source");
		m_pParentApp->UpdateContentSourceTexture(m_pDesktopTexture.get(), pContentSource);
		m_fDesktopDuplicationIsRunning = true;
	}
	//*/

	if (dynamic_cast<OGLTexture*>(m_pDesktopTexture.get())->IsOGLPBOUnpackEnabled()) {
		m_pDesktopTexture->UpdateTextureFromBuffer((unsigned char*)pMessageData, messageSize);
	}
	else {
		CR(m_pDesktopTexture->Update((unsigned char*)(pMessageData), pxWidth, pxHeight, PIXEL_FORMAT::BGRA));
	}
	
	CNR(GetDOS()->GetSharedContentTexture(), R_SKIPPED);
	CBR(GetSourceTexture() == GetDOS()->GetSharedContentTexture(), R_SKIPPED);
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
	RESULT r = R_PASS;
	
	m_pParentApp = pParentApp;
	std::shared_ptr<DreamContentSource> pContentSource = std::dynamic_pointer_cast<DreamContentSource>(GetDOS()->GetDreamAppFromUID(GetAppUID()));
	CNM(pContentSource, "Failed getting Desktop as a content source");
	m_pParentApp->UpdateContentSourceTexture(m_pLoadingScreenTexture.get(), pContentSource);
	//CRM(StartDuplicationProcess(), "Error starting duplication process");
	
Error:
	return r;
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
	HWND hwndForegroundWindow = GetForegroundWindow();
	wchar_t pszTitle[2048];

	GetWindowTextW(hwndForegroundWindow, pszTitle, sizeof(pszTitle));
	
	std::wstring wstrTitle = std::wstring(pszTitle);
	std::string strTitle = util::WideStringToString(wstrTitle);
	
	// Can only be one parent (child cannot also be an owner) - https://msdn.microsoft.com/en-us/library/windows/desktop/ms632599%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396#foreground
	if (strTitle == "") {	// If foreground is blank, it may be a popup window, so check for parent
		HWND hwndParentWindow = GetWindow(hwndForegroundWindow, GW_OWNER);
		if (hwndParentWindow == nullptr) { // If parent is null then it's desktop... probably
			strTitle = "Windows Desktop";
		}
		else {
			GetWindowTextW(hwndParentWindow, pszTitle, sizeof(pszTitle));
			wstrTitle = std::wstring(pszTitle);
			strTitle = util::WideStringToString(wstrTitle);
		}
	}

	return strTitle;
}

std::string DreamDesktopApp::GetContentType() {
	return m_strContentType;
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

texture* DreamDesktopApp::GetSourceTexture() {
	return m_pDesktopTexture.get();
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

RESULT DreamDesktopApp::SendFirstFrame() {
	return R_NOT_IMPLEMENTED;
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
