#ifndef DIMENSION_TEST_SUITE_H_
#define DIMENSION_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Dimension Test Suite
// dos/src/test/suites/DimensionTestSuite.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

class DreamOS;

class DimensionTestSuite : public DreamTestSuite {
public:
	DimensionTestSuite(DreamOS *pDreamOS);
	~DimensionTestSuite() = default;

	virtual RESULT AddTests() override;
	virtual RESULT SetupTestSuite() override;

public:
	RESULT AddTestDimObjRotationFunctions();

	RESULT AddTestVectorRotationFunctions();
};

#endif // ! DIMENSION_TEST_SUITE_H_