#ifndef SANDBOX_TEST_SUITE_H_
#define SANDBOX_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sandbox/SandboxTestSuite.h

#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

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