#ifndef DREAM_OS_TEST_SUITE_H_
#define DREAM_OS_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"
#include "Primitives/Subscriber.h"
#include "InteractionEngine/InteractionEngine.h"

#include <functional>
#include <memory>

class DreamOS;
class UIStageProgram;
struct WebBrowserPoint;

class DreamOSTestSuite : public TestSuite, public Subscriber<InteractionObjectEvent>
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
	RESULT AddTestDreamDesktop();

	// Capturing Apps
	RESULT AddTestCaptureApp();

	// Casting
	RESULT AddTestDreamShareView();
	RESULT AddTestBasicBrowserCast();

private:
	RESULT SetupPipeline(std::string strRenderProgramName = "environment");
	RESULT SetupDreamAppPipeline();

	// Used for creating a clickable quad in tests
public:
	RESULT Notify(InteractionObjectEvent *pEvent);
	WebBrowserPoint GetRelativeBrowserPointFromContact(point ptIntersectionContact);

private:
	DreamOS *m_pDreamOS = nullptr;
	UIStageProgram *m_pUIProgramNode;

//browser testing
private:
	bool m_fBrowserActive = false;
	//WebBrowserPoint m_lastWebBrowserPoint;	
};

#endif // ! DREAM_OS_TEST_SUITE_H_