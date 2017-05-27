#include "CefHandler.h"
#include "CefBrowserController.h"

//#include "easylogging++.h"

#include "include\cef_client.h"

#include <sstream>
#include <string>

// cef triggers this warning
#pragma warning(disable : 4067)

#include "include/cef_browser.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#include "CEFApp.h"

#pragma warning(default : 4067)

// Initialize and allocate the instance
CEFHandler* singleton<CEFHandler>::s_pInstance = nullptr;

// This is not the way to do a singleton
// TODO: Do we want this to be a singleton?
CEFHandler::CEFHandler() :
	m_fShuttingdown(false) 
{
	// empty
}

CEFHandler::~CEFHandler() {
	// empty
}

RESULT CEFHandler::RegisterCEFHandlerObserver(CEFHandlerObserver* pCEFHandlerObserver) {
	RESULT r = R_PASS;

	CBM((m_pCEFHandlerObserver == nullptr), "CEFHandlerObserver already registered");
	CN(pCEFHandlerObserver);

	m_pCEFHandlerObserver = pCEFHandlerObserver;

Error:
	return r;
}

// Handler Routing
// Render handler is done at CEFApp to get access to all of the Browser Controllers
CefRefPtr<CefRenderHandler> CEFHandler::GetRenderHandler() { 
	return this;
}

CefRefPtr<CefDisplayHandler> CEFHandler::GetDisplayHandler() {
	return this;
}

CefRefPtr<CefLifeSpanHandler> CEFHandler::GetLifeSpanHandler() {
	return this;
}

CefRefPtr<CefLoadHandler> CEFHandler::GetLoadHandler() {
	return this;
}

/*
CefRefPtr<CefDownloadHandler> CEFHandler::GetDownloadHandler() {
	return this;
}
*/

void PlatformTitleChange(CefRefPtr<CefBrowser> pCEFBrowser, const CefString& strTitle) {
	CefWindowHandle hwnd = pCEFBrowser->GetHost()->GetWindowHandle();
	SetWindowText(hwnd, std::wstring(strTitle).c_str());
}

void CEFHandler::OnTitleChange(CefRefPtr<CefBrowser> pCEFBrowser, const CefString& strTitle) {
	DEBUG_LINEOUT("CEFHANDLE: OnTitleChange %S", std::wstring(strTitle).c_str());

	CEF_REQUIRE_UI_THREAD();

	PlatformTitleChange(pCEFBrowser, strTitle);
}

void CEFHandler::OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFHANDLE: OnAfterCreated");

	CEF_REQUIRE_UI_THREAD();
	
	m_cefBrowsers.push_back(pCEFBrowser);
	m_fBrowserRunning = true;

	std::shared_ptr<CEFBrowserController> pCEFBrowserController = std::make_shared<CEFBrowserController>(pCEFBrowser);
	CN(pCEFBrowserController);

	if (m_pCEFHandlerObserver != nullptr) {
		CR(m_pCEFHandlerObserver->OnBrowserCreated(pCEFBrowserController));
	}

Error:
	return;
}

bool CEFHandler::DoClose(CefRefPtr<CefBrowser> pCEFBrowser) {
	DEBUG_LINEOUT("CEFHANDLE: DoClose");
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed description of this
	// process.
	if (m_cefBrowsers.size() > 1) {
		// Set a flag to indicate that the window close should be allowed.
		m_fShuttingdown = true;
	}

	// Allow the close. For windowed browsers this will result in the OS close
	// event being sent.
	return false;
}

void CEFHandler::OnBeforeClose(CefRefPtr<CefBrowser> pCEFBrowser) {
	DEBUG_LINEOUT("CEFHANDLE: OnBeforeClose");

	CEF_REQUIRE_UI_THREAD();

	///*
	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		if ((*it)->IsSame(pCEFBrowser)) {
			m_cefBrowsers.erase(it);
			break;
		}
	}
	//*/

	// All browser windows have closed. Quit the application message loop.
	if (m_cefBrowsers.empty()) {
		m_fBrowserRunning = false;
		//CefQuitMessageLoop();
	}
}

void CEFHandler::OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, ErrorCode errorCode,
	const CefString& strError, const CefString& strFailedURL) 
{
	DEBUG_LINEOUT("CEFHANDLE: OnLoadError %S url: %S", strError.c_str(), strFailedURL.c_str());

	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files.
	if (errorCode == ERR_ABORTED)
		return;

	// Display a load error message.
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(strFailedURL) <<
		" with error " << std::string(strError) << " (" << errorCode <<
		").</h2></body></html>";

	pCEFFrame->LoadString(ss.str(), strFailedURL);
}

void CEFHandler::CloseAllBrowsers(bool fForceClose) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("CEFHANDLE: CloseAllBrowsers");

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CEFHandler::CloseAllBrowsers, this, fForceClose));
		return;
	}

	if (m_cefBrowsers.empty()) {
		return;
	}

	// Make a copy - threading issue
	std::list<CefRefPtr<CefBrowser>> tempCEFBrowsers = std::list<CefRefPtr<CefBrowser>>(m_cefBrowsers);

	for (auto &pCEFBrowser : tempCEFBrowsers) {
		if (pCEFBrowser != nullptr) {
			pCEFBrowser->GetHost()->CloseBrowser(fForceClose);
		}
	}

//Error:
	return;
}

bool CEFHandler::GetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: GetViewRect");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnGetViewRect(pCEFBrowser, cefRect));

Error:
	//return (r >= 0);
	return true;
}

void CEFHandler::OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, PaintElementType type, const RectList &dirtyRects, const void *pBuffer, int width, int height) {
	RESULT r = R_PASS;
	DEBUG_LINEOUT("CEFHANDLE: OnPaint");

	CN(m_pCEFHandlerObserver);
	CR(m_pCEFHandlerObserver->OnPaint(pCEFBrowser, type, dirtyRects, pBuffer, width, height));

Error:
	return;
}

