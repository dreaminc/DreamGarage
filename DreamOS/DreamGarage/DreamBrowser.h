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
class AudioPacket;

#include "DreamShareViewMessage.h"

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

	virtual RESULT SetURI(std::string strURI) = 0;
};

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
	public DreamBrowserHandle,
	public WebBrowserController::observer
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

	RESULT BroadcastDreamBrowserMessage(DreamShareViewMessage::type msgType, DreamShareViewMessage::type ackType = DreamShareViewMessage::type::INVALID);

	// InteractionObjectEvent
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);
	bool m_fTestQuadActive = false;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnAudioPacket(const AudioPacket &pendingAudioPacket) override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward, std::string strCurrentURL) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode, std::string strCurrentURL) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;

	virtual RESULT GetResourceHandlerType(ResourceHandlerType &resourceHandlerType,std::string strURL) override;

	virtual RESULT HandleBackEvent() override;
	virtual RESULT HandleForwardEvent() override;
	virtual RESULT HandleStopEvent() override;

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

	virtual RESULT SetBrowserScope(std::string strScope) override;
	virtual RESULT SetBrowserPath(std::string strPath) override;

	RESULT SetEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	virtual RESULT SetURI(std::string strURI) override;
	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

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
};

#endif // ! DREAM_CONTENT_VIEW_H_
