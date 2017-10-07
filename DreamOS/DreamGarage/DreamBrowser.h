#ifndef DREAM_BROWSER_VIEW_H_
#define DREAM_BROWSER_VIEW_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamApp.h
// The Dream Content view is a rudimentary Dream application 
// that effectively is a single quad that can load / present 
// content of various formats 

#include "DreamApp.h"
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

class DreamBrowser : 
	public DreamApp<DreamBrowser>, 
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

	virtual std::shared_ptr<DreamAppHandle> GetAppHandle() override;

	// InteractionObjectEvent
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

	// WebBrowserController Observer
	virtual RESULT OnPaint(const WebBrowserRect &rect, const void *pBuffer, int width, int height) override;
	virtual RESULT OnLoadingStateChange(bool fLoading, bool fCanGoBack, bool fCanGoForward) override;
	virtual RESULT OnLoadStart() override;
	virtual RESULT OnLoadEnd(int httpStatusCode) override;

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
	float GetAspectRatio();

	RESULT UpdateViewQuad();

	bool IsVisible();
	RESULT SetVisible(bool fVisible);

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
	std::shared_ptr<sphere> m_pTestSphereRelative = nullptr;
	sphere *m_pTestSphereAbsolute = nullptr;
	std::shared_ptr<quad> m_pBrowserQuad = nullptr;
	std::shared_ptr<texture> m_pBrowserTexture = nullptr;

	std::shared_ptr<composite> m_pPointerCursor = nullptr;
	std::shared_ptr<composite> m_pHandCursor = nullptr;

	std::shared_ptr<WebBrowserController> m_pWebBrowserController = nullptr;
	std::shared_ptr<WebBrowserManager> m_pWebBrowserManager = nullptr;

	WebBrowserPoint m_lastWebBrowserPoint;
	bool m_fBrowserActive = false;

	float m_aspectRatio = 1.0f;
	float m_diagonalSize = 5.0f;
	vector m_vNormal;

	int m_scrollFactor = DEFAULT_SCROLL_FACTOR;

	TextEntryString m_strEntered;
};

#endif // ! DREAM_CONTENT_VIEW_H_