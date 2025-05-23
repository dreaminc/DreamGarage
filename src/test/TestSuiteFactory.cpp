#include "TestSuiteFactory.h"

// TODO: Maybe revisit this approach 

#include "suites/PhysicsEngineTestSuite.h"
#include "suites/CollisionTestSuite.h"
#include "suites/InteractionEngineTestSuite.h"
#include "suites/AnimationTestSuite.h"
#include "suites/UITestSuite.h"
#include "suites/UIViewTestSuite.h"
#include "suites/CloudTestSuite.h"
#include "suites/WebRTCTestSuite.h"
#include "suites/HALTestSuite.h"
#include "suites/PipelineTestSuite.h"
#include "suites/DOSTestSuite.h"
#include "suites/MultiContentTestSuite.h"
#include "suites/SoundTestSuite.h"
#include "suites/SandboxTestSuite.h"
#include "suites/DimensionTestSuite.h"

std::shared_ptr<TestSuite> TestSuiteFactory::Make(TEST_SUITE_TYPE type, void *pContext) {
	RESULT r = R_PASS;
	std::shared_ptr<TestSuite> pTestSuite = nullptr;

	switch (type) {
		case TEST_SUITE_TYPE::PHYSICS: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<PhysicsEngineTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::COLLISION: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<CollisionTestSuite>((DreamOS*)pContext);
		} break;
		
		case TEST_SUITE_TYPE::INTERACTION: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<InteractionEngineTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::ANIMATION: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<AnimationTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::CLOUD: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			
			DreamOS* pDreamOS = (DreamOS*)(pContext);
			CNM(pDreamOS, "This test suite requires a valid DreamOS handle to be passed as context")

			pTestSuite = std::make_shared<CloudTestSuite>(pDreamOS);
		} break;

		case TEST_SUITE_TYPE::WEBRTC: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<WebRTCTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::MULTICONTENT: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<MultiContentTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::UI: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<UITestSuite>((DreamOS*)pContext);			
		} break;

		case TEST_SUITE_TYPE::OS: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<DOSTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::SOUND: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<SoundTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::UIVIEW: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<UIViewTestSuite>((DreamOS*)pContext);			
		} break;

		case TEST_SUITE_TYPE::HAL: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<HALTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::PIPELINE: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<PipelineTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::SANDBOX: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<SandboxTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::MATH: {
			// TODO: TODO
			//pTestSuite = std::make_shared<MATHTestSuite>(nullptr);
		} break;

		case TEST_SUITE_TYPE::DIMENSION: {
			CNM(pContext, "This test suite requires DreamOS to be passed as context");
			pTestSuite = std::make_shared<DimensionTestSuite>((DreamOS*)pContext);
		} break;

		default: {
			pTestSuite = nullptr;
			DEBUG_LINEOUT("Test Suite %d not supported on this platform!", type);
		} break;
	}

	CNM(pTestSuite, "Failed to allocate test suite");
	CRM(pTestSuite->Initialize(), "Failed to initialize test suite");

	return pTestSuite;

Error:
	pTestSuite = nullptr;
	return nullptr;
}