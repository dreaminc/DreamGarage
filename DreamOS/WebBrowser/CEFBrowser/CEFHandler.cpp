#include "CefHandler.h"
#include "CefBrowserController.h"

#include "easylogging++.h"

#include <sstream>
#include <string>

// cef triggers this warning
#pragma warning(disable : 4067)

#include "include/cef_browser.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"

#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

#pragma warning(default : 4067)

// Initialize and allocate the instance
CEFHandler* CEFHandler::s_pInstance = nullptr;

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
	DEBUG_LINEOUT("CEFHANDLE: OnAfterCreated");

	CEF_REQUIRE_UI_THREAD();
	
	m_cefBrowsers.push_back(pCEFBrowser);
}

bool CEFHandler::DoClose(CefRefPtr<CefBrowser> pCEFBrowser) {
	DEBUG_LINEOUT("CEFHANDLE: DoClose");
	CEF_REQUIRE_UI_THREAD();

	// Closing the main window requires special handling. See the DoClose()
	// documentation in the CEF header for a detailed destription of this
	// process.
	if (m_cefBrowsers.size() == 1) {
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

	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		if ((*it)->IsSame(pCEFBrowser)) {
			m_cefBrowsers.erase(it);
			break;
		}
	}

	// All browser windows have closed. Quit the application message loop.
	if (m_cefBrowsers.empty()) {
		CefQuitMessageLoop();
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

	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		(*it)->GetHost()->CloseBrowser(fForceClose);
	}

//Error:
	return;
}

WebBrowserController* CEFHandler::CreateBrowser(unsigned int width, unsigned int height, const std::string& strURL) {
	DEBUG_LINEOUT("CEFHANDLE: CreateBrowser");

	CefWindowInfo cefWindowInfo;
	CefBrowserSettings cefBrowserSettings;

	//cefWindowInfo.SetAsWindowless(0, false);
	cefWindowInfo.SetAsPopup(nullptr, "cefsimple");
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;

	// clear the promise for reuse
	/*
	m_NewWebBrowserControllerPromise = std::promise<CEFBrowserController*>();
	auto newBrowser = m_NewWebBrowserControllerPromise.get_future();
	*/
	
	if (CefBrowserHost::CreateBrowser(cefWindowInfo, this, strURL, cefBrowserSettings, nullptr) == false) {
		DEBUG_LINEOUT("CreateBrowser failed");
		return nullptr;
	}

	// Blocks until promise is settled
	/*
	WebBrowserController* pBrowserController = newBrowser.get();
	pBrowserController->Resize(width, height);
	return pBrowserController;
	*/

	return nullptr;
}

/*

bool CEFHandler::GetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) {
	DEBUG_LINEOUT("CEFHANDLE: GetViewRect");

	return DelegateToController(pCEFBrowser, [&](CEFBrowserController* pCEFBrowserController) {
		pCEFBrowserController->GetViewRect(pCEFBrowser, cefRect);
	});
}

void CEFHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	DEBUG_LINEOUT("CEFHANDLE: OnPaint");

	DelegateToController(browser, [&](CEFBrowserController* controller) {
		controller->OnPaint(browser, type, dirtyRects, buffer, width, height);
	});
}
*/

