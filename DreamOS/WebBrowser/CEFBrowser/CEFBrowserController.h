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

class CEFBrowserController : public WebBrowserController {

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
	virtual RESULT ReplaceURL(const std::string& strURL) override;
	virtual RESULT LoadRequest(const WebRequest &webRequest) override;

	virtual bool CheckIsError(int errorCode) override;

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
	virtual RESULT ParseURL(std::string strURL, std::string& strParsedURL) override;

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
	int m_cefErrorCodes[54] = {
	  ERR_NONE,
	  ERR_FAILED,
	  ERR_ABORTED,
	  ERR_INVALID_ARGUMENT,
	  ERR_INVALID_HANDLE,
	  ERR_FILE_NOT_FOUND,
	  ERR_TIMED_OUT,
	  ERR_FILE_TOO_BIG,
	  ERR_UNEXPECTED,
	  ERR_ACCESS_DENIED,
	  ERR_NOT_IMPLEMENTED,
	  ERR_CONNECTION_CLOSED,
	  ERR_CONNECTION_RESET,
	  ERR_CONNECTION_REFUSED,
	  ERR_CONNECTION_ABORTED,
	  ERR_CONNECTION_FAILED,
	  ERR_NAME_NOT_RESOLVED,
	  ERR_INTERNET_DISCONNECTED,
	  ERR_SSL_PROTOCOL_ERROR,
	  ERR_ADDRESS_INVALID,
	  ERR_ADDRESS_UNREACHABLE,
	  ERR_SSL_CLIENT_AUTH_CERT_NEEDED,
	  ERR_TUNNEL_CONNECTION_FAILED,
	  ERR_NO_SSL_VERSIONS_ENABLED,
	  ERR_SSL_VERSION_OR_CIPHER_MISMATCH,
	  ERR_SSL_RENEGOTIATION_REQUESTED,
	  ERR_CERT_COMMON_NAME_INVALID,
	  ERR_CERT_DATE_INVALID,
	  ERR_CERT_AUTHORITY_INVALID,
	  ERR_CERT_CONTAINS_ERRORS,
	  ERR_CERT_NO_REVOCATION_MECHANISM,
	  ERR_CERT_UNABLE_TO_CHECK_REVOCATION,
	  ERR_CERT_REVOKED,
	  ERR_CERT_INVALID,
	  ERR_CERT_WEAK_SIGNATURE_ALGORITHM,
	  ERR_CERT_NON_UNIQUE_NAME,
	  ERR_CERT_WEAK_KEY,
	  ERR_CERT_NAME_CONSTRAINT_VIOLATION,
	  ERR_CERT_VALIDITY_TOO_LONG,
	  ERR_CERT_END,
	  ERR_INVALID_URL,
	  ERR_DISALLOWED_URL_SCHEME,
	  ERR_UNKNOWN_URL_SCHEME,
	  ERR_TOO_MANY_REDIRECTS,
	  ERR_UNSAFE_REDIRECT,
	  ERR_UNSAFE_PORT,
	  ERR_INVALID_RESPONSE,
	  ERR_INVALID_CHUNKED_ENCODING,
	  ERR_METHOD_NOT_SUPPORTED,
	  ERR_UNEXPECTED_PROXY_AUTH,
	  ERR_EMPTY_RESPONSE,
	  ERR_RESPONSE_HEADERS_TOO_BIG,
	  ERR_CACHE_MISS,
	  ERR_INSECURE_RESPONSE 
	};

};

#endif // ! CEF_BROWSER_CONTROLLER_H_