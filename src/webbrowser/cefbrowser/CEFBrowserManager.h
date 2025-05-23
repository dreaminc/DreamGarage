#ifndef CEF_BROWSER_MANAGER_H_
#define CEF_BROWSER_MANAGER_H_

#include "core/ehm/EHM.h"

// Dream Webbrowser CEF 
// dos/src/webbrowser/CEFBrowser/CEFBrowserManager.h

// The CEF implementation of browser manager

#define CEF_PROCESS_NAME_DEFAULT "DreamCef.exe"

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

#include "webbrowser/WebBrowserManager.h"

#include "cloud/Environment/EnvironmentAsset.h"

#include "CEFAppObserver.h"

class CEFApp;
class CEFBrowserController;

class CEFBrowserManager : public WebBrowserManager, public CEFAppObserver {
public:
	enum class state {
		UNINITIALIZED,
		INITIALIZING,
		INITIALIZED,
		INITIALIZATION_FAILED,
		INVALID
	};

public:
	CEFBrowserManager();
	~CEFBrowserManager();

public:
	virtual RESULT Initialize() override;
	virtual RESULT Update() override;
	virtual RESULT Shutdown() override;

	virtual std::shared_ptr<WebBrowserController> MakeNewBrowser(int width, int height, const std::string& strURL) override;
	virtual RESULT UpdateJobProcesses() override;

	virtual RESULT DeleteCookies() override;

private:
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual RESULT OnAfterCreated(CefRefPtr<CefBrowser> pCEFBrowser) override;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) override;
	virtual RESULT OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
	virtual RESULT OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int audioSteamID, int frames, int channels, int bitsPerSample, const void* pDataBuffer) override;
	virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) override;
	virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) override;
	virtual RESULT OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::ErrorCode errorCode, const CefString& strError, const CefString& strFailedURL) override;
	virtual RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) override;
	virtual bool OnCertificateError(CefRefPtr<CefBrowser> browser, cef_errorcode_t cert_error, const CefString& request_url, CefRefPtr<CefSSLInfo> ssl_info, CefRefPtr<CefRequestCallback> callback) override;

	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) override;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(int cefBrowserID) override;

	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL)  override;
	virtual RESULT CheckForHeaders(std::multimap<std::string, std::string> &headermap, CefRefPtr<CefBrowser> pCefBrowser, std::string strURL) override;

	virtual RESULT HandleDreamExtensionCall(CefRefPtr<CefBrowser> pCefBrowser, CefRefPtr<CefListValue> pMessageArguments) override;

private:
	RESULT CEFManagerThread();

private:
	state m_state = state::UNINITIALIZED;

	const std::string k_CEFProcessName = CEF_PROCESS_NAME_DEFAULT;
	std::thread m_ServiceThread;

	HANDLE m_hDreamJob = nullptr;
	bool m_fUpdateJob = false;

	std::mutex m_mutex;
};

#endif // ! CEF_BROWSER_MANAGER_H_