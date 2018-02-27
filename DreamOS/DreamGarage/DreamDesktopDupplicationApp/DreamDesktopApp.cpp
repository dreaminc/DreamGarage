#include "DreamDesktopApp.h"
#include "DreamOS.h"
#include "Core/Utilities.h"

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

RESULT DreamDesktopApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	int pxWidth = m_pxDesktopWidth;
	int pxHeight = m_pxDesktopHeight;
	m_aspectRatio = ((float)pxWidth / (float)pxHeight);

	std::vector<unsigned char> vectorByteBuffer(pxWidth * pxHeight * 4, 0xFF);

	SetAppName("DreamDesktopApp");
	SetAppDescription("A Shared Desktop View");

	// TODO: quad is in shareview/control area, just need texture
	// Set up the quad
	//m_pDesktopQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pDesktopQuad = GetComposite()->AddQuad(.938f * 4.0, .484f * 4.0, 1, 1, nullptr, vector::kVector());	// these are all temp, just until merge with controlview/shareview
	m_pDesktopQuad->SetPosition(0.0f, 0.0f, 0.0f);
	m_pDesktopQuad->FlipUVVertical();

	// Initialize texture
	m_pDesktopTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, &vectorByteBuffer[0], pxWidth * pxHeight * 4));
	m_pDesktopQuad->SetDiffuseTexture(m_pDesktopTexture.get());

	GetComposite()->SetVisible(true);	

	CRM(StartDuplicationProcess(), "Error starting duplication process");

	// TODO: get this from main?
	m_hwndDreamHandle = FindWindow(NULL, L"Dream Testing");
	CNM(m_hwndDreamHandle, "Unable to find the Dream window");

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

	// TODO: macro with project pre-definition using project name
	PathManager* pPathManager = PathManager::instance();
	std::wstring wstrDreamPath;
	pPathManager->GetDreamPath(wstrDreamPath);

	std::wstring wstrPathfromDreamPath = L"\\Project\\Windows\\DreamOS\\x64\\Release\\DreamDesktopCapture.exe";
	std::wstring wstrFullpath = wstrDreamPath + wstrPathfromDreamPath;
	const wchar_t *wPath = wstrFullpath.c_str();
	std::vector<wchar_t> vwszLocation(wstrFullpath.begin(), wstrFullpath.end());
	vwszLocation.push_back(0);
	LPWSTR lpwstrLocation = vwszLocation.data();
	bool fCreateDuplicationProcess = false;

	CBR(m_hwndDesktopHandle == nullptr, R_SKIPPED);		// Desktop duplication shouldn't be running, but if it is, and we have a handle, don't start another.

	fCreateDuplicationProcess = CreateProcess(lpwstrLocation,
		L" -output 0",						// Command line
		nullptr,							// Process handle not inheritable
		nullptr,							// Thread handle not inheritable
		false,								// Set handle inheritance to FALSE
		0,									// No creation flags
		nullptr,							// Use parent's environment block
		nullptr,							// Use parent's starting directory 
		&startupinfoDesktopDuplication,     // Pointer to STARTUPINFO structure
		&processinfoDesktopDuplication		// Pointer to PROCESS_INFORMATION structure
	);

	CBM(fCreateDuplicationProcess, "CreateProcess failed (%d)", GetLastError());

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
		CR(SendStartDesktopDuplicationIPCMessage());
	}

Error:
	return r;
}

RESULT DreamDesktopApp::SendStartDesktopDuplicationIPCMessage() {
	RESULT r = R_PASS;

	DDCIPCMessage ddcMessage;
	ddcMessage.m_msgType = DDCIPCMessage::type::START;
	COPYDATASTRUCT desktopCDS;

	desktopCDS.dwData = (unsigned long)ddcMessage.m_msgType;
	desktopCDS.cbData = sizeof(ddcMessage);
	desktopCDS.lpData = &ddcMessage;

	SendMessage(m_hwndDesktopHandle, WM_COPYDATA, (WPARAM)(HWND)m_hwndDreamHandle, (LPARAM)(LPVOID)&desktopCDS);
	DWORD dwError = GetLastError();

	CBR(dwError == ERROR_SUCCESS, R_SKIPPED);
	m_fDesktopDuplicationIsRunning = true;

Error:
	return r;
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
	m_frameDataBuffer_n = messageSize;
	//m_pFrameDataBuffer = (unsigned char*)malloc(m_frameDataBuffer_n);
	//m_pFrameDataBuffer = (unsigned char*)pMessageData;
	//memcpy(m_pFrameDataBuffer, (unsigned char*)pMessageData, m_frameDataBuffer_n);

	//CNR(m_pFrameDataBuffer, R_SKIPPED);
	CNR(pMessageData, R_SKIPPED);
	if (m_pxDesktopHeight != pxHeight || m_pxDesktopWidth != pxWidth) {
		m_pxDesktopWidth = pxWidth;
		m_pxDesktopHeight = pxHeight;
		m_pDesktopTexture->UpdateDimensions(pxWidth, pxHeight);
	}

	m_pDesktopTexture->Update((unsigned char*)pMessageData, pxWidth, pxHeight, PIXEL_FORMAT::BGRA);

Error:
	if (m_pFrameDataBuffer != nullptr) {
		free(m_pFrameDataBuffer);
		m_pFrameDataBuffer = nullptr;
	}

	return r;
}

float DreamDesktopApp::GetHeight() {
	return std::sqrt((m_diagonalSize * m_diagonalSize) / (1.0f + (m_aspectRatio * m_aspectRatio)));
}

float DreamDesktopApp::GetWidth() {
	return std::sqrt(((m_aspectRatio * m_aspectRatio) * (m_diagonalSize * m_diagonalSize)) / (1.0f + (m_aspectRatio * m_aspectRatio)));
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

bool DreamDesktopApp::IsVisible() {
	return m_pDesktopQuad->IsVisible();
}

RESULT DreamDesktopApp::SetVisible(bool fVisible) {
	RESULT r = R_PASS;

	CR(m_pDesktopQuad->SetVisible(fVisible));

Error:
	return r;
}

