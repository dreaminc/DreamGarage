#ifndef CEF_BROWSER_MANAGER_H_
#define CEF_BROWSER_MANAGER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/WebBrowser/CEFBrowser/CEFBrowserManager.h
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
#include "WebBrowser/WebBrowserManager.h"

#include "Cloud/Environment/EnvironmentAsset.h"

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

private:
	virtual RESULT OnGetViewRect(CefRefPtr<CefBrowser> pCEFBrowser, CefRect &cefRect) override;
	virtual RESULT OnPaint(CefRefPtr<CefBrowser> pCEFBrowser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height) override;
	virtual RESULT OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int frames, int channels, int bitsPerSample, const void* pDataBuffer) override;
	virtual RESULT OnLoadingStateChanged(CefRefPtr<CefBrowser> pCEFBrowser, bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT OnLoadStart(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type) override;
	virtual RESULT OnLoadEnd(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode) override;
	virtual RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode) override;

	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser) override;
	virtual std::shared_ptr<CEFBrowserController> GetCEFBrowserController(int cefBrowserID) override;

	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefRefPtr<CefBrowser> pCefBrowser, CefString strCEFURL)  override;

private:
	RESULT CEFManagerThread();

private:
	state m_state = state::UNINITIALIZED;

	const std::string k_CEFProcessName = CEF_PROCESS_NAME_DEFAULT;
	std::thread m_ServiceThread;

	std::mutex m_mutex;
	std::condition_variable m_condBrowserInit;
};

#endif // ! CEF_BROWSER_MANAGER_H_