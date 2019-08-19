#ifndef MULTI_CONTENT_TEST_SUITE_H_
#define MULTI_CONTENT_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// TODO: Merge this with the other tests (in other words do a real test plan)
// Dream Multi Content Test Suite
// dos/src/test/suites/MultiContentTestSuite.h

#include "DreamTestSuite.h"

#include "core/types/Subscriber.h"

#include "modules/InteractionEngine/InteractionEngine.h"

class DreamOS;
class DreamShareViewApp; 
class DreamBrowser;
class CloudController;
class EnvironmentAsset;
class CEFBrowserManager;

class sphere;
class quad;
class composite;
class texture;
class UIStageProgram;

class MultiContentTestSuite : public DreamTestSuite, public Subscriber<InteractionObjectEvent>
{
public:
	MultiContentTestSuite(DreamOS *pDreamOS);
	~MultiContentTestSuite();

	RESULT Initialize();

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderProgramName = "standard") override;
	virtual RESULT SetupTestSuite() override;

public:

	// forms
	RESULT AddTestLoginForms();

	// test to fix ui clipping shader
	RESULT AddTestMenuShader();

	// test active source switching
	RESULT AddTestActiveSource();

	// build out UserControlArea features
	RESULT AddTestUserControlArea();

	// test out the feel of the first design spec without committing to arch yet
	RESULT AddTestUserControlAreaLayout();

	// Only share static textures between users
	//TODO: textures have some problems with deallocation in this test
	RESULT AddTestMultiPeerBasic();

	// Share website stream between users
	RESULT AddTestMultiPeerBrowser();

	// multiple browsers
	RESULT AddTestManyBrowsers();

	// flat context with dream tab view
	RESULT AddTestDreamTabView();

	// all ui objects visible at once
	RESULT AddTestAllUIObjects();

	RESULT AddTestCameraSettings();
	RESULT AddTestChangeUIWidth();
	RESULT AddTestDreamSettingsApp();

	// test for memory leaks in the menu
	RESULT AddTestMenuMemory();
	RESULT AddTestRemoveObjects();
	RESULT AddTestRemoveObjects2();
	RESULT AddTestRemoveText();

public:
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

private:
	DreamOS *m_pDreamOS;

	// members used with interaction engine events
	std::shared_ptr<DreamShareViewApp> m_pDreamShareViewApp = nullptr;
	std::shared_ptr<DreamBrowser> m_pDreamBrowser = nullptr;
	std::shared_ptr<quad> m_pTestQuad = nullptr;
	std::shared_ptr<composite> m_pPointerCursor = nullptr;
	std::string m_strID;
	bool m_fTestQuadActive = false;

	std::shared_ptr<texture> m_pTestTexture = nullptr;
	std::shared_ptr<texture> m_pTestTextureUser1 = nullptr;
	std::shared_ptr<texture> m_pTestTextureUser2 = nullptr;

	std::shared_ptr<quad> m_pTestQuad1 = nullptr;
	std::shared_ptr<quad> m_pTestQuad2 = nullptr;

	std::string m_strURL;
	UIStageProgram *m_pUIProgramNode = nullptr;;
};

#endif // ! MULTI_CONTENT_TEST_SUITE_H_
