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
 
class DreamUserControlArea : public DreamApp<DreamUserControlArea>, public Subscriber<InteractionObjectEvent> {
	friend class DreamAppManager;
	friend class MultiContentTestSuite;

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

// DreamControlView
public:
	int GetPXWidth();
	int GetPXHeight();

	RESULT SendContactAtPoint(WebBrowserPoint ptContact, bool fMouseDown);
	RESULT SendMalletMoveEvent(WebBrowserPoint mousePoint);
	RESULT SendKeyCharacter(char chkey, bool fKeyDown);
	RESULT ScrollByDiff(int pxXDiff, int pxYDiff, WebBrowserPoint scrollPoint);

	//TODO: Set Scope/Path should be removed once DreamUIBar follows "open" implementation
	RESULT SetScope(std::string strScope);
	RESULT SetPath(std::string strPath);
	RESULT RequestOpenAsset(std::string strScope, std::string strPath, std::string strTitle);
	RESULT SendURL();

// DreamControlBar
public:
	RESULT HandleControlBarEvent(ControlEventType type);

	bool CanPressButton(UIButton *pButtonContext);

// DreamTabView
public:
	std::shared_ptr<DreamBrowser> GetActiveBrowser();
	RESULT SetActiveBrowser(std::shared_ptr<DreamBrowser> pNewBrowser);

// DreamBrowser
public:
	RESULT UpdateTextureForBrowser(std::shared_ptr<texture> pTexture, DreamBrowser* pContext);
	RESULT UpdateControlBarText(std::string& strTitle);
	//TODO: present keyboard from browser::OnNodeFocusChanged
	RESULT ShowKeyboard(std::string strInitial, point ptTextBox);
	bool IsContentVisible();

// DreamGarage compatability (temp?)
public:
	RESULT ResetAppComposite();
	RESULT AddEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);
	RESULT SetUIProgramNode(UIStageProgram *pUIProgramNode);

	RESULT CloseActiveAsset();

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
	std::shared_ptr<DreamBrowser> m_pActiveBrowser;

	//TODO: list of objects that relate to the right bar
	//std::vector<std::shared_ptr<DreamApp>> m_openApps;

	// string saved from KEY_DOWN interaction events
	// this is used for opening a URL, key presses are sent directly to the active browser
	std::string m_strURL;

// logic
private:
	bool m_fHasOpenApp;

	bool m_fCanPressButton[2];

// layout variables
private:
	float m_spacingSize = SPACING_SIZE;
	float m_pxWidth = DEFAULT_PX_WIDTH;
	float m_pxHeight = DEFAULT_PX_HEIGHT;

	float m_diagonalSize = MAIN_DIAGONAL;
	float m_aspectRatio;
	float m_baseWidth;
	float m_baseHeight;
};

#endif // ! DREAM_USER_CONTROL_AREA_H_