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

namespace {
	CEFHandler * g_instance = NULL;
}

// This is not the way to do a singleton
// TODO: Do we want this to be a singleton?
CEFHandler::CEFHandler() :
	m_fShuttingdown(false) 
{
	DCHECK(!g_instance);
	g_instance = this;
}

CEFHandler ::~CEFHandler() {
	g_instance = nullptr;
}

CEFHandler * CEFHandler::GetInstance() {
	return g_instance;
}

void CEFHandler::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	//
}

void CEFHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
	CEF_REQUIRE_UI_THREAD();

	//
}

void CEFHandler::OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) {
	CEF_REQUIRE_UI_THREAD();
	
	m_cefBrowsers.push_back(pCEFBrowser);
	
	m_browserMap[pCEFBrowser] = new CEFBrowserController(pCEFBrowser);
	//m_browserMap[browser]->Resize(0, 0);
	m_NewWebBrowserControllerPromise.set_value(m_browserMap[pCEFBrowser]);
}

bool CEFHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	if (m_cefBrowsers.size() == 1) {
		m_fShuttingdown = true;
	}

	return false;
}

void CEFHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		if ((*it)->IsSame(browser)) {
			m_cefBrowsers.erase(it);
			break;
		}
	}

	// don't quit cef when browsers are destroyed to keep the browser service running
	//if (m_browsers.empty()) {
	//	CefQuitMessageLoop();
	//}
}

void CEFHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) 
{
	CEF_REQUIRE_UI_THREAD();
}

void CEFHandler::CloseAllBrowsers(bool fForceClose) {
	RESULT r = R_PASS;

	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI, base::Bind(&CEFHandler::CloseAllBrowsers, this, fForceClose));
		return;
	}

	if (m_cefBrowsers.empty()) {
		CefQuitMessageLoop();
		return;
	}

	for (auto it = m_cefBrowsers.begin(); it != m_cefBrowsers.end(); it++) {
		(*it)->GetHost()->CloseBrowser(fForceClose);
	}

	if (m_cefBrowsers.empty()) {
		CefQuitMessageLoop();
	}

//Error:
	return;
}

WebBrowserController* CEFHandler::CreateBrowser(unsigned int width, unsigned int height, const std::string& url) {
	CefWindowInfo cefWindowInfo;
	CefBrowserSettings cefBrowserSettings;

	cefWindowInfo.SetAsWindowless(0, false);
	cefWindowInfo.width = width;
	cefWindowInfo.height = height;

	// clear the promise for reuse
	m_NewWebBrowserControllerPromise = std::promise<CEFBrowserController*>();

	auto newBrowser = m_NewWebBrowserControllerPromise.get_future();
	
	if (!CefBrowserHost::CreateBrowser(cefWindowInfo, this, url, cefBrowserSettings, nullptr)) {
		LOG(ERROR) << "CreateBrowser failed";
		return nullptr;
	}

	// blocks until promise is settled
	WebBrowserController* pBrowserController = newBrowser.get();

	pBrowserController->Resize(width, height);

	return pBrowserController;
}

bool CEFHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
	return DelegateToController(browser, [&](CEFBrowserController* controller) {
		controller->GetViewRect(browser, rect);
	});
}

void CEFHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	DelegateToController(browser, [&](CEFBrowserController* controller) {
		controller->OnPaint(browser, type, dirtyRects, buffer, width, height);
	});
}

inline bool CEFHandler::DelegateToController(CefRefPtr<CefBrowser> browser, std::function<void(CEFBrowserController* controller)> func) {
	auto foundItem = std::find_if(m_browserMap.begin(), m_browserMap.end(),
		[&](std::pair<CefRefPtr<CefBrowser>, CEFBrowserController*> const& item) {
			return item.first->IsSame(browser);
		}
	);

	if (foundItem != std::end(m_browserMap)) {
		func((*foundItem).second);
		return true;
	}

	return false;
}

