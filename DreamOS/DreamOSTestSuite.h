#ifndef DREAM_OS_TEST_SUITE_H_
#define DREAM_OS_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;
class UIStageProgram;

class DreamOSTestSuite : public TestSuite
{
public:
	DreamOSTestSuite(DreamOS *pDreamOS);
	~DreamOSTestSuite();

	virtual RESULT AddTests() override;

public:

	// Tests
	RESULT AddTestDreamApps();
	RESULT AddTestUserApp();
	RESULT AddTestPeerApps();

	// Apps
	RESULT AddTestUIKeyboard();
	RESULT AddTestDreamUIBar();
	RESULT AddTestDreamBrowser();
	RESULT AddTestDreamOS();

	// Capturing Apps
	RESULT AddTestCaptureApp();

	// Casting
	RESULT AddTestDreamShareView();

private:
	RESULT SetupPipeline(std::string strRenderProgramName = "environment");
	RESULT SetupDreamAppPipeline();

private:
	DreamOS *m_pDreamOS = nullptr;
	UIStageProgram *m_pUIProgramNode;
};

#endif // ! DREAM_OS_TEST_SUITE_H_