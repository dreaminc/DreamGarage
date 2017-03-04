#include "CloudTestSuite.h"
#include "DreamOS.h"

CloudTestSuite::CloudTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

CloudTestSuite::~CloudTestSuite() {
	// empty
}

RESULT CloudTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestConnectLogin());

Error:
	return r;
}

RESULT CloudTestSuite::AddTestConnectLogin() {
	RESULT r = R_PASS;

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		RESULT r = R_PASS;

		// 
		CR(r);
		DEBUG_LINEOUT("TEST: hi");

	Error:
		return R_PASS;
	};

	// Add the test
	auto pNewTest = AddTest(fnTest);
	CN(pNewTest);

	pNewTest->SetTestName("Test Connect and Login");
	pNewTest->SetTestDescription("Test connect and log into service");

Error:
	return r;
}