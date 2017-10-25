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

#define DEFAULT_SCROLL_FACTOR 5

class quad;
class sphere;
class texture;

class EnvironmentAsset;
class WebBrowserManager;
class DOMNode;

class DreamBrowserHandle : public DreamAppHandle {
public:
	RESULT SetScope(std::string strScope);
	RESULT SetPath(std::string strPath);
	
	RESULT ScrollTo(int pxXScroll, int pxYScroll);		// Absolute- scroll to this point
	RESULT ScrollToX(int pxXScroll);
	RESULT ScrollToY(int pyYScroll);
	
	RESULT ScrollByDiff(int pxXDiff, int pxYDiff);			// Relative- scroll this far
	RESULT ScrollXByDiff(int pxXDiff);
	RESULT ScrollYByDiff(int pxYDiff);

	RESULT SendClickToBrowserAtPoint(WebBrowserPoint ptContact);
	
	int GetScrollPixelsX();
	int GetScrollPixelsY();

	int GetPageHeightFromBrowser();
	int GetPageWidthFromBrowser();

	int GetHeightOfBrowser();
	int GetWidthOfBrowser();
	float GetAspectRatioFromBrowser();

	// GetFrameFocused();		//textbox detection
	std::shared_ptr<texture> GetBrowserTexture();

private:
	virtual RESULT SetBrowserScope(std::string strScope) = 0;
	virtual RESULT SetBrowserPath(std::string strPath) = 0;
	
	virtual RESULT ScrollBrowserToPoint(int pxXScroll, int pxYScroll) = 0;		// Absolute- scroll to this point
	virtual RESULT ScrollBrowserToX(int pxXScroll) = 0;
	virtual RESULT ScrollBrowserToY(int pyYScroll) = 0;
	
	virtual RESULT ScrollBrowserByDiff(int pxXDiff, int pxYDiff) = 0;			// Relative- scroll this far
	virtual RESULT ScrollBrowserXByDiff(int pxXDiff) = 0;
	virtual RESULT ScrollBrowserYByDiff(int pxYDiff) = 0;
	
	virtual RESULT ClickBrowser(WebBrowserPoint ptContact) = 0;

	virtual int GetScrollX() = 0;
	virtual int GetScrollY() = 0;
	
	virtual int GetPageHeight() = 0;
	virtual int GetPageWidth() = 0;

	virtual int GetBrowserHeight() = 0;
	virtual int GetBrowserWidth() = 0;
	virtual float GetAspectRatio() = 0;

	virtual std::shared_ptr<texture> BrowserTexture() = 0;
};

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
	public DreamBrowserHandle,
	public Subscriber<InteractionObjectEvent>, 
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

	virtual RESULT ScrollBrowserByDiff(int pxXDiff, int pxYDiff) override;			// Relative- scroll this far
	virtual RESULT ScrollBrowserXByDiff(int pxXDiff) override;
	virtual RESULT ScrollBrowserYByDiff(int pxYDiff) override;

	virtual int GetScrollX() override;		// use to get position scrolled to
	virtual int GetScrollY() override;

	virtual int GetBrowserHeight() override;
	virtual int GetBrowserWidth() override;

	virtual int GetPageHeight() override;	// get page context
	virtual int GetPageWidth() override;

	virtual RESULT ClickBrowser(WebBrowserPoint ptDiff) override;
	virtual std::shared_ptr<texture> BrowserTexture() override;

	// InteractionObjectEvent
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;
	RESULT HandleTestQuadInteractionEvents(InteractionObjectEvent *pEvent);
	bool m_fTestQuadActive = false;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode) override;
	virtual RESULT OnNodeFocusChanged(DOMNode *pDOMNode) override;

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
	RESULT SetURI(std::string strURI);
	RESULT LoadRequest(const WebRequest &webRequest);

	RESULT SetScrollFactor(int scrollFactor);

	std::shared_ptr<texture> GetScreenTexture();
private:
	RESULT SetScreenTexture(texture *pTexture);

protected:
	static DreamBrowser* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

private:
	std::shared_ptr<quad> m_pBrowserQuad = nullptr;
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

	// Test Stuff
	std::shared_ptr<sphere> m_pTestSphereRelative = nullptr;
	sphere *m_pTestSphereAbsolute = nullptr;
	std::shared_ptr<quad> m_pTestQuad = nullptr;

	std::shared_ptr<composite> m_pPointerCursor = nullptr;
	std::shared_ptr<composite> m_pHandCursor = nullptr;

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager = nullptr;

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

	TextEntryString m_strEntered;

	std::string m_strScope;
	std::string m_strPath;
};

#endif // ! DREAM_CONTENT_VIEW_H_
