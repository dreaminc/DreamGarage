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
#include "Cloud/Environment/EnvironmentAsset.h"

#include <map>
#include <vector>

#include "WebBrowser/WebBrowserController.h"

#include "Sense/SenseController.h"

#include "Primitives/TextEntryString.h"

#include "DreamVideoStreamSubscriber.h"

#include "DreamUserControlArea/DreamContentSource.h"

#include "Sound/SoundCommon.h"

#include "DreamGarage/UICommon.h"

#define DEFAULT_SCROLL_FACTOR 5

#define CEF_UPDATE_MS (1000.0/24.0)

class quad;
class sphere;
class texture;

class EnvironmentAsset;
class WebBrowserManager;
class DOMNode;
class DreamUserControlArea;
class AudioPacket;
class SoundBuffer;

#include "DreamShareViewShareMessage.h"

// TODO: Move to the standard DreamBrowser::observer arch
class DreamBrowserObserver {
public:
	virtual RESULT HandleAudioPacket(const AudioPacket &pendingAudioPacket, DreamContentSource *pContext) = 0;

	virtual RESULT UpdateControlBarText(std::string& strTitle) = 0;
	virtual RESULT UpdateControlBarNavigation(bool fCanGoBack, bool fCanGoForward) = 0;
	virtual RESULT UpdateURLBarSecurity(bool fSecure) = 0;

	virtual RESULT UpdateContentSourceTexture(texture* pTexture, std::shared_ptr<DreamContentSource> pContext) = 0;

	virtual RESULT HandleNodeFocusChanged(DOMNode *pDOMNode, DreamContentSource *pContext) = 0;
	virtual RESULT HandleIsInputFocused(bool fIsInputFocused, DreamContentSource *pContext) = 0;
	virtual RESULT HandleLoadEnd() = 0;

	virtual RESULT HandleDreamFormSuccess() = 0;
	virtual RESULT HandleDreamFormCancel() = 0;
	virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& accessToken) = 0;
	virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) = 0;

	virtual RESULT HandleCanTabNext(bool fCanNext) = 0;
	virtual RESULT HandleCanTabPrevious(bool fCanPrevious) = 0;
};

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
	public DreamContentSource,
	public WebBrowserController::observer
{
	friend class DreamAppManager;

public:
	DreamBrowser(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamBrowser();

	// DreamApp Interface
	virtual RESULT InitializeApp(void *pContext = nullptr);
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr);
	virtual RESULT Update(void *pContext = nullptr);
	virtual RESULT Shutdown(void *pContext = nullptr);

	virtual RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

	// control events
	RESULT ScrollBrowserToPoint(int pxXScroll, int pxYScroll);		// Absolute- scroll to this point
	RESULT ScrollBrowserToX(int pxXScroll);
	RESULT ScrollBrowserToY(int pyYScroll);

	RESULT ScrollBrowserXByDiff(int pxXDiff);
	RESULT ScrollBrowserYByDiff(int pxYDiff);

	virtual RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint) override;		// Relative- scroll this far

	int GetScrollX();		// use to get position scrolled to
	int GetScrollY();

	int GetBrowserHeight();
	int GetBrowserWidth();

	int GetPageHeight();	// get page context
	int GetPageWidth();

	virtual RESULT OnKeyPress(char chkey, bool fkeyDown) override;
	RESULT CreateBrowserSource(std::string strURL);

	virtual RESULT OnMouseMove(point mousePoint) override;
	virtual RESULT OnClick(point ptDiff, bool fMouseDown) override;

	RESULT BroadcastDreamBrowserMessage(DreamShareViewShareMessage::type msgType, DreamShareViewShareMessage::type ackType = DreamShareViewShareMessage::type::INVALID);

	// InteractionObjectEvent
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);
	bool m_fTestQuadActive = false;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const void *pBuffer, int width, int height, WebBrowserController::PAINT_ELEMENT_TYPE type, WebBrowserRect rect) override;
	virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT OnAfterCreated() override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode, std::string strCurrentURL) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;
	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType,std::string strURL) override;
	virtual RESULT CheckForHeaders(std::multimap<std::string, std::string> &headermap, std::string strURL) override;
	virtual RESULT SetTitle(std::string strTitle) override;
	virtual RESULT SetIsSecureConnection(bool fSecure) override;

	virtual RESULT HandleDreamFormSuccess() override;
	virtual RESULT HandleDreamFormCancel() override;
	virtual RESULT HandleDreamFormSetCredentials(std::string& strRefreshToken, std::string& strAccessToken) override;
	virtual RESULT HandleDreamFormSetEnvironmentId(int environmentId) override;

	virtual RESULT HandleIsInputFocused(bool fInputFocused) override;
	virtual RESULT HandleCanTabNext(bool fCanNext) override;
	virtual RESULT HandleCanTabPrevious(bool fCanPrevious) override;

	virtual RESULT HandleBackEvent();
	virtual RESULT HandleForwardEvent();
	virtual RESULT HandleStopEvent();

	RESULT SetForceObserverAudio(bool fForceObserverAudio);

	RESULT HandleTabEvent();
	RESULT HandleBackTabEvent();
	RESULT HandleUnfocusEvent();

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);

	float GetWidthFromAspectDiagonal();
	float GetHeightFromAspectDiagonal();
	vector GetNormal();
	point GetOrigin();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	// DreamContentSource
	virtual RESULT SetScope(std::string strScope) override;
	virtual RESULT SetPath(std::string strPath) override;

	virtual int GetHeight() override;
	virtual int GetWidth() override;

	virtual std::string GetTitle() override;
	virtual std::string GetContentType() override;

	virtual std::string GetScheme() override;
	virtual std::string GetURL() override;

	RESULT PendEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	virtual RESULT SetURI(std::string strURI);
	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

	RESULT InitializeWithBrowserManager(std::shared_ptr<WebBrowserManager> pWebBrowserManager, std::string strURL);

	virtual texture* GetSourceTexture() override;
	virtual long GetCurrentAssetID() override;
	RESULT SetCurrentAssetID(long assetID);

	virtual RESULT CloseSource() override;
	virtual RESULT SendFirstFrame() override;

	RESULT PendUpdateObjectTextures();
	bool ShouldUpdateObjectTextures();
	RESULT UpdateObjectTextures();

	RESULT UpdateNavigationFlags();

	RESULT RegisterObserver(DreamBrowserObserver *pObserver);
	RESULT UnregisterObserver(DreamBrowserObserver *pObserver);

private:
	DreamBrowserObserver *m_pObserver = nullptr;

protected:
	static DreamBrowser* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager = nullptr;

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	std::chrono::high_resolution_clock::time_point m_tLastUpdate;

	WebBrowserPoint m_lastWebBrowserPoint;	// This is so scrolling can get which frame the mouse is on - e.g. drop down menus are now scrollable

	int m_browserWidth = BROWSER_WIDTH;
	int m_browserHeight = BROWSER_HEIGHT;
	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	int m_pxXPosition = 0;
	int m_pxYPosition = 0;

	int m_pxXScroll = 0;
	int m_pxYScroll = 0;

	WebBrowserMouseEvent m_mouseScrollEvent;

	WebBrowserMouseEvent m_mouseDragEvent;
	bool m_fUpdateDrag = false;

	int m_scrollFactor = DEFAULT_SCROLL_FACTOR;

	int m_pageDepth = 0; // hack to avoid the loading page on back
	std::string m_strCurrentTitle;
	std::string m_strCurrentURL;
	bool m_fSecure = false;

	TextEntryString m_strEntered;
	
	std::string m_strScope;
	std::string m_strPath;
	std::string m_strContentType;

	long m_currentEnvironmentAssetID = 0;
	std::map<std::string, ResourceHandlerType> m_dreamResourceHandlerLinks;
	std::map<std::string, std::multimap<std::string, std::string>> m_headermap;

	bool m_fShowControlView = false;

	bool m_fShouldBeginStream = false;
	bool m_fSendFrame = false;
	bool m_fFirstFrameIsReady = false;
	int m_sentFrames = 0;
	double m_msTimeLastSent = 0.0;
	double m_msTimeBetweenSends = 100.0;

	double m_msLastScreenUpdate = 0.0;
	double m_msTimeBetweenUpdates = CEF_UPDATE_MS;

	unsigned char *m_pLoadBuffer = nullptr;
	size_t m_pLoadBuffer_n = 0;

	std::shared_ptr<EnvironmentAsset> m_pPendingEnvironmentAsset;

	// CEF can call LoadRequest once a URL is loaded
	bool m_fCanLoadRequest = false;
	bool m_fUpdateObjectTextures = false;
	bool m_fUpdateControlBarInfo = false;

	long m_assetID = -1;

	// TODO: Convert into configuration struct or flag system
	bool m_fForceObserverAudio = false;

private:
	// when the user goes to a URL that starts with these strings, we send their auth token as well
	// TODO: build native URL parser for future flexibility
	std::vector<std::string> m_authenticatedURLs = {
		"http://localhost:8001",
		"http://localhost:8002",
		"https://api.develop.dreamos.com",
		"https://www.develop.dreamos.com",
		"https://api.dreamos.com",
		"https://dreamos.com"
	};

	// Sound stuff
private:
	RESULT InitializeDreamBrowserSoundSystem();
	RESULT InitializeRenderSoundBuffer(int numChannels, int samplingRate, sound::type bufferType);
	RESULT AudioProcess();

	sound::state m_soundState = sound::state::UNINITIALIZED;
	SoundBuffer *m_pRenderSoundBuffer = nullptr;
	std::thread	m_browserAudioProcessingThread;
};

#endif // ! DREAM_CONTENT_VIEW_H_
