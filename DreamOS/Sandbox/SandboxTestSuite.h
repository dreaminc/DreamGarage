#ifndef SANDBOX_TEST_SUITE_H_
#define SANDBOX_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/SandboxTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class SandboxTestSuite : public TestSuite
{
public:
	SandboxTestSuite(DreamOS *pDreamOS);
	~SandboxTestSuite();

	virtual RESULT AddTests() override;

public:

	// Tests
	RESULT AddTestObjectPipeline();
	

private:
	RESULT SetupPipeline(std::string strRenderProgramName = "environment");
	RESULT SetupDreamAppPipeline();

private:
	DreamOS * m_pDreamOS = nullptr;
};

#endif // ! SANDBOX_TEST_SUITE_H_