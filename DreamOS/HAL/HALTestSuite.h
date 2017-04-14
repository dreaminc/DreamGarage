#ifndef HAL_TEST_SUITE_H_
#define HAL_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/HALTestSuite.h

// This test suite should cover testing all functionality related to the
// HAL (graphics) layer and interfaces.

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;
class HALImp;

class HALTestSuite : public TestSuite {
public:
	HALTestSuite(DreamOS *pDreamOS);
	~HALTestSuite();

	virtual RESULT AddTests() override;

public:
	RESULT AddTestFramerateVolumes();
	RESULT AddTestAlphaVolumes();

private:
	RESULT ResetTest(void *pContext);

private:
	HALImp *GetHALImp();

private:
	DreamOS *m_pDreamOS;
	HALImp *m_pHALImp = nullptr;
};

#endif // ! HAL_TEST_SUITE_H_
