#ifndef PHYSICS_ENGINE_TEST_SUITE_H_
#define PHYSICS_ENGINE_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/PhysicsEngine/PhysicsEngineTestSutie.h

#include "Test/DreamTestSuite.h"

#include <functional>
#include <memory>

#include "Scene/ObjectStoreNode.h"

class DreamOS;

class PhysicsEngineTestSuite : public DreamTestSuite {
public:
	PhysicsEngineTestSuite(DreamOS *pDreamOS);
	~PhysicsEngineTestSuite();

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderShaderName = "standard") override;
	virtual RESULT SetupTestSuite() override;

	RESULT AddTestCompositeRay();
	RESULT AddTestMultiCompositeRayQuad();
	RESULT AddTestRay();
	RESULT AddTestRayQuads();
	RESULT AddTestRayQuadsComposite();
	RESULT AddTestBallVolume();
	RESULT AddTestVolumeVolumePointFace();
	RESULT AddTestVolumeToPlaneVolumeDominos();
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
	RESULT ResetTest(void *pContext);

private:
	DreamOS *m_pDreamOS = nullptr;
	ObjectStoreNode *m_pSceneGraph = nullptr;

};

#endif // ! PHYSICS_ENGINE_TEST_SUITE_H_
