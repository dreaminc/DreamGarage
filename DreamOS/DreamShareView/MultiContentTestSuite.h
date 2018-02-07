#ifndef MULTI_CONTENT_TEST_SUITE_H_
#define MULTI_CONTENT_TEST_SUITE_H_

#include "Test/TestSuite.h"
#include "InteractionEngine/InteractionEngine.h"
#include "Primitives/Subscriber.h"

class DreamOS;
class DreamShareView; 
class DreamBrowser;
class CloudController;
class EnvironmentAsset;
class CEFBrowserManager;

class sphere;
class quad;
class composite;
class texture;

class MultiContentTestSuite : public TestSuite, public Subscriber<InteractionObjectEvent>, public valid
{
public:
	MultiContentTestSuite(DreamOS *pDreamOS);
	~MultiContentTestSuite();

	RESULT Initialize();

	virtual RESULT AddTests() override;

	RESULT SetupPipeline();

public:
	// Only share static textures between users
	//TODO: textures have some problems with deallocation in this test
	RESULT AddTestMultiPeerBasic();

	// Share website stream between users
	RESULT AddTestMultiPeerBrowser();

	// multiple browsers
	RESULT AddTestManyBrowsers();

public:
	virtual RESULT Notify(InteractionObjectEvent *pEvent) override;

private:
	DreamOS *m_pDreamOS;

	// members used with interaction engine events
	std::shared_ptr<DreamShareView> m_pDreamShareView = nullptr;
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
};

#endif // ! MULTI_CONTENT_TEST_SUITE_H_
