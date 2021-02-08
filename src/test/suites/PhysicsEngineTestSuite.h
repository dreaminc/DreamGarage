#ifndef PHYSICS_ENGINE_TEST_SUITE_H_
#define PHYSICS_ENGINE_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine Test Suite
// dos/src/test/suites/PhysicsEngineTestSutie.h

#include <functional>
#include <memory>

#include "DreamTestSuite.h"

#include "scene/ObjectStoreNode.h"

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
	RESULT ResetTest(void *pContext) override;

private:
	DreamOS *m_pDreamOS = nullptr;
	ObjectStoreNode *m_pSceneGraph = nullptr;

};

#endif // ! PHYSICS_ENGINE_TEST_SUITE_H_
