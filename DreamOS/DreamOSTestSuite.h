#ifndef DREAM_OS_TEST_SUITE_H_
#define DREAM_OS_TEST_SUITE_H_

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/DreamTestSuite.h"

#include "Primitives/Subscriber.h"

#include "Primitives/point.h"     // for point
#include "RESULT/RESULT.h"        // for RESULT
#include "xstring"                // for string

struct InteractionObjectEvent;

class DreamOS;
class UIStageProgram;
struct WebBrowserPoint;

class DreamOSTestSuite : 
	public DreamTestSuite, 
	public Subscriber<InteractionObjectEvent>
{
public:
	DreamOSTestSuite(DreamOS *pDreamOS);
	~DreamOSTestSuite() = default;

	virtual RESULT AddTests() override;

	// TODO: in use?
	//RESULT SetupDreamAppPipeline();

	virtual RESULT SetupPipeline(std::string strRenderProgramName = "standard") override;
	virtual RESULT SetupTestSuite() override;

public:

	// Logging
	RESULT AddTestDreamLogger();

	// Tests
	RESULT AddTestDreamApps();
	RESULT AddTestUserApp();
	RESULT AddTestPeerApps();
	RESULT AddTestGamepadCamera();
	RESULT AddTestEnvironmentSwitching();
	RESULT AddTestEnvironmentSeating();

	// Meta UI
	RESULT AddTestMeta();

	// IPC
	RESULT AddTestNamedPipes();

	// Modules
	RESULT AddTestModuleManager();
	RESULT AddTestDreamVCam();
	RESULT AddTestDreamSoundSystem();
	RESULT AddTestDreamObjectModule();

	// Apps
	RESULT AddTestUIKeyboard();
	RESULT AddTestDreamUIBar();
	RESULT AddTestDreamBrowser();
	RESULT AddTestDreamOS();
	RESULT AddTestDreamDesktop();
	RESULT AddTestCaptureApp();

	// Casting
	RESULT AddTestDreamShareView();
	RESULT AddTestBasicBrowserCast();

	RESULT AddTestCredentialStorage();	

	// Used for creating a clickable quad in tests
public:
	RESULT Notify(InteractionObjectEvent *pEvent);
	WebBrowserPoint GetRelativeBrowserPointFromContact(point ptIntersectionContact);

private:
	UIStageProgram *m_pUIProgramNode = nullptr;

//browser testing
private:
	bool m_fBrowserActive = false;
	//WebBrowserPoint m_lastWebBrowserPoint;	
};

#endif // ! DREAM_OS_TEST_SUITE_H_