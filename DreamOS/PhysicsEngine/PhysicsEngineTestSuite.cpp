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

	CR((AddTest((std::function<RESULT(void*)>)std::bind(&PhysicsEngineTestSuite::TestBallVolume, this, std::placeholders::_1), 
		reinterpret_cast<void*>(m_pDreamOS))));

Error:
	return r;
}

RESULT PhysicsEngineTestSuite::TestBallVolume(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = reinterpret_cast<DreamOS*>(pContext);
	CVM(pDreamOS, "DreamOS instance handle is invalid");

	// Initialize
	{
		pDreamOS->SetGravityState(false);

		// Ball to Volume
		auto pVolume = pDreamOS->AddVolume(0.5, 0.5, 2.0f);

		pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pVolume->SetMass(10.0f);
		//pVolume->SetRotationalVelocity(vector(0.0f, 0.0f, 5.6f));
		//pVolume->RotateZByDeg(-135.0f);
		//pVolume->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		//pVolume->ApplyTorqueImpulse(vector(0.0f, 0.0f, 1.0f));
		//pVolume->ApplyTorqueImpulse(vector(0.0f, 0.1f, 0.0f));
		//pVolume->ApplyForceAtPoint(vector(-10.0f, 0.0f, 10.0f), point(0.5f, 1.5f, 0.5f), 0.02f);
		pDreamOS->AddPhysicsObject(pVolume);

		auto pSphere = pDreamOS->AddSphere(0.25f, 10, 10);
		pSphere->SetPosition(point(3.0f, 0.75f, 0.0f));
		pSphere->SetMass(1.0f);
		pSphere->SetVelocity(vector(-1.0f, 0.0f, 0.0f));
		pDreamOS->AddPhysicsObject(pSphere);
	}

Error:
	return r;
}