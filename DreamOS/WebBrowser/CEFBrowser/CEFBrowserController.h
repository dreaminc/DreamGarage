#ifndef CEF_BROWSER_CONTROLLER_H_
#define CEF_BROWSER_CONTROLLER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/WebBrowser/CEFBrowser/CEFBrowserController.h

#include "WebBrowser/WebBrowserController.h"

/*
#include "include\cef_client.h"
#include "include\cef_base.h"
#include "include\cef_app.h"
#include "include\internal\cef_win.h"

#include "include\cef_sandbox_win.h"
*/

#ifdef LOG
#undef LOG
#endif

#ifdef PLOG
#undef PLOG
#endif

#include "include\cef_base.h"
#include "include/cef_render_handler.h"
#include "include/cef_load_handler.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#include <list>
#include <future>

#include "Sound/AudioPacket.h"

// Theirs 
class CefBrowser;
class CefRequest;
class CefPostData;

// Ours
class WebRequest;
class WebRequestPostData;

class CEFDOMNode;

class CEFBrowserController :  public WebBrowserController {

	RESULT RegisterCEFAppObserver(CEFBrowserController::observer* pCEFBrowserControllerObserver);
public:
	CEFBrowserController(CefRefPtr<CefBrowser> pCEFBrowser);
	~CEFBrowserController();

	// WebBrowserController
	virtual RESULT PollFrame() override;
	virtual RESULT PollNewDirtyFrames(int &rNumFramesProcessed) override;		// TODO: This should be reviewed 
	virtual RESULT PollPendingAudioPackets(int &numAudioPacketsProcessed) override;
	virtual RESULT Resize(unsigned int width, unsigned int height) override;
	virtual RESULT LoadURL(const std::string& strURL) override;
	virtual RESULT LoadRequest(const WebRequest &webRequest) override;

	CefRefPtr<CefRequest> MakeCEFRequest(const WebRequest &webRequest);
	CefRefPtr<CefPostData> MakeCEFRequestPostData(std::shared_ptr<WebRequestPostData> pWebRequestPostData);

	// WebBrowser Controller Render Handling
	RESULT OnGetViewRect(CefRect &cefRect);
	RESULT OnAfterCreated();
	RESULT OnPaint(CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList &dirtyRects, const void *pBuffer, int width, int height);
	RESULT OnPopupSize(const CefRect& rect);
	RESULT OnAudioData(CefRefPtr<CefBrowser> pCEFBrowser, int frames, int channels, int bitsPerSample, const void* pDataBuffer);
	RESULT OnLoadingStateChanged(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL);
	RESULT OnLoadStart(CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::TransitionType transition_type);
	RESULT OnLoadEnd(CefRefPtr<CefFrame> pCEFFrame, int httpStatusCode);
	RESULT OnLoadError(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefLoadHandler::ErrorCode errorCode, const CefString& strError, const CefString& strFailedURL);
	//RESULT OnFocusedNodeChanged(CefRefPtr<CefBrowser> pCEFBrowser, CefRefPtr<CefFrame> pCEFFrame, CefRefPtr<CefDOMNode> pCEFDOMNode);
	RESULT OnFocusedNodeChanged(int cefBrowserID, int cefFrameID, CEFDOMNode *pCEFDOMNode);
	bool OnCertificateError(std::string strURL, unsigned int certError);

	RESULT SetIsSecureConnection(bool fSecure);

	RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType, CefString strCEFURL);
	RESULT CheckForHeaders(std::multimap<std::string, std::string> &headermap, std::string strURL);

	RESULT HandleDreamFormSuccess();
	RESULT HandleDreamFormCancel();
	RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& strAccessToken);
	RESULT HandleDreamFormSetEnvironmentId(int environmentId);

	RESULT HandleIsInputFocused(bool fInputFocused);

	RESULT HandleCanTabNext(bool fTabNext);
	RESULT HandleCanTabPrevious(bool fTabPrevious);

	virtual RESULT SendMouseClick(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseDown, int clickCount = 1) override;
	virtual RESULT SendMouseMove(const WebBrowserMouseEvent& webBrowserMouseEvent, bool fMouseLeave = false) override; 
	virtual RESULT SendMouseWheel(const WebBrowserMouseEvent& webBrowserMouseEvent, int deltaX, int deltaY) override;	
	virtual RESULT CloseBrowser() override;

	virtual RESULT SendKeyEventChar(char chKey, bool fKeyDown) override;
	virtual RESULT SendKeySequence(const std::string& strKeySequence) override;

	virtual RESULT Shutdown() override; 

	CefRefPtr<CefBrowser> GetCEFBrowser();

	// Frames
	virtual size_t GetFrameCount() override;

	// Browser Controls
	virtual bool CanGoBack() override;
	virtual bool CanGoForward() override;
	virtual RESULT GoBack() override;
	virtual RESULT GoForward() override;

	virtual RESULT TabNext() override;
	virtual RESULT TabPrevious() override;
	virtual RESULT CanTabNext() override;
	virtual RESULT CanTabPrevious() override;

	virtual RESULT IsInputFocused() override;
	virtual RESULT UnfocusInput() override;

	// Get Focused DOM element
	//virtual RESULT GetFocusedNode() override;

private:
	// browser logical size
	int m_browserWidth = 0;
	int m_browserHeight = 0;

	// Buffer for the browser content to render into
	std::vector<unsigned char> m_vectorFrameBuffer;
	std::vector<unsigned char> m_vectorPopupBuffer;
	WebBrowserController::PAINT_ELEMENT_TYPE m_paintType;
	
	CefRect m_popupRect;
	
	// browser physical size (buffer size)
	int m_bufferWidth = 0;
	int m_bufferHeight = 0;

	bool m_fUseOGLPBO = false;

	// reference to the browser object
	CefRefPtr<CefBrowser> m_pCEFBrowser;

	// Represented new dirty frames since last time they were polled
	CefRenderHandler::RectList m_NewDirtyFrames;
	std::mutex m_BufferMutex;

	std::queue<AudioPacket> m_pendingAudioPackets;

public:
	AudioPacket PopPendingAudioPacket();
	RESULT PushPendingAudioPacket(int frames, int channels, int bitsPerSample, uint8_t *pDataBfufer);
	bool IsAudioPacketPending();
	RESULT ClearPendingAudioPacketQueue();
	size_t PendingAudioPacketQueueLength();

	//IMPLEMENT_REFCOUNTING(CEFBrowserController);
};

#endif // ! CEF_BROWSER_CONTROLLER_H_