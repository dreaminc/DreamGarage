#ifndef PIPELINE_TEST_SUITE_H_
#define PIPELINE_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Pipeline Test Suite
// dos/src/test/suites/SoundTestSuite.h

#include <functional>
#include <memory>

#include "test/suites/DreamTestSuite.h"

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
	RESULT AddTestDirtyNode();

	// TODO: 
	// Flags (no render)
	
};

#endif // ! PIPELINE_TEST_SUITE_H_