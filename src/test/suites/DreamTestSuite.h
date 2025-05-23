#ifndef DREAM_TEST_SUITE_H_
#define DREAM_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Test/DreamTestSuite.h
// Simple interface for dream specific test suites 
// Mostly forces the SetupPipeline virtual function

#include "Test/TestSuite.h"

class DreamOS;

class DreamTestSuite : public TestSuite {
public:
	DreamTestSuite(std::string strName, DreamOS *pParentDreamOS);

	~DreamTestSuite() = default;

protected:
	virtual RESULT ResetTest(void *pContext);

	virtual RESULT DefaultInitializeProcess(void *pContext) override;
	virtual RESULT DefaultUpdateProcess(void *pContext) override;
	virtual RESULT DefaultEvaluateProcess(void *pContext) override;
	virtual RESULT DefaultResetProcess(void *pContext) override;

public:
	virtual RESULT SetupPipeline(std::string strRenderProgramName = "standard");

protected:
	DreamOS *m_pDreamOS = nullptr;
};

#endif // DREAM_TEST_SUITE_H_