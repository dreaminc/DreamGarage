#include "PhysicsEngineTestSuite.h"
#include "DreamOS.h"

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

	CR(AddTestCompositeCollisionSphereQuads());
	CR(AddTestCompositeCompositionQuads());
	CR(AddTestCompositeCollisionVolumes());
	CR(AddTestVolumeVolumePointFace());
	CR(AddTestVolumeVolumeEdge());
	CR(AddTestCompositeCollisionVolumeSphere());
	CR(AddTestCompositeCollisionSphereVolume());
	CR(AddTestCompositeCollisionSpheres());
	CR(AddTestCompositeComposition());
	CR(AddTestQuadVsSphere());
	CR(AddTestSphereGenerator());
	CR(AddTestSphereVsSphere());
	CR(AddTestSphereVsSphereArray());
	CR(AddTestVolumeToPlaneVolume());
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

RESULT PhysicsEngineTestSuite::AddTestBallVolume() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	volume *pVolume = nullptr;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext){
		m_pDreamOS->SetGravityState(false);

		// Ball to Volume
		pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);

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

RESULT PhysicsEngineTestSuite::AddTestVolumeVolumePointFace() {
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
		pVolume->SetVelocity(-1.0f/2, 0.0f, 0.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5f);
		CN(pVolume);
		pVolume->SetPosition(point(1.0f, 0.0f, 0.0f));
		pVolume->RotateYByDeg(45.0f);
		pVolume->RotateZByDeg(45.0f);
		pVolume->SetMass(1.0f);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

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


RESULT PhysicsEngineTestSuite::AddTestVolumeToPlaneVolume() {
	RESULT r = R_PASS;

	double sTestTime = 15.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(true);

		// Volume to "plane"
		auto pVolume = m_pDreamOS->AddVolume(5.0, 5.0, 1.0f);
		CN(pVolume);

		pVolume->SetPosition(point(0.0f, -3.0f, 0.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);

		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		pVolume = m_pDreamOS->AddVolume(0.5, 0.5, 2.0f);
		CN(pVolume);

		pVolume->SetPosition(point(0.5f, 1.0f, 0.0f));
		pVolume->SetMass(1.0f);
		pVolume->RotateZByDeg(45.0f);

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
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, m_pDreamOS);
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

		sphere *pSphere1 = m_pDreamOS->AddSphere(2.0f, 10, 10);
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

		float width = 5.0f;
		float wallThickness = 0.25f;
		float wallHeight = 1.0f;
		float posY = -3.0f;

		auto pVolume = m_pDreamOS->AddVolume(width, width, 1.0f);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, posY, 0.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		// left wall
		pVolume = m_pDreamOS->AddVolume(width, wallThickness, wallHeight);
		CN(pVolume);
		pVolume->SetPosition(point(-width/2.0f + wallThickness/2.0f, posY + wallHeight + DREAM_EPSILON, 0.0f));
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
		pVolume = m_pDreamOS->AddVolume(wallThickness, width - wallThickness*2.1f, wallHeight);
		CN(pVolume);
		pVolume->SetPosition(point(0.0f, posY + wallHeight + DREAM_EPSILON, width / 2.0f - wallThickness / 2.0f));
		pVolume->SetMass(100000.0f);
		pVolume->SetImmovable(true);
		CR(m_pDreamOS->AddPhysicsObject(pVolume));

		// back wall
		pVolume = m_pDreamOS->AddVolume(wallThickness, width - wallThickness*2.1f, wallHeight);
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

		// Quad vs Sphere
		double spacing = 1.25f;
		double angleFactor = 0.2f;

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

	double sTestTime = 6.0f;
	int nRepeats = 5;
	static int nRepeatCounter = 0;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Volume vs Volume edge edge

		volume *pVolume = nullptr;

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

		auto pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
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

		auto pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
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

		auto pVolumeCollide = m_pDreamOS->AddVolume(0.5f);
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

		auto pVolumeCollide = m_pDreamOS->AddVolume(0.5f);
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

		auto pSphereCollide = m_pDreamOS->AddSphere(0.25f, 10, 10);
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