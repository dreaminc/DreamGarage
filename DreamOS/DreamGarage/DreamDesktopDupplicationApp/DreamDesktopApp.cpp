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

	SetAppName("DreamDesktopApp");
	SetAppDescription("A Shared Desktop View");

	// Set up the quad
	m_pDesktopQuad = GetComposite()->AddQuad(GetWidth(), GetHeight(), 1, 1, nullptr, GetNormal());
	m_pDesktopQuad->SetPosition(0.0f, 2.0f, -2.0f);
	m_pDesktopTexture = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, pxWidth, pxHeight, PIXEL_FORMAT::BGRA, 4, m_pFrameDataBuffer, (int)m_pFrameDataBuffer_n));

	m_pDesktopQuad->SetDiffuseTexture(m_pDesktopTexture.get());

	GetComposite()->SetVisible(true);	

	// Start duplication process
	STARTUPINFO siDesktopDuplication;
	PROCESS_INFORMATION piDesktopDuplication;

	m_hwndDesktopHandle = FindWindow(NULL, L"DreamDesktopDuplication");
	if (m_hwndDesktopHandle == NULL) {
		ZeroMemory(&siDesktopDuplication, sizeof(siDesktopDuplication));
		siDesktopDuplication.cb = sizeof(siDesktopDuplication);
		ZeroMemory(&piDesktopDuplication, sizeof(piDesktopDuplication));

		char location[] = "C:/Users/John/Documents/GitHub/DreamGarage/DreamOS/Project/Windows/DreamOS/x64/Release/DreamDesktopCapture.exe";
		wchar_t wlocation[sizeof(location)];
		mbstowcs(wlocation, location, sizeof(location) + 1);
		LPWSTR strLPWlocation = wlocation;

		if (!CreateProcess(strLPWlocation,
			L" -output 0",				// Command line
			NULL,						// Process handle not inheritable
			NULL,						// Thread handle not inheritable
			FALSE,						// Set handle inheritance to FALSE
			0,							// No creation flags
			NULL,						// Use parent's environment block
			NULL,						// Use parent's starting directory 
			&siDesktopDuplication,      // Pointer to STARTUPINFO structure
			&piDesktopDuplication)      // Pointer to PROCESS_INFORMATION structure
			)
		{
			DEBUG_LINEOUT("CreateProcess failed (%d). \n", GetLastError());
			r = R_FAIL;
		}

		while (m_hwndDesktopHandle == NULL) {
			m_hwndDesktopHandle = FindWindow(NULL, L"DreamDesktopDuplication");
		}
	}

	// TODO: get this from main?
	m_hwndDreamHandle = FindWindow(NULL, L"Dream");
	if (m_hwndDreamHandle == NULL) {
		MessageBox(m_hwndDreamHandle, L"Unable to find the Dream window",
			L"Error", MB_ICONERROR);
		return r;
	}

	return r;
}

RESULT DreamDesktopApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamDesktopApp::Update(void *pContext) {
	RESULT r = R_PASS;

	if (!m_fDesktopDuplicationIsRunning) {
		DDCIPCMessage ddcMessage;
		ddcMessage.SetType(DDCIPCMessage::type::START);
		COPYDATASTRUCT desktopCDS;

		desktopCDS.dwData = (unsigned long)ddcMessage.GetMessage();
		desktopCDS.cbData = sizeof(ddcMessage);
		desktopCDS.lpData = &ddcMessage;

		SendMessage(m_hwndDesktopHandle, WM_COPYDATA, (WPARAM)(HWND)m_hwndDreamHandle, (LPARAM)(LPVOID)&desktopCDS);
		DWORD dwError = GetLastError();
		if (dwError != NO_ERROR) {
			MessageBox(m_hwndDreamHandle, L"error sending message", L"error", MB_ICONERROR);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	
	// Clean up
	//CloseHandle(UnexpectedErrorEvent);
	//CloseHandle(ExpectedErrorEvent);
	//CloseHandle(TerminateThreadsEvent);

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

RESULT DreamDesktopApp::OnDesktopFrame(unsigned long messageSize, void* pMessageData) {
	RESULT r = R_PASS;

	m_pFrameDataBuffer_n = messageSize;
	unsigned char* m_pFrameDataBuffer = (unsigned char*)malloc(m_pFrameDataBuffer_n);
	m_pFrameDataBuffer = (unsigned char*)pMessageData;
	CN(m_pFrameDataBuffer);

	m_pDesktopTexture->Update(m_pFrameDataBuffer, GetWidth(), GetHeight(), PIXEL_FORMAT::BGRA);

Error:
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

