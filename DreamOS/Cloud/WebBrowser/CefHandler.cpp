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
	CefHandler* g_instance = NULL;
}

CefHandler::CefHandler() :
	m_isShuttingDown(false) {
	DCHECK(!g_instance);
	g_instance = this;
}

CefHandler::~CefHandler() {
	g_instance = nullptr;
}

CefHandler* CefHandler::GetInstance() {
	return g_instance;
}

void CefHandler::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD();

	//
}

void CefHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) {
	CEF_REQUIRE_UI_THREAD();

	//
}

void CefHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	
	m_browsers.push_back(browser);
	
	m_browserMap[browser] = new CefBrowserController(browser);
	//m_browserMap[browser]->Resize(0, 0);
	m_NewWebBrowserControllerPromise.set_value(m_browserMap[browser]);
}

bool CefHandler::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	if (m_browsers.size() == 1) {
		m_isShuttingDown = true;
	}

	return false;
}

void CefHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();

	for (BrowserList::iterator it = m_browsers.begin(); it != m_browsers.end(); ++it) {
		if ((*it)->IsSame(browser)) {
			m_browsers.erase(it);
			break;
		}
	}

	// don't quit cef when browsers are destroyed to keep the browser service running
	//if (m_browsers.empty()) {
	//	CefQuitMessageLoop();
	//}
}

void CefHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	ErrorCode errorCode,
	const CefString& errorText,
	const CefString& failedUrl) {
	CEF_REQUIRE_UI_THREAD();

	//
}

void CefHandler::CloseAllBrowsers(bool force_close) {
	if (!CefCurrentlyOn(TID_UI)) {
		// Execute on the UI thread.
		CefPostTask(TID_UI,
			base::Bind(&CefHandler::CloseAllBrowsers, this, force_close));
		return;
	}

	if (m_browsers.empty()) {
		CefQuitMessageLoop();
		return;
	}

	for (BrowserList::const_iterator it = m_browsers.begin(); it != m_browsers.end(); ++it)
		(*it)->GetHost()->CloseBrowser(force_close);

	if (m_browsers.empty()) {
		CefQuitMessageLoop();
	}
}

WebBrowserController*	CefHandler::CreateBrowser(unsigned int width, unsigned int height, const std::string& url) {
	CefWindowInfo window_info;
	CefBrowserSettings browserSettings;

	window_info.SetAsWindowless(0, false);
	window_info.width = width;
	window_info.height = height;

	// clear the promise for reuse
	m_NewWebBrowserControllerPromise = std::promise<CefBrowserController*>();

	auto newBrowser = m_NewWebBrowserControllerPromise.get_future();
	
	if (!CefBrowserHost::CreateBrowser(window_info, this, url, browserSettings, nullptr)) {
		LOG(ERROR) << "CreateBrowser failed";
		return nullptr;
	}

	// blocks until promise is settled
	auto browserController = newBrowser.get();

	browserController->Resize(width, height);

	return browserController;
}

bool CefHandler::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
	return DelegateToController(browser, [&](CefBrowserController* controller) {
		controller->GetViewRect(browser, rect);
	});
}

void CefHandler::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height) {
	DelegateToController(browser, [&](CefBrowserController* controller) {
		controller->OnPaint(browser, type, dirtyRects, buffer, width, height);
	});
}

inline bool CefHandler::DelegateToController(CefRefPtr<CefBrowser> browser, std::function<void(CefBrowserController* controller)> func) {
	auto foundItem = std::find_if(m_browserMap.begin(), m_browserMap.end(),
		[&](std::pair<CefRefPtr<CefBrowser>, CefBrowserController*> const& item)
	{
		return item.first->IsSame(browser);
	});

	if (foundItem != std::end(m_browserMap)) {
		func((*foundItem).second);
		return true;
	}

	return false;
}

