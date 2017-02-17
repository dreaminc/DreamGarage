#ifndef INTERACTION_ENGINE_TEST_SUITE_H_
#define INTERACTION_ENGINE_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/matrix/MatrixTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class InteractionEngineTestSuite : public TestSuite {
public:
	InteractionEngineTestSuite(DreamOS *pDreamOS);
	~InteractionEngineTestSuite();

	virtual RESULT AddTests() override;

	RESULT AddTestCompositeRay();

	RESULT ResetTest(void *pContext);

private:
	DreamOS *m_pDreamOS;

};

#endif // ! INTERACTION_ENGINE_TEST_SUITE_H_
