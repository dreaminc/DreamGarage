#include "HALTestSuite.h"
#include "DreamOS.h"

HALTestSuite::HALTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

HALTestSuite::~HALTestSuite() {
	// empty
}

RESULT HALTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestFramerateVolumes());

Error:
	return r;
}

RESULT HALTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT HALTestSuite::AddTestFramerateVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 40.0f;
	int nRepeats = 1;

	float width = 0.25f;
	float height = width;
	float length = width;

	float padding = 0.2f;

	int numObj = 7;

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		m_pDreamOS->SetGravityState(false);

		volume *pVolume = nullptr;  
		
		for (int i = 0; i < numObj; i++) {
			for (int j = 0; j < numObj; j++) {
				for (int k = 0; k < numObj; k++) {
					float xPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (i * (width + padding));
					float yPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (j * (width + padding));
					float zPos = (-1.0f * ((float)numObj / 2.0f)) * (width + padding) + (k * (width + padding));
					
					pVolume = m_pDreamOS->AddVolume(width, height, length);
					pVolume->SetPosition(point(xPos, yPos, zPos));
				}
			}
		}

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

	pNewTest->SetTestName("Framerate Volumes");
	pNewTest->SetTestDescription("Test frame rate vs many volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

HALImp* HALTestSuite::GetHALImp() {
	return m_pDreamOS->GetHALImp();
}