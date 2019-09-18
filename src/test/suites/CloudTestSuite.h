#ifndef CLOUD_TEST_SUITE_H_
#define CLOUD_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Cloud Test Suite
// dos/src/test/suites/CloudTestSuite.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

#include "Cloud/CloudController.h"
#include "Cloud/Menu/MenuController.h"

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentViewApp;

// Test Users login (refresh) tokens for users test<key>@dreamos.com
extern std::map<int, std::string> k_refreshTokens;

class CloudTestSuite : public DreamTestSuite, 
					   public MenuController::observer,
					   public CloudController::EnvironmentObserver
{
public:
	CloudTestSuite(DreamOS *pDreamOS);
	~CloudTestSuite();

	virtual RESULT AddTests() override;

public:

	// Tests
	RESULT AddTestDownloadFile();
	RESULT AddTestConnectLogin();
	RESULT AddTestMenuAPI();
	RESULT AddTestMultiConnectTest();

	RESULT AddTestSwitchingEnvironmentSockets();

	//TODO: remove when tests don't need pipelines
	virtual RESULT SetupPipeline(std::string strRenderShaderName = "standard") override;
	virtual RESULT SetupTestSuite() override;

	// Functionality
	RESULT LaunchDreamView();

	// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

	// Environment Asset Callback
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;

	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnSendCameraPlacement() override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnStopSendingCameraPlacement() override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnReceiveCameraPlacement(long userID) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	virtual RESULT OnStopReceivingCameraPlacement() override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

	// Environment observer forms
	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override {
		return R_NOT_IMPLEMENTED_WARNING;
	}

private:
	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS = nullptr;
	CloudController *m_pCloudController = nullptr;
	UserController *m_pUserController = nullptr;
	std::shared_ptr<DreamContentViewApp> m_pDreamContentView = nullptr;

public:
	static std::string GetTestUserRefreshToken(int testUserID) {
		return k_refreshTokens[testUserID];
	}
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
