#ifndef DREAM_TEST_SUITE_H_
#define DREAM_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Test/DreamTestSuite.h
// Simple interface for dream specific test suites 
// Mostly forces the SetupPipeline virtual function

#include "Test/TestSuite.h"

class DreamTestSuite : public TestSuite {
public:
	DreamTestSuite(std::string strName) :
		TestSuite(strName)
	{
		// empty
	}

	~DreamTestSuite() = default;

public:
	virtual RESULT SetupPipeline(std::string strRenderProgramName) = 0;
};

#endif // DREAM_TEST_SUITE_H_