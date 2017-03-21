#include "TestSuiteFactory.h"

#include "PhysicsEngine/PhysicsEngineTestSuite.h"
#include "InteractionEngine/InteractionEngineTestSuite.h"
#include "UI/UITestSuite.h"
#include "Cloud/CloudTestSuite.h"
#include "HAL/HALTestSuite.h"

std::shared_ptr<TestSuite> TestSuiteFactory::Make(TEST_SUITE_TYPE type, void *pContext) {
	RESULT r = R_PASS;
	std::shared_ptr<TestSuite> pTestSuite = nullptr;

	switch (type) {
		case TEST_SUITE_TYPE::PHYSICS: {
			CNM(pContext, "This test suite requires DreamOS to be bassed as context");
			pTestSuite = std::make_shared<PhysicsEngineTestSuite>((DreamOS*)pContext);
		} break;
		
		case TEST_SUITE_TYPE::INTERACTION: {
			CNM(pContext, "This test suite requires DreamOS to be bassed as context");
			pTestSuite = std::make_shared<InteractionEngineTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::CLOUD: {
			CNM(pContext, "This test suite requires DreamOS to be bassed as context");
			pTestSuite = std::make_shared<CloudTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::UI: {
			CNM(pContext, "This test suite requires DreamOS to be bassed as context");
			pTestSuite = std::make_shared<UITestSuite>((DreamOS*)pContext);			
		} break;

		case TEST_SUITE_TYPE::HAL: {
			CNM(pContext, "This test suite requires DreamOS to be bassed as context");
			pTestSuite = std::make_shared<HALTestSuite>((DreamOS*)pContext);
		} break;

		case TEST_SUITE_TYPE::MATH: {
			// TODO: TODO
			//pTestSuite = std::make_shared<MATHTestSuite>(nullptr);
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