#ifndef DREAM_OS_TEST_SUITE_H_
#define DREAM_OS_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Cloud/CloudTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class DreamOSTestSuite : public TestSuite
{
public:
	DreamOSTestSuite(DreamOS *pDreamOS);
	~DreamOSTestSuite();

	virtual RESULT AddTests() override;

public:

	// Tests
	RESULT AddTestDreamApps();
	RESULT AddTestUserApp();

private:
	RESULT SetupPipeline();

private:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // ! DREAM_OS_TEST_SUITE_H_