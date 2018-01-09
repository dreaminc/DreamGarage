#ifndef COLLISION_TEST_SUITE_H_
#define COLLISION_TEST_SUITE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/PhysicsEngine/CollisionTestSuite.h
// The Collision test suite

#include "Test/TestSuite.h"

#include <functional>
#include <memory>

class DreamOS;

class CollisionTestSuite : public TestSuite {
public:
	CollisionTestSuite(DreamOS *pDreamOS);
	~CollisionTestSuite();

	virtual RESULT AddTests() override;

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
	
	// OBB
	//RESULT AddTestOBBOBB();


	// TODO: OBB - OBB

	// TODO: OBB - Sphere

	// TODO: OBB - Quad
	
	// TODO: OBB - Ray

	// TODO: Sphere - Quad

	// TODO: Sphere - Ray

	// TODO: Quad - Ray

	// AABB 

private:
	RESULT SetupSkyboxPipeline(std::string strRenderShaderName);
	RESULT ResetTest(void *pContext);

private:
	DreamOS *m_pDreamOS;

};

#endif // ! COLLISION_TEST_SUITE_H_
