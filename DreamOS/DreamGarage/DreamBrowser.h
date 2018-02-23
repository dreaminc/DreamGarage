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

#define DEFAULT_SCROLL_FACTOR 5

class quad;
class sphere;
class texture;

class EnvironmentAsset;
class WebBrowserManager;
class DOMNode;
class DreamUserHandle;
class DreamUserControlArea;
class AudioPacket;

#include "DreamShareViewMessage.h"

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
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

	RESULT ScrollBrowserByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint);		// Relative- scroll this far
	RESULT ScrollBrowserXByDiff(int pxXDiff);
	RESULT ScrollBrowserYByDiff(int pxYDiff);

	int GetScrollX();		// use to get position scrolled to
	int GetScrollY();

	int GetBrowserHeight();
	int GetBrowserWidth();

	int GetPageHeight();	// get page context
	int GetPageWidth();

	RESULT SendKeyPressed(char chkey, bool fkeyDown);
	RESULT SendURL(std::string strURL);

	RESULT SendMouseMoveEvent(WebBrowserPoint mousePoint);
	RESULT ClickBrowser(WebBrowserPoint ptDiff, bool fMouseDown);

	RESULT BroadcastDreamBrowserMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType = DreamShareViewMessage::type::INVALID);

	// InteractionObjectEvent
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);
	bool m_fTestQuadActive = false;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT OnAfterCreated() override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode, std::string strCurrentURL) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;

	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType,std::string strURL) override;

	virtual RESULT HandleBackEvent();
	virtual RESULT HandleForwardEvent();
	virtual RESULT HandleStopEvent();

	RESULT SetPosition(point ptPosition);
	RESULT SetAspectRatio(float aspectRatio);
	RESULT SetDiagonalSize(float diagonalSize);
	RESULT SetNormalVector(vector vNormal);
	RESULT SetParams(point ptPosition, float diagonal, float aspectRatio, vector vNormal);

	float GetWidth();
	float GetHeight();
	vector GetNormal();
	point GetOrigin();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

	virtual RESULT SetBrowserScope(std::string strScope);
	virtual RESULT SetBrowserPath(std::string strPath);

	RESULT PendEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	virtual RESULT SetURI(std::string strURI);
	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

	RESULT InitializeWithBrowserManager(std::shared_ptr<WebBrowserManager> pWebBrowserManager, std::string strURL = "about:blank");
	RESULT InitializeWithParent(DreamUserControlArea *pParentApp);
	std::shared_ptr<texture> GetScreenTexture();

	RESULT CloseBrowser();

protected:
	static DreamBrowser* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager = nullptr;
	DreamUserHandle* m_pDreamUserHandle = nullptr;

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	WebBrowserPoint m_lastWebBrowserPoint;	// This is so scrolling can get which frame the mouse is on - e.g. drop down menus are now scrollable

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

	TextEntryString m_strEntered;
	
	std::string m_strScope;
	std::string m_strPath;
	std::string m_strContentType;
	long m_currentEnvironmentAssetID = 0;
	std::map<std::string, ResourceHandlerType> m_dreamResourceHandlerLinks;
	bool m_fShowControlView = false;

	bool m_fShouldBeginStream = false;

	std::shared_ptr<EnvironmentAsset> m_pPendingEnvironmentAsset;

	// CEF can call LoadRequest once a URL is loaded
	bool m_fCanLoadRequest = false;

	DreamUserControlArea *m_pParentApp = nullptr;

};

#endif // ! DREAM_CONTENT_VIEW_H_
