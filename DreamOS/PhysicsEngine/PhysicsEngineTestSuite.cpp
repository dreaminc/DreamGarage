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

	//CR((AddTest((std::function<RESULT(void*)>)std::bind(&PhysicsEngineTestSuite::TestBallVolume, this, std::placeholders::_1), 
	//	reinterpret_cast<void*>(m_pDreamOS))));

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

	double sTestTime = 3.0f;
	int nRepeats = 2;

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