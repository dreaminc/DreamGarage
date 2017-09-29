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

	RESULT AddTestRayModel();
	RESULT AddTestCompositeRay();
	RESULT AddTestMultiCompositeRayQuad();
	RESULT AddTestRay();
	RESULT AddTestRayQuads();
	RESULT AddTestRayQuadsComposite();
	RESULT AddTestBallVolume();
	RESULT AddTestVolumeVolumePointFace();
	RESULT AddTestVolumeToPlaneVolume();
	RESULT AddTestSphereVsSphereArray();
	RESULT AddTestSphereVsSphere();
	RESULT AddTestSphereGenerator();
	RESULT AddTestQuadVsSphere();
	RESULT AddTestVolumeVolumeEdge();
	RESULT AddTestCompositeComposition();
	RESULT AddTestCompositeCompositionQuads();
	RESULT AddTestCompositeCollisionSpheres();
	RESULT AddTestCompositeCollisionVolumes();
	RESULT AddTestCompositeCollisionSphereVolume();
	RESULT AddTestCompositeCollisionVolumeSphere();
	RESULT AddTestCompositeCollisionSphereQuads();
	
	RESULT AddTestBoundingScale();
	RESULT AddTestBoundingScaleSphereVolume();
	RESULT AddTestBoundingScaleSphereQuad();
	RESULT AddTestBoundingScaleSpheres();
	RESULT AddTestBoundingScaleVolumes();
	RESULT AddTestRayScaledQuads();
	RESULT AddTestMultiCompositeRayScaledQuad();


private:
	RESULT SetupSkyboxPipeline(std::string strRenderShaderName);
	RESULT ResetTest(void *pContext);

private:
	DreamOS *m_pDreamOS;

};

#endif // ! PHYSICS_ENGINE_TEST_SUITE_H_
