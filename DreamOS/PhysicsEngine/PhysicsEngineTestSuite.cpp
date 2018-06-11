#include "PhysicsEngineTestSuite.h"
#include "DreamOS.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "PhysicsEngine/CollisionManifold.h"

PhysicsEngineTestSuite::PhysicsEngineTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	// empty
}

PhysicsEngineTestSuite::~PhysicsEngineTestSuite() {
	// empty
}

RESULT PhysicsEngineTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(SetupSkyboxPipeline("blinnphong"));

	CR(AddTestVolumeToPlaneVolumeDominos());

	CR(AddTestVolumeToPlaneVolume());

	CR(AddTestVolumeVolumePointFace());

	CR(AddTestVolumeVolumeEdge());

	CR(AddTestRayQuadsComposite());
	CR(AddTestRayQuads());

	CR(AddTestMultiCompositeRayScaledQuad());
	CR(AddTestRayScaledQuads());
	CR(AddTestBoundingScaleVolumes());
	CR(AddTestBoundingScaleSphereQuad());
	CR(AddTestBoundingScaleSpheres());
	CR(AddTestBoundingScaleSphereVolume());
	CR(AddTestBoundingScale());

	CR(AddTestRay());
	CR(AddTestCompositeCollisionSphereQuads());
	CR(AddTestCompositeCollisionVolumes());
	CR(AddTestMultiCompositeRayQuad());
	CR(AddTestCompositeComposition());
	CR(AddTestCompositeCompositionQuads());
	CR(AddTestCompositeRay());
	CR(AddTestSphereVsSphereArray());
	CR(AddTestCompositeCollisionVolumeSphere());
	CR(AddTestCompositeCollisionSphereVolume());
	CR(AddTestCompositeCollisionSpheres());
	CR(AddTestQuadVsSphere());
	CR(AddTestSphereGenerator());
	CR(AddTestSphereVsSphere());
	CR(AddTestBallVolume());

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBoundingScale() {
	RESULT r = R_PASS;

	double sTestTime = 25.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		m_pDreamOS->SetGravityState(false);

		// Ball to Volume
		auto pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);

		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		m_pDreamOS->AddPhysicsObject(pVolume);

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		pSphere->SetPosition(point(3.0f, 0.75f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->Scale(0.5f);
		//pSphere->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		m_pDreamOS->AddPhysicsObject(pSphere);

		auto pQuad = m_pDreamOS->AddQuad(0.5f, 0.5f);
		pQuad->SetPosition(point(0.0f, -1.0f, 0.0f));
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		m_pDreamOS->AddPhysicsObject(pQuad);

		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Bounding scale test");
	pNewTest->SetTestDescription("Bounding scale test to see that all bounding volume reference geometry is scaled correctly");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBoundingScaleSphereVolume() {
	RESULT r = R_PASS;

	double sTestTime = 25.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		m_pDreamOS->SetGravityState(false);

		// Ball to Volume
		auto pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);

		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(10.0f);
		pVolume->Scale(0.5f);
		m_pDreamOS->AddPhysicsObject(pVolume);

		auto pSphere1 = m_pDreamOS->AddSphere(0.25f, 10, 10);
		pSphere1->SetPosition(point(3.0f, 0.75f, 0.0f));
		pSphere1->SetMass(1.0f);
		pSphere1->Scale(0.5f);
		pSphere1->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		m_pDreamOS->AddPhysicsObject(pSphere1);

		auto pSphere2 = m_pDreamOS->AddSphere(0.25f, 10, 10);
		pSphere2->SetPosition(point(3.0f, -0.5f, 0.0f));
		pSphere2->SetMass(1.0f);
		pSphere2->Scale(0.5f);
		pSphere2->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		m_pDreamOS->AddPhysicsObject(pSphere2);

		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Scaled Sphere Volume");
	pNewTest->SetTestDescription("Collision of scaled sphere and volume");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBoundingScaleSpheres() {
	RESULT r = R_PASS;

	double sTestTime = 20.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		sphere *pSphere = nullptr;

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(1.0f, -0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.5f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(1.0f, 0.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(1.0f, -0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 1.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(0.0f, -1.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, -1.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(0.0f, 1.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(1.0f, 0.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(2.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		pSphere->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Scaled Sphere vs Sphere");
	pNewTest->SetTestDescription("Scaled spheres colliding with one another in a chain");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBoundingScaleSphereQuad() {
	RESULT r = R_PASS;

	double sTestTime = 10.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		m_pDreamOS->SetGravityState(true);

		// Quad vs Sphere
		double spacing = 1.75f;
		double angleFactor = 0.2f;

		// TODO: Add in pipeline (old test)

		{

			quad *pQuad1 = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(angleFactor, 1.0f, 0.0f));
			CN(pQuad1);
			pQuad1->SetPosition(point(-spacing, -1.0f, 0.0f));
			pQuad1->SetMass(1.0f);
			pQuad1->SetImmovable(true);
			pQuad1->Scale(0.5f);
			CR(m_pDreamOS->AddPhysicsObject(pQuad1));

			quad *pQuad2 = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(-angleFactor, 1.0f, 0.0f));
			CN(pQuad2);
			pQuad2->SetPosition(point(spacing, -1.0f, 0.0f));
			pQuad2->SetMass(1.0f);
			pQuad2->SetImmovable(true);
			pQuad2->Scale(0.5f);
			CR(m_pDreamOS->AddPhysicsObject(pQuad2));

			sphere *pSphere1 = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pSphere1);
			pSphere1->SetPosition(point(-spacing, 2.0f, 0.0f));
			pSphere1->SetMass(1.0f);
			//pSphere1->SetVelocity(0.0f, -1.0f, 0.0f);
			pSphere1->Scale(0.5f);
			CR(m_pDreamOS->AddPhysicsObject(pSphere1));
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Scaled Quad vs Sphere");
	pNewTest->SetTestDescription("Scaled Sphere colliding with quads");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBoundingScaleVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 6.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Volume vs Volume point - face

		auto pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		pVolume->SetVelocity(-1.0f / 2, 0.0f, 0.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-3.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		pVolume->Scale(0.5f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Scaled Volume vs Volume Pt Face");
	pNewTest->SetTestDescription("Scaled Volume colliding with volume pt to face");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestRayScaledQuads() {
	RESULT r = R_PASS;

	double sTestTime = 25.0f;
	int nRepeats = 1;
	const int numQuads = 4;

	struct RayTestContext {
		DimRay *pRay = nullptr;
		quad *pQuad[numQuads] = { nullptr };
		sphere *pCollidePoint[4] = { nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		double yPos = -1.0f;
		double xPos = 2.0f;

		// Ray to quads 
		int quadCount = 0;

		// Normal Quad

		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		pTestContext->pQuad[quadCount]->Scale(0.5f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		// Rotated by orientation
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		pTestContext->pQuad[quadCount]->Scale(0.5f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		pTestContext->pQuad[quadCount]->SetRotationalVelocity(vector(0.0f, 1.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		///*
		// Rotated by normal
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		pTestContext->pQuad[quadCount]->Scale(0.5f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		// Rotated by normal and orientation (should be flat)
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(-1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		pTestContext->pQuad[quadCount]->Scale(0.5f);
		pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;
		//*/

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);

		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for quad collisions using the ray
		for (int i = 0; i < numQuads; i++) {
			if (pTestContext->pRay->Intersect(pTestContext->pQuad[i])) {
				CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pQuad[i]);

				if (manifold.NumContacts() > 0) {
					for (int i = 0; i < manifold.NumContacts(); i++) {
						pTestContext->pCollidePoint[i]->SetVisible(true);
						pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Scaled Quads");
	pNewTest->SetTestDescription("Ray intersection of scaled quads oriented in various fashion");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestMultiCompositeRayScaledQuad() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		std::shared_ptr<composite> pCompositeChild = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		composite *pComposite = nullptr;

		double yPos = -1.0f;

		// Ray to composite

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);

		pComposite = pTestContext->pComposite;
		CN(pComposite);

		// Test the various bounding types
		switch (nRepeatCounter) {
			case 0: pComposite->InitializeOBB(); break;
			case 1: pComposite->InitializeAABB(); break;
			case 2: pComposite->InitializeBoundingSphere(); break;
		}
		pComposite->SetMass(1.0f);

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));

		pCompositeChild = pComposite->AddComposite();
		CN(pCompositeChild);
		pCompositeChild->InitializeOBB();
		pCompositeChild->SetMass(1.0f);
		pCompositeChild->SetPosition(point(1.0f, 0.0f, 0.0f));

		pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		pQuad->SetPosition(point(-0.5f, 0.0f, 0.0f));

		pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		pQuad->SetPosition(point(0.5f, 0.0f, 0.0f));

		pCompositeChild = pComposite->AddComposite();
		CN(pCompositeChild);
		pCompositeChild->InitializeOBB();
		pCompositeChild->SetMass(1.0f);
		pCompositeChild->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		pQuad->SetPosition(point(-0.5f, 0.0f, 0.0f));

		pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->Scale(0.5f);
		pQuad->SetPosition(point(0.5f, 0.0f, 0.0f));

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pComposite->SetPosition(point(0.0f, yPos, 0.0f));
		pComposite->RotateZByDeg(45.0f);

		// Add physics composite
		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		// The Ray
		///*
		pTestContext->pRay = m_pDreamOS->AddRay(point(-4.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);

		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

		nRepeatCounter++;

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		// Check for composite collisions using the ray
		{
			CollisionManifold manifold = pTestContext->pComposite->Collide(pTestContext->pRay->GetRay());
			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}


	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Nested Composite Scaled Quads");
	pNewTest->SetTestDescription("Ray intersection of multiple layers of nested scaled quads in a composite and resolving those points, also returning the object");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestBallVolume() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext){
		m_pDreamOS->SetGravityState(false);

		// Ball to Volume
		volume *pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);

		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(10.0f);
		//pVolume->SetRotationalVelocity(vector(0.0f, 0.0f, 5.6f));
		//pVolume->RotateZByDeg(-135.0f);
		//pVolume->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		//pVolume->ApplyTorqueImpulse(vector(0.0f, 0.0f, 1.0f));
		//pVolume->ApplyTorqueImpulse(vector(0.0f, 0.1f, 0.0f));
		//pVolume->ApplyForceAtPoint(vector(-10.0f, 0.0f, 10.0f), point(0.5f, 1.5f, 0.5f), 0.02f);
		m_pDreamOS->AddPhysicsObject(pVolume);

		auto pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		pSphere->SetPosition(point(3.0f, 0.75f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		m_pDreamOS->AddPhysicsObject(pSphere);

		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs OBB");
	pNewTest->SetTestDescription("Sphere colliding with an OBB with various orientations");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: This is really a collision test
// Should create a collision / intersection test suite
RESULT PhysicsEngineTestSuite::AddTestRayQuadsComposite() {
	RESULT r = R_PASS;

	double sTestTime = 25.0f;
	int nRepeats = 1;
	const int numQuads = 4;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		std::shared_ptr<quad> pQuad[numQuads] = { nullptr };
		sphere *pCollidePoint[4] = { nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		double yPos = -1.0f;
		double xPos = 2.0f;

		// Ray to quads 
		int quadCount = 0;

		CR(SetupSkyboxPipeline("blinnphong"));

		RayTestContext *pTestContext;
		pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		pTestContext->pComposite = m_pDreamOS->AddComposite();
		CN(pTestContext->pComposite);
		CR(pTestContext->pComposite->InitializeOBB());

		// Normal Quad

		pTestContext->pQuad[quadCount] = pTestContext->pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetVertexColor(color(COLOR_GREEN));
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		xPos -= 1.0f;
		quadCount++;

		// Rotated by orientation
		pTestContext->pQuad[quadCount] = pTestContext->pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetVertexColor(color(COLOR_GREEN));
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		pTestContext->pQuad[quadCount]->SetRotationalVelocity(vector(0.0f, 1.0f, 0.0f));
		xPos -= 1.0f;

		///*
		// Rotated by normal
		pTestContext->pQuad[quadCount] = pTestContext->pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetVertexColor(color(COLOR_GREEN));
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		xPos -= 1.0f;

		// Rotated by normal and orientation (should be flat)
		pTestContext->pQuad[quadCount] = pTestContext->pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(-1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		pTestContext->pQuad[quadCount]->SetVertexColor(color(COLOR_GREEN));
		xPos -= 1.0f;
		//*/

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetMaterialDiffuseColor(color(COLOR_BLUE));
			pTestContext->pCollidePoint[i]->SetVertexColor(color(COLOR_BLUE));
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, -0.5f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);

		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		rCast = pTestContext->pRay->GetRay();

		// Check for quad collisions using the ray
		for (int i = 0; i < numQuads; i++) {
			if (pTestContext->pComposite->Intersect(rCast)) {
				CollisionManifold manifold = pTestContext->pComposite->Collide(rCast);

				if (manifold.NumContacts() > 0) {
					for (int i = 0; i < manifold.NumContacts(); i++) {
						pTestContext->pCollidePoint[i]->SetVisible(true);
						pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Quads in Composite");
	pNewTest->SetTestDescription("Ray intersection of quads oriented in various fashion in a composite");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestRayQuads() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	const int numQuads = 4;

	struct RayTestContext {
		DimRay *pRay = nullptr;
		quad *pQuad[numQuads] = { nullptr };
		sphere *pCollidePoint[4] = { nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		double yPos = -1.0f;
		double xPos = 2.0f;

		// Ray to quads 
		int quadCount = 0;

		// Normal Quad

		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		// Rotated by orientation
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		pTestContext->pQuad[quadCount]->SetRotationalVelocity(vector(0.0f, 1.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		///*
		// Rotated by normal
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		//pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;

		// Rotated by normal and orientation (should be flat)
		pTestContext->pQuad[quadCount] = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(-1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad[quadCount]);
		pTestContext->pQuad[quadCount]->SetPosition(point(xPos, yPos, 0.0f));
		pTestContext->pQuad[quadCount]->SetMass(1.0f);
		pTestContext->pQuad[quadCount]->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad[quadCount++]));
		xPos -= 1.0f;
		//*/

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);

		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for quad collisions using the ray
		for (int i = 0; i < numQuads; i++) {
			if (pTestContext->pRay->Intersect(pTestContext->pQuad[i])) {
				CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pQuad[i]);

				if (manifold.NumContacts() > 0) {
					for (int i = 0; i < manifold.NumContacts(); i++) {
						pTestContext->pCollidePoint[i]->SetVisible(true);
						pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Quads");
	pNewTest->SetTestDescription("Ray intersection of quads oriented in various fashion");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestRay() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	struct RayTestContext {
		DimRay *pRay = nullptr;
		volume *pVolume = nullptr;
		sphere *pSphere = nullptr;
		quad *pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		double yPos = -1.0f;

		// Ray to objects 

		pTestContext->pVolume = m_pDreamOS->AddVolume(0.5);
		CN(pTestContext->pVolume);
		pTestContext->pVolume->SetPosition(point(-1.0f, yPos, 0.0f));
		pTestContext->pVolume->SetMass(10.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pVolume));

		pTestContext->pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pTestContext->pSphere);
		pTestContext->pSphere->SetPosition(point(1.0f, yPos, 0.0f));
		pTestContext->pSphere->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pSphere));

		pTestContext->pQuad = m_pDreamOS->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(1.0f, 1.0f, 0.0f));
		CN(pTestContext->pQuad);
		pTestContext->pQuad->SetPosition(point(0.0f, yPos, 0.0f));
		pTestContext->pQuad->SetMass(1.0f);
		//pTestContext->pQuad->RotateZByDeg(45.0f);
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pQuad));

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
		CN(pTestContext->pRay);
		
		///*
		pTestContext->pRay->SetMass(1.0f);
		pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
		//*/

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pRay);
		CN(pTestContext->pVolume);
		CN(pTestContext->pSphere);
		CN(pTestContext->pQuad);

		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for object collisions using the ray
		if (pTestContext->pRay->Intersect(pTestContext->pQuad)) {
			CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pQuad);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}

		if (pTestContext->pRay->Intersect(pTestContext->pVolume)) {
			CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pVolume);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}

		if (pTestContext->pRay->Intersect(pTestContext->pSphere)) {
			CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pSphere);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}		

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Objects");
	pNewTest->SetTestDescription("Ray intersection of various objects and resolving those points");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestVolumeVolumePointFace() {
	RESULT r = R_PASS;

	double sTestTime = 60.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Volume vs Volume point - face

		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		pVolume->SetVelocity(-1.0f/2, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		//pVolume->SetVelocity(-1.0f / 2, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		///*
		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(-3.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));
		//*/
	
	Error:
		return R_PASS;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Volume vs Volume Pt Face");
	pNewTest->SetTestDescription("Volume colliding with volume pt to face");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestVolumeToPlaneVolumeDominos() {
	RESULT r = R_PASS;

	double sTestTime = 215.0f;
	int nRepeats = 1;


	class TestContext {
	public:
		sphere * pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
		DimRay *pCollidePointRay[4] = { nullptr, nullptr, nullptr, nullptr };


	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(true);

		volume *pVolume = nullptr;

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);

			pTestContext->pCollidePointRay[i] = m_pDreamOS->MakeRay(point(), vector::jVector(-1.0f), 1.0f);
			CN(pTestContext->pCollidePointRay[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePointRay[i]);
			pTestContext->pCollidePointRay[i]->SetVisible(false);
		}

		// Volume to "plane"
		//pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);
		pVolume = m_pDreamOS->AddVolume(10.0, 10.0, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, -3.0f, 0.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-4.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->AddAngularMomentum(vector(0.0f, 0.0f, -0.1f));
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-3.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-3.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-2.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-2.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-1.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-1.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(-0.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(1.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(1.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(2.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(2.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(3.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(3.5f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.125, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(4.0f, -1.99f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Volume vs Volume Plane");
	pNewTest->SetTestDescription("Volume colliding with immovable volume");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestVolumeToPlaneVolume() {
	RESULT r = R_PASS;

	double sTestTime = 215.0f;
	int nRepeats = 1;

	
	class TestContext {
	public:
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
		DimRay *pCollidePointRay[4] = { nullptr, nullptr, nullptr, nullptr };
		

	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [=](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(true);

		volume *pVolume = nullptr;

		// Test Context
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		CN(pTestContext);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i] = m_pDreamOS->MakeSphere(0.025f, 10, 10);
			CN(pTestContext->pCollidePoint[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePoint[i]);
			pTestContext->pCollidePoint[i]->SetVisible(false);

			pTestContext->pCollidePointRay[i] = m_pDreamOS->MakeRay(point(), vector::jVector(-1.0f), 1.0f);
			CN(pTestContext->pCollidePointRay[i]);
			m_pSceneGraph->PushObject(pTestContext->pCollidePointRay[i]);
			pTestContext->pCollidePointRay[i]->SetVisible(false);
		}

		// Volume to "plane"
		//pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);
		pVolume = m_pDreamOS->AddVolume(5.0, 5.0, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, -3.0f, 0.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(0.5f, 1.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->RotateZByDeg(25.0f);
		//pVolume->RotateZByDeg(-15.01f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		//pQuad = AddQuad(10.0f, 10.0f, 1, 1, nullptr, vector::jVector(1.0f));
		//pQuad->SetPosition(point(0.0f, -3.0f, 0.0f));
		//pQuad->SetMass(1.0f);
		//pQuad->SetImmovable(true);
		//AddPhysicsObject(pQuad);
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Volume vs Volume Plane");
	pNewTest->SetTestDescription("Volume colliding with immovable volume");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestSphereVsSphereArray() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Sphere vs Sphere
		float radius = 0.2f;
		float padding = 0.1f;
		int num = 6;

		for (int i = 0; i < num; i++) {
			float startY = (((radius * 2.0) + padding) * (num));
			startY /= -2.0f;

			for (int j = 0; j < num; j++) {
				sphere *pSphereTemp = m_pDreamOS->AddSphere(radius, 10, 10);
				CN(pSphereTemp);

				pSphereTemp->SetPosition(point(((2.0f * radius) + padding) * i, startY + (((radius * 2.0) + padding) * j), 0.0f));
				pSphereTemp->SetMass(1.0f);
				CR(m_pDreamOS->AddPhysicsObject(pSphereTemp));
			}
		}

		sphere *pSphere1;
		pSphere1 = m_pDreamOS->AddSphere(2.0f, 10, 10);
		CN(pSphere1);
		pSphere1->SetPosition(point(-4.0f, 0.0f, 0.0f));
		pSphere1->SetMass(10.0f);
		pSphere1->SetVelocity(1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere1));

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Sphere Array");
	pNewTest->SetTestDescription("Larger sphere colliding with an array of spheres of lesser mass");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestSphereVsSphere() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		sphere *pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(1.0f, -0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.5f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);		
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));
		
		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(1.0f, -0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, 1.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(0.0f, -1.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(0.0f, -1.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(0.0f, 1.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

		pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetPosition(point(2.0f, 0.0f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(-1.0f, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pSphere));

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Sphere");
	pNewTest->SetTestDescription("Spheres colliding with one another in a chain");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestSphereGenerator() {
	RESULT r = R_PASS;

	double sTestTime = 25.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		m_pDreamOS->SetGravityState(true);

		// TODO: Add in pipeline (old test)

		float width = 5.0f;
		float wallThickness = 0.25f;
		float wallHeight = 1.0f;
		float posY = -3.0f;

		{
			auto pVolume = m_pDreamOS->AddVolume(width, width, 1.0f);
			CN(pVolume);
			pVolume->SetPosition(point(0.0f, posY, 0.0f));
			pVolume->SetMass(100000.0f);
			pVolume->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pVolume));

			// left wall
			pVolume = m_pDreamOS->AddVolume(width, wallThickness, wallHeight);
			CN(pVolume);
			pVolume->SetPosition(point(-width / 2.0f + wallThickness / 2.0f, posY + wallHeight + DREAM_EPSILON, 0.0f));
			pVolume->SetMass(100000.0f);
			pVolume->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pVolume));

			// right wall
			pVolume = m_pDreamOS->AddVolume(width, wallThickness, wallHeight);
			CN(pVolume);
			pVolume->SetPosition(point(width / 2.0f - wallThickness / 2.0f, posY + wallHeight + DREAM_EPSILON, 0.0f));
			pVolume->SetMass(100000.0f);
			pVolume->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pVolume));

			// front wall
			pVolume = m_pDreamOS->AddVolume(wallThickness, width - wallThickness * 2.1f, wallHeight);
			CN(pVolume);
			pVolume->SetPosition(point(0.0f, posY + wallHeight + DREAM_EPSILON, width / 2.0f - wallThickness / 2.0f));
			pVolume->SetMass(100000.0f);
			pVolume->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pVolume));

			// back wall
			pVolume = m_pDreamOS->AddVolume(wallThickness, width - wallThickness * 2.1f, wallHeight);
			CN(pVolume);
			pVolume->SetPosition(point(0.0f, posY + wallHeight + DREAM_EPSILON, -width / 2.0f + wallThickness / 2.0f));
			pVolume->SetMass(100000.0f);
			pVolume->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pVolume));

			sphere *pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pSphere);
			pSphere->SetPosition(point(0.0f, 1.0f, 0.0f));
			pSphere->SetMass(1.0f);
			CR(m_pDreamOS->AddPhysicsObject(pSphere));
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		static int counter = 0;
		static int j = 1;
		static int k = 1;

		if (counter >= 100) {
			sphere *pSphere = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pSphere);
			pSphere->SetPosition(point(0.0f + (j) * DREAM_EPSILON * std::pow(-1.0f, j), 1.0f, 0.0f + (k) * DREAM_EPSILON * std::pow(-1.0f, k)));
			pSphere->SetMass(1.0f);
			CR(m_pDreamOS->AddPhysicsObject(pSphere));
			
			counter = 0;
			j++;
			if (j % 2)
				k++;
		}
		else {
			counter++;
		}

	Error:
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Sphere vs Sphere");
	pNewTest->SetTestDescription("Spheres colliding with one another in a chain");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestQuadVsSphere() {
	RESULT r = R_PASS;

	double sTestTime = 10.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		
		m_pDreamOS->SetGravityState(true);

		// TODO: Add in the pipeline

		// Quad vs Sphere
		double spacing = 1.25f;
		double angleFactor = 0.2f;

		{

			quad *pQuad1 = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(angleFactor, 1.0f, 0.0f));
			CN(pQuad1);
			pQuad1->SetPosition(point(-spacing, -1.0f, 0.0f));
			pQuad1->SetMass(1.0f);
			pQuad1->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pQuad1));

			quad *pQuad2 = m_pDreamOS->AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(-angleFactor, 1.0f, 0.0f));
			CN(pQuad2);
			pQuad2->SetPosition(point(spacing, -1.0f, 0.0f));
			pQuad2->SetMass(1.0f);
			pQuad2->SetImmovable(true);
			CR(m_pDreamOS->AddPhysicsObject(pQuad2));

			sphere *pSphere1 = m_pDreamOS->AddSphere(0.25f, 10, 10);
			CN(pSphere1);
			pSphere1->SetPosition(point(-spacing, 2.0f, 0.0f));
			pSphere1->SetMass(1.0f);
			//pSphere1->SetVelocity(0.0f, -1.0f, 0.0f);
			CR(m_pDreamOS->AddPhysicsObject(pSphere1));
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Quad vs Sphere");
	pNewTest->SetTestDescription("Sphere colliding with quads");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

// TODO: There are clearly issues with this test
RESULT PhysicsEngineTestSuite::AddTestVolumeVolumeEdge() {
	RESULT r = R_PASS;

	double sTestTime = 60.0f;
	int nRepeats = 5;
	static int nRepeatCounter = 5;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pDreamOS->SetGravityState(false);

		// Volume vs Volume edge edge

		CR(SetupSkyboxPipeline("blinnphong"));

		volume *pVolume;
		pVolume = nullptr;

		switch (nRepeatCounter) {
			case 0: {
				// case 1
				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
				pVolume->RotateZByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateYByDeg(45.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);
			} break;

			case 1: {
				// case 1 - reversed
				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateYByDeg(45.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
				pVolume->RotateZByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);
			} break;

			case 2: {
				// case 2
				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateZByDeg(45.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);
			} break;

			case 3: {
				// case 2 - reversed 
				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateZByDeg(45.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

				pVolume = m_pDreamOS->AddVolume(1.0f);
				pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

			} break;

			// TODO: Edge to parallel edge

			case 4: {
				// case 3
				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateZByDeg(45.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateYByDeg(45.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateZByDeg(45.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);
				pVolume->SetMass(1.0f);
				m_pDreamOS->AddPhysicsObject(pVolume);

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				pVolume->RotateZByDeg(45.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(-3.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);				
				pVolume->SetMass(1.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));
			} break;

			case 5: {
				// case 3
				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(4.0f, 0.0f, 0.0f));
				pVolume->RotateYByDeg(45.0f);
				pVolume->SetMass(1.0f);
				pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				pVolume = m_pDreamOS->AddVolume(0.5f);
				CN(pVolume);
				pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
				pVolume->SetMass(1.0f);
				//pVolume->RotateZByDeg(45.0f);
				CR(m_pDreamOS->AddPhysicsObject(pVolume));

				//pVolume = m_pDreamOS->AddVolume(0.5f);
				//CN(pVolume);
				//pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));
				//pVolume->SetMass(1.0f);
				//pVolume->RotateYByDeg(45.0f);
				//CR(m_pDreamOS->AddPhysicsObject(pVolume));
				//
				//pVolume = m_pDreamOS->AddVolume(0.5f);
				//CN(pVolume);
				//pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
				//pVolume->SetMass(1.0f);
				//pVolume->RotateZByDeg(45.0f);
				//CR(m_pDreamOS->AddPhysicsObject(pVolume));
				//
				//pVolume = m_pDreamOS->AddVolume(0.5f);
				//CN(pVolume);
				//pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
				//pVolume->RotateYByDeg(45.0f);
				//pVolume->SetMass(1.0f);
				//m_pDreamOS->AddPhysicsObject(pVolume);
				//
				//pVolume = m_pDreamOS->AddVolume(0.5f);
				//CN(pVolume);
				//pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
				//pVolume->SetMass(1.0f);
				//pVolume->RotateZByDeg(45.0f);
				//CR(m_pDreamOS->AddPhysicsObject(pVolume));
				//
				//pVolume = m_pDreamOS->AddVolume(0.5f);
				//CN(pVolume);
				//pVolume->SetPosition(point(-3.0f, 0.0f, 0.0f));
				//pVolume->RotateYByDeg(45.0f);
				//pVolume->SetMass(1.0f);
				//CR(m_pDreamOS->AddPhysicsObject(pVolume));
			} break;
		}

		nRepeatCounter++;

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Volume vs Volume - Edges");
	pNewTest->SetTestDescription("Testing edge - edge for volume collisions");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeCompositionQuads() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 3;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<quad> pQuad = nullptr;
		composite *pComposite = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}
		
		pComposite->SetMass(1.0f);

		pQuad = pComposite->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(1.0f, 0.0f, 0.0f));

		/*
		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(0.0f, -1.0f, 0.0f));

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector::kVector(1.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(0.0f, 1.0f, 0.0f));
		*/


		pComposite->SetVelocity(point(0.2f, 0.0f, 0.0f));
		pComposite->SetRotationalVelocity(vector(0.0f, 0.0f, 0.25f));

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Composition");
	pNewTest->SetTestDescription("Testing composite composition along with internal rotations and active external transformations");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestMultiCompositeRayQuad() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		std::shared_ptr<composite> pCompositeChild = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;

		double yPos = -1.0f;

		// Ray to composite
		// TODO: Do we need this, this is more of a collision test
		// Also add in pipeline

		{

			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);

			composite *pComposite = pTestContext->pComposite;
			CN(pComposite);

			// Test the various bounding types
			switch (nRepeatCounter) {
			case 0: pComposite->InitializeOBB(); break;
			case 1: pComposite->InitializeAABB(); break;
			case 2: pComposite->InitializeBoundingSphere(); break;
			}
			pComposite->SetMass(1.0f);

			pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pQuad);
			pQuad->SetMass(1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));

			pCompositeChild = pComposite->AddComposite();
			CN(pCompositeChild);
			pCompositeChild->InitializeOBB();
			pCompositeChild->SetMass(1.0f);
			pCompositeChild->SetPosition(point(1.0f, 0.0f, 0.0f));

			pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pQuad);
			pQuad->SetMass(1.0f);
			pQuad->SetPosition(point(-0.5f, 0.0f, 0.0f));

			pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pQuad);
			pQuad->SetMass(1.0f);
			pQuad->SetPosition(point(0.5f, 0.0f, 0.0f));

			pCompositeChild = pComposite->AddComposite();
			CN(pCompositeChild);
			pCompositeChild->InitializeOBB();
			pCompositeChild->SetMass(1.0f);
			pCompositeChild->SetPosition(point(-1.0f, 0.0f, 0.0f));

			pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pQuad);
			pQuad->SetMass(1.0f);
			pQuad->SetPosition(point(-0.5f, 0.0f, 0.0f));

			pQuad = pCompositeChild->AddQuad(0.25f, 0.25f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pQuad);
			pQuad->SetMass(1.0f);
			pQuad->SetPosition(point(0.5f, 0.0f, 0.0f));

			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}

			pComposite->SetPosition(point(0.0f, yPos, 0.0f));
			pComposite->RotateZByDeg(45.0f);

			// Add physics composite
			CR(m_pDreamOS->AddPhysicsObject(pComposite));

			// The Ray
			///*
			pTestContext->pRay = m_pDreamOS->AddRay(point(-4.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay);

			///*
			pTestContext->pRay->SetMass(1.0f);
			pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
			CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
			//*/

			nRepeatCounter++;

		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		// Check for composite collisions using the ray
		{
			CollisionManifold manifold = pTestContext->pComposite->Collide(pTestContext->pRay->GetRay());
			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}
		

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Nested Composite Quads");
	pNewTest->SetTestDescription("Ray intersection of multiple layers of nested quads in a composite and resolving those points, also returning the object");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeRay() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	struct RayTestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		std::shared_ptr<volume> pVolume = nullptr;
		std::shared_ptr<sphere> pSphere = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	RayTestContext *pTestContext = new RayTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		double yPos = -1.0f;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);
		CN(pTestContext);

		// Ray to composite
		// TODO: Add in pipeline, also potentially remove this test
		// considering that it's a collision test

		{

			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);

			composite *pComposite = pTestContext->pComposite;
			CN(pComposite);

			// Test the various bounding types
			switch (nRepeatCounter) {
			case 0: pComposite->InitializeOBB(); break;
			case 1: pComposite->InitializeAABB(); break;
			case 2: pComposite->InitializeBoundingSphere(); break;
			}
			pComposite->SetMass(1.0f);

			pTestContext->pVolume = pComposite->AddVolume(0.5);
			CN(pTestContext->pVolume);
			pTestContext->pVolume->SetMass(1.0f);
			pTestContext->pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));

			pTestContext->pSphere = pComposite->AddSphere(0.25f, 10, 10);
			CN(pTestContext->pSphere);
			pTestContext->pSphere->SetMass(1.0f);
			pTestContext->pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));

			pTestContext->pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
			CN(pTestContext->pQuad);
			pTestContext->pQuad->SetMass(1.0f);
			pTestContext->pQuad->SetPosition(point(-1.0f, 0.0f, 0.0f));

			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}

			pComposite->SetPosition(point(0.0f, yPos, 0.0f));

			// Add physics composite
			CR(m_pDreamOS->AddPhysicsObject(pComposite));

			// The Ray
			///*
			pTestContext->pRay = m_pDreamOS->AddRay(point(-3.0f, 2.0f, 0.0f), vector(0.5f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay);

			///*
			pTestContext->pRay->SetMass(1.0f);
			pTestContext->pRay->SetVelocity(vector(0.4f, 0.0f, 0.0f));
			CR(m_pDreamOS->AddPhysicsObject(pTestContext->pRay));
			//*/

			nRepeatCounter++;
		}

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);
		CN(pTestContext->pVolume);
		CN(pTestContext->pSphere);
		CN(pTestContext->pQuad);

		
		for (int i = 0; i < 4; i++)
			pTestContext->pCollidePoint[i]->SetVisible(false);

		// Check for composite collisions using the ray

		//if (pTestContext->pRay->Intersect(pTestContext->pComposite)) {
		if (pTestContext->pComposite->Intersect(pTestContext->pRay->GetRay())) {
			//CollisionManifold manifold = pTestContext->pRay->Collide(pTestContext->pComposite);
			CollisionManifold manifold = pTestContext->pComposite->Collide(pTestContext->pRay->GetRay());

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RayTestContext *pTestContext = reinterpret_cast<RayTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray vs Composite Objects");
	pNewTest->SetTestDescription("Ray intersection of various objects in a composite and resolving those points");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeComposition() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 3;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<sphere> pSphere = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pCompositeChild = nullptr;
		volume *pVolume = nullptr;

		pVolume = m_pDreamOS->AddVolume(5.0, 5.0, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, -3.0f, 0.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);
		m_pDreamOS->AddPhysicsObject(pVolume);

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
			case 0: pComposite->InitializeOBB(); break;
			case 1: pComposite->InitializeAABB(); break;
			case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pSphere = pComposite->AddSphere(0.25f, 10, 10);		
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.2f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);

		pCompositeChild = pComposite->AddComposite();
		CN(pCompositeChild);
		pCompositeChild->InitializeOBB();
		pCompositeChild->SetMass(1.0f);

		pSphere = pCompositeChild->AddSphere(0.1f, 10, 10);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(-0.5f, 0.0f, 0.0f));

		pSphere = pCompositeChild->AddSphere(0.1f, 10, 10);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(0.5f, 0.0f, 0.0f));
		pCompositeChild->SetPosition(point(1.0f, 0.0f, 0.0f));
		pCompositeChild->SetRotationalVelocity(vector(0.0f, 0.0f, 0.25f));

		pComposite->SetVelocity(point(0.2f, 0.0f, 0.0f));
		pComposite->SetRotationalVelocity(vector(0.0f, 0.0f, 0.25f));

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Composition");
	pNewTest->SetTestDescription("Testing composite composition along with internal rotations and active external transformations");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT PhysicsEngineTestSuite::AddTestCompositeCollisionSpheres() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<sphere> pSphere = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pCompositeChild = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
			case 0: pComposite->InitializeOBB(); break;
			case 1: pComposite->InitializeAABB(); break;
			case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(0.0f, 1.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(0.0f, -1.0f, 0.0f));

		pComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pComposite->RotateZByDeg(45.0f);

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		sphere *pSphereCollide;
		pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphereCollide);
		pSphereCollide->SetMass(1.0f);
		pSphereCollide->SetPosition(point(3.0f, 1.0f, 0.0f));
		pSphereCollide->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pSphereCollide));

		nRepeatCounter++;

	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Sphere Collision");
	pNewTest->SetTestDescription("Testing composite collisions with spheres");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeCollisionSphereVolume() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<volume> pVolume = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pCompositeChild = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(0.0f, 1.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(0.0f, -1.0f, 0.0f));

		pComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pComposite->RotateZByDeg(45.0f);

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		sphere *pSphereCollide;
		pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphereCollide);
		pSphereCollide->SetMass(1.0f);
		pSphereCollide->SetPosition(point(3.0f, 1.25f, 0.0f));
		pSphereCollide->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pSphereCollide));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Sphere Volume");
	pNewTest->SetTestDescription("Testing composite collisions with spheres and volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeCollisionVolumeSphere() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<sphere> pSphere = nullptr;
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pCompositeChild = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(1.0f, 0.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(0.0f, 1.0f, 0.0f));

		pSphere = pComposite->AddSphere(0.25f, 10, 10);
		CN(pSphere);
		pSphere->SetMass(1.0f);
		pSphere->SetPosition(point(0.0f, -1.0f, 0.0f));

		pComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pComposite->RotateZByDeg(45.0f);

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		volume *pVolumeCollide;
		pVolumeCollide = m_pDreamOS->AddVolume(0.5f);
		CN(pVolumeCollide);
		pVolumeCollide->SetMass(1.0f);
		pVolumeCollide->SetPosition(point(3.0f, 1.0f, 0.0f));
		pVolumeCollide->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pVolumeCollide));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Sphere Volume");
	pNewTest->SetTestDescription("Testing composite collisions with spheres and volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::AddTestCompositeCollisionVolumes() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<volume> pVolume = nullptr;
		composite *pComposite = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(0.0f, 1.0f, 0.0f));

		pVolume = pComposite->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetMass(1.0f);
		pVolume->SetPosition(point(0.0f, -1.0f, 0.0f));

		pComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pComposite->RotateZByDeg(45.0f);

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		volume *pVolumeCollide;
		pVolumeCollide = m_pDreamOS->AddVolume(0.5f);
		CN(pVolumeCollide);
		pVolumeCollide->SetMass(1.0f);
		//pVolumeCollide->RotateYByDeg(45.0f);
		//pVolumeCollide->RotateZByDeg(45.0f);
		pVolumeCollide->SetPosition(point(3.0f, 0.5f, 0.2f));
		pVolumeCollide->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pVolumeCollide));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Sphere Volume");
	pNewTest->SetTestDescription("Testing composite collisions with spheres and volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT PhysicsEngineTestSuite::AddTestCompositeCollisionSphereQuads() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Composite vs plane

		std::shared_ptr<quad> pQuad = nullptr;
		composite *pComposite = nullptr;

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		// Test the various types
		switch (nRepeatCounter) {
		case 0: pComposite->InitializeOBB(); break;
		case 1: pComposite->InitializeAABB(); break;
		case 2: pComposite->InitializeBoundingSphere(); break;
		}

		pComposite->SetMass(1.0f);

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(-1.0f, 0.0f, 0.0f));

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(1.0f, 0.0f, 0.0f));

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(0.0f, 0.0f, 1.0f));

		pQuad = pComposite->AddQuad(0.5f, 0.5f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
		CN(pQuad);
		pQuad->SetMass(1.0f);
		pQuad->SetPosition(point(0.0f, 0.0f, -1.0f));

		pComposite->SetPosition(point(0.0f, -1.0f, 0.0f));
		pComposite->RotateYByDeg(45.0f);

		CR(m_pDreamOS->AddPhysicsObject(pComposite));

		sphere *pSphereCollide;
		pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
		CN(pSphereCollide);
		pSphereCollide->SetMass(1.0f);
		pSphereCollide->SetPosition(point(0.707f, 1.0f, 0.707f));
		pSphereCollide->SetVelocity(vector(0.0f, -1.0f, 0.0f));
		CR(m_pDreamOS->AddPhysicsObject(pSphereCollide));

		nRepeatCounter++;
	Error:
		return r;
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		return ResetTest(pContext);
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
	CN(pNewTest);

	pNewTest->SetTestName("Composite Sphere Volume");
	pNewTest->SetTestDescription("Testing composite collisions with spheres and volumes");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}


RESULT PhysicsEngineTestSuite::SetupSkyboxPipeline(std::string strRenderShaderName) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	{
		m_pSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
		CNM(m_pSceneGraph, "Failed to allocate Debug Scene Graph");

		CR(pHAL->MakeCurrentContext());

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode(strRenderShaderName);
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(pSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		// Debug Overlay
		ProgramNode* pDebugOverlay = pHAL->MakeProgramNode("debug_overlay");
		CN(pDebugOverlay);
		CR(pDebugOverlay->ConnectToInput("scenegraph", m_pSceneGraph->Output("objectstore")));
		CR(pDebugOverlay->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		CR(pDebugOverlay->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenQuad);
		CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDebugOverlay->Output("output_framebuffer")));
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

		CR(pHAL->ReleaseCurrentContext());

		//light *pLight = m_pDreamOS->AddLight(LIGHT_DIRECTIONAL, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
		light *pLight = m_pDreamOS->AddLight(LIGHT_SPOT, 1.0f, point(0.0f, 5.0f, 3.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.2f, -1.0f, 0.5f));
	}

Error:
	return r;
}