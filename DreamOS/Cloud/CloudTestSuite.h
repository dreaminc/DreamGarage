#ifndef CLOUD_TEST_SUITE_H_
#define CLOUD_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

#include "Cloud/Menu/MenuController.h"

class DreamOS;
class CloudController;
class EnvironmentAsset;

class DreamContentView;

class CloudTestSuite : public TestSuite, public MenuController::observer {
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
	RESULT HandleOnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset);

private:
	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS;
	CloudController *m_pCloudController = nullptr;
	std::shared_ptr<DreamContentView> m_pDreamContentView = nullptr;
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
