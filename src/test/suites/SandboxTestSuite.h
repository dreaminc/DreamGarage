#ifndef SANDBOX_TEST_SUITE_H_
#define SANDBOX_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Sandbox Test Suite
// dos/src/test/suites/SandboxTestSuite.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

class DreamOS;

class SandboxTestSuite : public DreamTestSuite
{
public:
	SandboxTestSuite(DreamOS *pDreamOS);
	~SandboxTestSuite();

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderProgramName = "standard") override;
	virtual RESULT SetupTestSuite() override;

public:

	// Tests
	RESULT AddTestObjectPipeline();
	RESULT AddTestCompositeObject();
	

private:
	RESULT SetupDreamAppPipeline();

private:
	DreamOS * m_pDreamOS = nullptr;
};

#endif // ! SANDBOX_TEST_SUITE_H_