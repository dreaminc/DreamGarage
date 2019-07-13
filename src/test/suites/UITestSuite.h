#ifndef UI_TEST_SUITE_H_
#define UI_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream UI Test Suite
// dos/src/test/suites/UITestSuite.h

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <memory>

#include "core/types/Subscriber.h"

#include "DreamTestSuite.h"

#include "cloud/CloudController.h"
#include "cloud/Menu/MenuController.h"

class DreamOS;
class DreamUIBarApp;
class UIKeyboard;
class VirtualObj;
class sphere;
class quad;

class DreamBrowserApp;

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

class UITestSuite : public DreamTestSuite, 
					public Subscriber<SenseControllerEvent>, public Subscriber<SenseKeyboardEvent>, 
					public Subscriber<SenseMouseEvent>,
					public Subscriber<UIEvent>,
					public MenuController::observer,
					public CloudController::EnvironmentObserver
{
public:
	UITestSuite(DreamOS *pDreamOS);
	~UITestSuite() = default;

	virtual RESULT SetupTestSuite() override;

	RESULT AddTestBrowserURL();
	RESULT AddTestInteractionFauxUI();
	RESULT AddTestSharedContentView();
	RESULT AddTestBrowserRequest();
	RESULT AddTestBrowserRequestWithMenuAPI();
	RESULT AddTestUIView();
	RESULT AddTestUIMenuItem();
	RESULT AddTestFont();
	RESULT AddTestFlatContextCompositionQuads();
	//RESULT AddTestFlatContext();

	virtual RESULT AddTests() override;

	// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

	// Environment Asset Callback
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;

	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnSendCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopSendingCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnReceiveCameraPlacement(long userID) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopReceivingCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	// Environment observer forms
	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override {
		return R_NOT_IMPLEMENTED;
	}

public:
	virtual RESULT Notify(SenseControllerEvent *event) override;
	virtual RESULT Notify(SenseKeyboardEvent *kbEvent) override;
	virtual RESULT Notify(SenseMouseEvent *mEvent) override;
	virtual RESULT Notify(UIEvent *mEvent) override;

private:
	virtual RESULT SetupPipeline(std::string strRenderProgramName = "environment") override;
	//RESULT SetupUINodePipeline();

private:
	
	std::shared_ptr<DreamBrowserApp> m_pDreamBrowserApp = nullptr;
	CloudController *m_pCloudController = nullptr;
	std::shared_ptr<DreamUIBarApp> m_pDreamUIBar;

	VirtualObj* m_pPrevSelected;

	std::map<std::string, std::vector<std::string>> m_menu;
	std::stack<std::string> m_path;

	sphere *m_pSphere1;
	sphere *m_pSphere2;
};



#endif UI_TEST_SUITE_H_