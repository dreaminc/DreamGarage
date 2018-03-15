#ifndef DREAM_USER_CONTROL_AREA_H_
#define DREAM_USER_CONTROL_AREA_H_

#include "RESULT/EHM.h"

#include "DreamApp.h"
#include "DreamGarage/DreamControlBar.h"
#include "Primitives/Subscriber.h"

#include <vector>
#include <map>

class DreamUserApp;
class DreamControlView;
class DreamUIBar;
class DreamTabView;
class DreamBrowser;
class DreamContentSource;
class DreamDesktopApp;

class CEFBrowserManager;
struct WebBrowserPoint;
class EnvironmentAsset;

struct InteractionObjectEvent;

class UIStageProgram;

class quad;

#define MAIN_DIAGONAL 0.6f
#define SPACING_SIZE 0.016129f
#define DEFAULT_PX_WIDTH 1366
#define DEFAULT_PX_HEIGHT 768

#define VIEW_ANGLE 32.0f
#define VIEW_POS_DEPTH 0.1f	
#define VIEW_POS_HEIGHT -0.2f
#define TITLEDESKTOP "Desktop"
#define TITLEWEBSITE "website"
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;
	friend class DreamOSTestSuite;

public:
	DreamUserControlArea(DreamOS *pDreamOS, void *pContext = nullptr);
	~DreamUserControlArea();

// DreamApp
public:
	virtual RESULT InitializeApp(void *pContext) override;
	virtual RESULT OnAppDidFinishInitializing(void *pContext = nullptr) override;
	virtual RESULT Update(void *pContext = nullptr) override;
	virtual RESULT Shutdown(void *pContext = nullptr) override;

protected:
	static DreamUserControlArea* SelfConstruct(DreamOS *pDreamOS, void *pContext = nullptr);

// Expose size variables
public:
	float GetSpacingSize();
	float GetBaseWidth();
	float GetBaseHeight();

// Animations
public:
	RESULT Show();
	RESULT Hide();

// DreamControlView
public:
	int GetWidth();
	int GetHeight();

	RESULT OnClick(point ptContact, bool fMouseDown);
	RESULT OnMouseMove(point mousePoint);
	RESULT OnKeyPress(char chkey, bool fKeyDown);
	RESULT OnScroll(float pxXDiff, float pxYDiff, point scrollPoint);

	//TODO: Set Scope/Path should be removed once DreamUIBar follows "open" implementation
	RESULT SetScope(std::string strScope);
	RESULT SetPath(std::string strPath);
	RESULT RequestOpenAsset(std::string strScope, std::string strPath, std::string strTitle);
	RESULT CreateBrowserSource();
	RESULT SetActiveBrowserURI();
	RESULT HideWebsiteTyping();

// DreamControlBar
public:
	RESULT HandleControlBarEvent(ControlEventType type);

	bool CanPressButton(UIButton *pButtonContext);

// DreamTabView
public:
	std::shared_ptr<DreamContentSource> GetActiveSource();
	RESULT SetActiveSource(std::shared_ptr<DreamContentSource> pNewContent);

// DreamBrowser
public:
	RESULT UpdateTextureForBrowser(std::shared_ptr<texture> pTexture, DreamBrowser* pContext);
	RESULT UpdateTextureForDesktop(std::shared_ptr<texture> pTexture, DreamDesktopApp* pContext);
	RESULT UpdateControlBarText(std::string& strTitle);
	//TODO: present keyboard from browser::OnNodeFocusChanged
	RESULT ShowKeyboard(std::string strInitial, point ptTextBox);
	bool IsContentVisible();

// Dream Desktop
public:
	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth);

// DreamGarage compatability (temp?)
public:
	RESULT ResetAppComposite();
	RESULT AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetUIProgramNode(UIStageProgram *pUIProgramNode);

	RESULT CloseActiveAsset();
	RESULT OnReceiveAsset();

public:
	virtual RESULT Notify(InteractionObjectEvent *pSubscriberEvent) override;

// child applications
private:

	// positioning helper
	std::shared_ptr<DreamUserApp> m_pDreamUserApp;

	// Generates browsers
	std::shared_ptr<CEFBrowserManager> m_pWebBrowserManager;

	// App used for opening content
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	// Apps in control area
	std::shared_ptr<DreamControlBar> m_pControlBar;
	std::shared_ptr<DreamControlView> m_pControlView;
	std::shared_ptr<DreamTabView> m_pDreamTabView;

	//TODO: potentially a class Browser and Desktop extend that implements
	// the control view events, ContactAtPoint, ScrollByDiff, etc.
	// ControlViewObserver?
	std::shared_ptr<DreamContentSource> m_pActiveSource;

	//TODO: list of objects that relate to the right bar
	//std::vector<std::shared_ptr<DreamApp>> m_openApps;

	// string saved from KEY_DOWN interaction events
	// this is used for opening a URL, key presses are sent directly to the active browser
	std::string m_strURL;

// logic
private:
	bool m_fHasOpenApp;

	bool m_fCanPressButton[2];

	std::shared_ptr<texture> m_pLoadingScreenTexture = nullptr;

	bool m_fKeyboardUp = false;

// layout variables
private:
	float m_spacingSize = SPACING_SIZE;
	float m_pxWidth = DEFAULT_PX_WIDTH;
	float m_pxHeight = DEFAULT_PX_HEIGHT;

	float m_diagonalSize = MAIN_DIAGONAL;
	float m_aspectRatio;
	float m_baseWidth;
	float m_baseHeight;

	std::shared_ptr<DreamDesktopApp> m_pDreamDesktop = nullptr;

	std::string m_strDesktopTitle = TITLEDESKTOP;
	std::string m_strWebsiteTitle = TITLEWEBSITE;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_