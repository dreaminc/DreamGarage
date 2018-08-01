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

	//TODO: remove when tests don't need pipelines
	RESULT SetupPipeline();

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
	UserController *m_pUserController = nullptr;
	std::shared_ptr<DreamContentView> m_pDreamContentView = nullptr;

// Test Users login (refresh) tokens
// For users test<key>@dreamos.com
protected:
	std::map<int, std::string> m_tokens = {
		{0, "NakvA43v1eVBqvvTJuqUdXHWL02CNuDqrgHMEBrIY6P5FoHZ2GtgbCVDYvHMaRTw"},
		{1, "daehZbIcTcXaPh29tWQy75ZYSLrRL4prhBoBYMRQtU48NMs6svnt5CkzCA5RLKJq"},
		{2, "GckLS9Q691PO6RmdmwRp368JjWaETNOMEoASqQF0TCnImHzpmOv2Rch1RDrgr2V7"},
		{3, "HYlowX58aRPRB85IT0M2wB20RC8rd0zpOxfIIvEgMF9XVzzFbL8UzY3yyCovdEIQ"},
		{4, "sROmFa73UM38v7snrTaDy3JF1vCJGdJhBBLvBcCLaWxjoEYVfAqcgMAZPVHzaZrR"},
		{5, "gc2EPtlKmKtkmiZC6cRfUtMIHwiWW9Tf55wbFBcq45Wg8DBRDWV3iZiLsqBedfqF"},
		{6, "F5EwwHxmgf4pqLXZjP6zWH4NBn42UtLQUmrlU4vl62BGeprnug0Hn1WeMm3snHQa"},
		{7, "cuX1beJjJE58DdU4cYOrsIoNFil534fOWscH9bzmhmcFkV1qn3M8zPkdW7J3UEH1"},
		{8, "B3Wwz6Lbwfj2emo7caKBQXtKoMYXR9P70eOvkFzFIfh9NRlal6PLFqIagTFXiDHy"},
		{9, "HPfaNfjFrAhlbqS9DuZD5dCrAzI215ETDTRzFMVXrtoYrI2A9XBS3VEKOjGlDSVE"}
	};
};

#endif // ! MENU_CONTROLLER_TEST_SUITE_H_
