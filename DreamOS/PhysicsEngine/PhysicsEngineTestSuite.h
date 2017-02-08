#ifndef PHYSICS_ENGINE_TEST_SUITE_H_
#define PHYSICS_ENGINE_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitices/matrix/MatrixTestSuite.h

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class PhysicsEngineTestSuite : public TestSuite {
public:
	PhysicsEngineTestSuite(DreamOS *pDreamOS);
	~PhysicsEngineTestSuite();

	virtual RESULT AddTests() override;

	RESULT AddTestBallVolume();
	RESULT AddTestVolumeVolumePointFace();
	RESULT AddTestVolumeToPlaneVolume();
	RESULT AddTestSphereVsSphereArray();
	RESULT AddTestSphereVsSphere();
	RESULT AddTestSphereGenerator();
	RESULT AddTestQuadVsSphere();
	RESULT AddTestVolumeVolumeEdge();
	RESULT AddTestComposite();

	RESULT ResetTest(void *pContext);

private:
	DreamOS *m_pDreamOS;

};

#endif // ! PHYSICS_ENGINE_TEST_SUITE_H_
