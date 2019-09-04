#ifndef DIMENSION_TEST_SUITE_H_
#define DIMENSION_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/DimensionTestSuite.h

#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

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