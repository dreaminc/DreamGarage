#ifndef PIPELINE_TEST_SUITE_H_
#define PIPELINE_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Sound/SoundTestSuite.h

#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class PipelineTestSuite : public DreamTestSuite
{
public:
	PipelineTestSuite(DreamOS *pDreamOS);
	~PipelineTestSuite() = default;

	virtual RESULT AddTests() override;
	virtual RESULT SetupTestSuite() override;

public:

	// Tests

	RESULT AddTestEmptyPipeline();
	RESULT AddTestDynamicPipe();

	// TODO: 
	// Flags (no render)
	
};

#endif // ! PIPELINE_TEST_SUITE_H_