#ifndef COLLISION_TEST_SUITE_H_
#define COLLISION_TEST_SUITE_H_

#include "core/ehm/EHM.h"

// Dream Collision Test Suite
// DreamOS/PhysicsEngine/CollisionTestSuite.h

// The Collision test suite

#include "test/suites/DreamTestSuite.h"

#include <functional>
#include <memory>

#include "scene/ObjectStoreNode.h"

class DreamOS;

class CollisionTestSuite : public DreamTestSuite {
public:
	CollisionTestSuite(DreamOS *pDreamOS);
	~CollisionTestSuite() = default;

	virtual RESULT AddTests() override;

	virtual RESULT SetupPipeline(std::string strRenderShaderName = "standard") override;
	virtual RESULT SetupTestSuite() override;  

	RESULT AddTestRayModel();
	RESULT AddTestScaledCompositeRay();

	RESULT AddTestRayInComposite();

	// Plane
	RESULT AddTestPlanePlane();
	RESULT AddTestPlaneSphere();
	RESULT AddTestPlaneRay();
	RESULT AddTestPlaneQuad();
	RESULT AddTestPlaneOBB();

	// Sphere
	RESULT AddTestSphereSphere();
	RESULT AddTestSphereQuad();
	RESULT AddTestSphereOBB();
	
	// Quad
	RESULT AddTestQuadQuad();
	//RESULT AddTestQuadOBB();

	// OBB - OBB
	RESULT AddTestOBBOBB();

	// Hysteresis
	RESULT AddTestHysteresisObj();

	// TODO: OBB - Sphere

	// TODO: OBB - Quad
	
	// TODO: OBB - Ray

	// TODO: Sphere - Quad

	// TODO: Sphere - Ray

	// TODO: Quad - Ray

	// AABB 

private:
	RESULT ResetTest(void *pContext) override;

private:
	DreamOS * m_pDreamOS = nullptr;
	ObjectStoreNode *m_pSceneGraph = nullptr;
};

#endif // ! COLLISION_TEST_SUITE_H_

