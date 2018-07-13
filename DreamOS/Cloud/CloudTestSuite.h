#ifndef CLOUD_TEST_SUITE_H_
#define CLOUD_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

#include "Cloud/CloudController.h"
#include "Cloud/Menu/MenuController.h"

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentView;

class CloudTestSuite : public TestSuite, 
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

	// Functionality
	RESULT LaunchDreamView();

	// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

	// Environment Asset Callback
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override;

	virtual RESULT OnReceiveAsset(long userID) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopSending() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopReceiving() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnShareAsset() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnCloseAsset() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override {
		return R_NOT_IMPLEMENTED;
	}

private:
	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS;
	CloudController *m_pCloudController = nullptr;
	std::shared_ptr<DreamContentView> m_pDreamContentView = nullptr;
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
