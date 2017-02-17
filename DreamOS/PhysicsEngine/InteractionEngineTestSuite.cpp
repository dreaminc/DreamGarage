#include "InteractionEngineTestSuite.h"
#include "DreamOS.h"

#include "PhysicsEngine/CollisionManifold.h"

InteractionEngineTestSuite::InteractionEngineTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

InteractionEngineTestSuite::~InteractionEngineTestSuite() {
	// empty
}

RESULT InteractionEngineTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestCompositeRay());

Error:
	return r;
}

RESULT InteractionEngineTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestCompositeRay() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		
		// TODO

		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}