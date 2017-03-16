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

class CloudTestSuite : public TestSuite, public MenuController::observer {
public:
	CloudTestSuite(DreamOS *pDreamOS);
	~CloudTestSuite();

	virtual RESULT AddTests() override;

public:
	RESULT AddTestConnectLogin();
	RESULT AddTestMenuAPI();

	RESULT AddTestMultiConnectTest();

// Menu Controller Observer
	RESULT OnMenuData(std::shared_ptr<MenuNode> pMenuNode);

private:
	CloudController *GetCloudController();

private:
	DreamOS *m_pDreamOS;
	CloudController *m_pCloudController = nullptr;
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
