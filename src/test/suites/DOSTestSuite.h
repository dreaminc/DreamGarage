#ifndef DREAM_OS_TEST_SUITE_H_
#define DREAM_OS_TEST_SUITE_H_

#include "core/ehm/EHM.h"        // for RESULT

// Dream OS Test Suite
// dos/src/test/suites/DOSTestSuite.h

#include "xstring"                // for string

#include "DreamTestSuite.h"

#include "core/types/Subscriber.h"

#include "core/primitives/point.h"     // for point

struct InteractionObjectEvent;

class DreamOS;
class UIStageProgram;
struct WebBrowserPoint;

class DOSTestSuite : 
	public DreamTestSuite, 
	public Subscriber<InteractionObjectEvent>
{
public:
	DOSTestSuite(DreamOS *pDreamOS);
	~DOSTestSuite() = default;

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