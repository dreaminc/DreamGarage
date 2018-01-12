#ifndef DREAM_BROWSER_VIEW_H_
#define DREAM_BROWSER_VIEW_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamApp.h
// The Dream Content view is a rudimentary Dream application 
// that effectively is a single quad that can load / present 
// content of various formats 

#include "DreamApp.h"
#include "DreamAppHandle.h"

#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include <map>
#include <vector>

#include "WebBrowser/WebBrowserController.h"

#include "Sense/SenseController.h"

#include "Primitives/TextEntryString.h"

#include "DreamVideoStreamSubscriber.h"

#define DEFAULT_SCROLL_FACTOR 5

class quad;
class sphere;
class texture;

class EnvironmentAsset;
class WebBrowserManager;
class DOMNode;
class DreamUserHandle;
class AudioPacket;

#include "DreamBrowserMessage.h"

class DreamBrowserHandle : public DreamAppHandle {
public:
	RESULT SetScope(std::string strScope);
	RESULT SetPath(std::string strPath);

	RESULT ScrollTo(int pxXScroll, int pxYScroll);		// Absolute- scroll to this point
	RESULT ScrollToX(int pxXScroll);
	RESULT ScrollToY(int pyYScroll);
	
	RESULT ScrollByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint);			// Relative- scroll this far
	RESULT ScrollXByDiff(int pxXDiff);
	RESULT ScrollYByDiff(int pxYDiff);

	RESULT SendMalletMoveEvent(WebBrowserPoint mousePoint);
	RESULT SendContactToBrowserAtPoint(WebBrowserPoint ptContact, bool fMouseDown);

	RESULT SendBackEvent();
	RESULT SendForwardEvent();
	RESULT SendStopEvent();

	RESULT SendKeyCharacter(char chKey, bool fkeyDown);
	virtual RESULT SendURL (std::string strURL) = 0;
	RESULT SendURI(std::string strURI);

	int GetScrollPixelsX();
	int GetScrollPixelsY();

	int GetPageHeightFromBrowser();
	int GetPageWidthFromBrowser();

	int GetHeightOfBrowser();
	int GetWidthOfBrowser();
	float GetAspectRatioFromBrowser();

	RESULT RequestBeginStream();

private:
	virtual RESULT SetBrowserScope(std::string strScope) = 0;
	virtual RESULT SetBrowserPath(std::string strPath) = 0;
	
	virtual RESULT ScrollBrowserToPoint(int pxXScroll, int pxYScroll) = 0;		// Absolute- scroll to this point
	virtual RESULT ScrollBrowserToX(int pxXScroll) = 0;
	virtual RESULT ScrollBrowserToY(int pyYScroll) = 0;
	
	virtual RESULT ScrollBrowserByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) = 0;			// Relative- scroll this far
	virtual RESULT ScrollBrowserXByDiff(int pxXDiff) = 0;
	virtual RESULT ScrollBrowserYByDiff(int pxYDiff) = 0;
	
	virtual RESULT SendKeyPressed(char chkey, bool fkeyDown) = 0;

	virtual RESULT SendMouseMoveEvent(WebBrowserPoint mousePoint) = 0;

	virtual RESULT HandleBackEvent() = 0;
	virtual RESULT HandleForwardEvent() = 0;
	virtual RESULT HandleStopEvent() = 0;

	virtual RESULT ClickBrowser(WebBrowserPoint ptContact, bool fMouseDown) = 0;

	virtual int GetScrollX() = 0;
	virtual int GetScrollY() = 0;
	
	virtual int GetPageHeight() = 0;
	virtual int GetPageWidth() = 0;

	virtual int GetBrowserHeight() = 0;
	virtual int GetBrowserWidth() = 0;
	virtual float GetAspectRatio() = 0;

	virtual RESULT BeginStream() = 0;

	virtual RESULT SetURI(std::string strURI) = 0;
};

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
	public DreamBrowserHandle,
	public Subscriber<InteractionObjectEvent>, 
	public WebBrowserController::observer,
	public DreamVideoStreamSubscriber
{
	friend class DreamAppManager;

public:
	DreamBrowser(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamBrowser();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

	virtual RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) override;

	virtual DreamAppHandle* GetAppHandle() override;

	// DreamBrowserHandle 
	virtual RESULT ScrollBrowserToPoint(int pxXScroll, int pxYScroll) override;		// Absolute- scroll to this point
	virtual RESULT ScrollBrowserToX(int pxXScroll) override;
	virtual RESULT ScrollBrowserToY(int pyYScroll) override;

	virtual RESULT ScrollBrowserByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint) override;		// Relative- scroll this far
	virtual RESULT ScrollBrowserXByDiff(int pxXDiff) override;
	virtual RESULT ScrollBrowserYByDiff(int pxYDiff) override;

	virtual int GetScrollX() override;		// use to get position scrolled to
	virtual int GetScrollY() override;

	virtual int GetBrowserHeight() override;
	virtual int GetBrowserWidth() override;

	virtual int GetPageHeight() override;	// get page context
	virtual int GetPageWidth() override;

	virtual RESULT SendKeyPressed(char chkey, bool fkeyDown);
	virtual RESULT SendURL(std::string strURL);

	virtual RESULT SendMouseMoveEvent(WebBrowserPoint mousePoint) override;
	virtual RESULT ClickBrowser(WebBrowserPoint ptDiff, bool fMouseDown) override;

	virtual RESULT BeginStream() override;

	// Set streaming state in both the browser and the user app
	RESULT SetStreamingState(bool fStreaming);
	bool IsStreaming();

	RESULT BroadcastDreamBrowserMessage(DreamBrowserMessage::type msgType, DreamBrowserMessage::type ackType = DreamBrowserMessage::type::INVALID);

	// InteractionObjectEvent
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);
	bool m_fTestQuadActive = false;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode, std::string strCurrentURL) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;

	virtual RESULT HandleBackEvent() override;
	virtual RESULT HandleForwardEvent() override;
	virtual RESULT HandleStopEvent() override;

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);

	RESULT FadeQuadToBlack();

	WebBrowserPoint GetRelativeBrowserPointFromContact(point ptIntersectionContact);

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();
	virtual float GetAspectRatio() override;

	RESULT UpdateViewQuad();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	virtual RESULT SetBrowserScope(std::string strScope) override;
	virtual RESULT SetBrowserPath(std::string strPath) override;

	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT StopSending();
	RESULT StartReceiving(PeerConnection *pPeerConnection);
	RESULT PendReceiving();
	RESULT StopReceiving();
	virtual RESULT SetURI(std::string strURI) override;
	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

	std::shared_ptr<texture> GetScreenTexture();
private:
	RESULT SetScreenTexture(texture *pTexture);

public:
	// Video Stream Subscriber
	virtual RESULT OnVideoFrame(PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight) override;
	RESULT SetupPendingVideoFrame(uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	RESULT UpdateFromPendingVideoFrame();

	struct PendingFrame {
		bool fPending = false;
		int pxWidth = 0;
		int pxHeight = 0;
		uint8_t *pDataBuffer = nullptr;
		size_t pDataBuffer_n = 0;
	} m_pendingFrame;

protected:
	static DreamBrowser* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pBrowserQuad = nullptr;
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

#ifdef _USE_TEST_APP
	// Test Stuff
	std::shared_ptr<sphere> m_pTestSphereRelative = nullptr;
	sphere *m_pTestSphereAbsolute = nullptr;
	std::shared_ptr<quad> m_pTestQuad = nullptr;
	std::shared_ptr<composite> m_pPointerCursor = nullptr;
#endif

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager = nullptr;
	DreamUserHandle* m_pDreamUserHandle = nullptr;

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	WebBrowserPoint m_lastWebBrowserPoint;	// This is so scrolling can get which frame the mouse is on - e.g. drop down menus are now scrollable
	bool m_fBrowserActive = false;

	int m_browserWidth = 1366;
	int m_browserHeight = 768;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	int m_pxXPosition = 0;
	int m_pxYPosition = 0;

	int m_scrollFactor = DEFAULT_SCROLL_FACTOR;

	int m_pageDepth = 0; // hack to avoid the loading page on back
	std::string m_strCurrentURL;

	bool m_fStreaming = false;
	bool m_fReceivingStream = false;

	TextEntryString m_strEntered;
	
	std::string m_strScope;
	std::string m_strPath;
	std::string m_strContentType;
	long m_currentEnvironmentAssetID = 0;

	DreamBrowserMessage::type m_currentMessageType;
	DreamBrowserMessage::type m_currentAckType;

	bool m_fShowControlView = false;

};

#endif // ! DREAM_CONTENT_VIEW_H_
