#ifndef CLOUD_TEST_SUITE_H_
#define CLOUD_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

#include "Cloud/CloudController.h"
#include "Cloud/Menu/MenuController.h"

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentView;

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

private:
	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS = nullptr;
	CloudController *m_pCloudController = nullptr;
	UserController *m_pUserController = nullptr;
	std::shared_ptr<DreamContentView> m_pDreamContentView = nullptr;

public:
	static std::string GetTestUserRefreshToken(int testUserID) {
		return k_refreshTokens[testUserID];
	}
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
