#ifndef UI_TEST_SUITE_H_
#define UI_TEST_SUITE_H_

#include "RESULT/EHM.h"

#include "Primitives/valid.h"
#include "Primitives/Subscriber.h"
#include "Test/TestSuite.h"

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>

#include "Cloud/Menu/MenuController.h"

class DreamOS;
class DreamUIBar;
class UIKeyboard;
class VirtualObj;
class sphere;
class quad;

class EnvironmentAsset;
class DreamBrowser;

struct SenseControllerEvent;
struct SenseKeyboardEvent;
struct SenseMouseEvent;
struct InteractionObjectEvent;
struct UIEvent;

struct KeyboardTestContext {
	sphere* pSphere = nullptr;
	sphere* pSphere2 = nullptr;
	quad* pQuad = nullptr;
};

class UITestSuite : public valid, public TestSuite, 
					public Subscriber<SenseControllerEvent>, public Subscriber<SenseKeyboardEvent>, 
					public Subscriber<SenseMouseEvent>,
					public Subscriber<UIEvent>,
					public MenuController::observer
{
public:
	UITestSuite(DreamOS *pDreamOS);
	~UITestSuite();

	RESULT Initialize();

	RESULT AddTestInteractionFauxUI();
	RESULT AddTestSharedContentView();
	RESULT AddTestBrowser();
	RESULT AddTestBrowserRequest();
	RESULT AddTestBrowserRequestWithMenuAPI();
	RESULT AddTestKeyboard();
	RESULT AddTestUIView();
	RESULT AddTestUIMenuItem();
	RESULT AddTestFont();
	RESULT AddTestFlatContextCompositionQuads();

	virtual RESULT AddTests() override;

	// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

	// Environment Asset Callback
	RESULT HandleOnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);

public:
	virtual RESULT Notify(SenseControllerEvent *event) override;
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseMouseEvent *mEvent) override;
	virtual RESULT Notify(UIEvent *mEvent) override;

private:
	RESULT SetupPipeline();
	RESULT SetupUINodePipeline();

private:
	DreamOS *m_pDreamOS;
	std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;
	CloudController *m_pCloudController = nullptr;
	std::shared_ptr<DreamUIBar> m_pDreamUIBar;

	VirtualObj* m_pPrevSelected;

	std::map<std::string, std::vector<std::string>> m_menu;
	std::stack<std::string> m_path;

	sphere *m_pSphere1;
	sphere *m_pSphere2;

	std::shared_ptr<UIKeyboard> m_pKeyboard;
};



#endif UI_TEST_SUITE_H_