#include "InteractionEngineTestSuite.h"
#include "DreamOS.h"

#include "PhysicsEngine/CollisionManifold.h"

InteractionEngineTestSuite::InteractionEngineTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	// Subscribers
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(m_pDreamOS->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

//Success:
	Validate();
	return;
Error:
	Invalidate();
	return;
}

InteractionEngineTestSuite::~InteractionEngineTestSuite() {
	// empty
}

RESULT InteractionEngineTestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestCompositeRayController());
	//CR(AddTestCompositeRay());

Error:
	return r;
}

RESULT InteractionEngineTestSuite::ResetTest(void *pContext) {
	RESULT r = R_PASS;

	// Will reset the sandbox as needed between tests
	CN(m_pDreamOS);
	CR(m_pDreamOS->RemoveAllObjects());

Error:
	return r;
}

RESULT InteractionEngineTestSuite::Notify(InteractionObjectEvent *mEvent) {
	RESULT r = R_PASS;

	// handle event
	switch (mEvent->m_eventType) {
		case InteractionEventType::ELEMENT_INTERSECT_BEGAN: {
			DEBUG_LINEOUT("began");

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);
			
			if (pDimObj != nullptr) {
				pDimObj->RotateYByDeg(45.0f);
			}

		} break;

		case InteractionEventType::ELEMENT_INTERSECT_MOVED: {
			DEBUG_LINEOUT("moved");
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_ENDED: {
			DEBUG_LINEOUT("ended");

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);

			if (pDimObj != nullptr) {
				pDimObj->ResetRotation();
			}
		} break;
	}

//Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddNestedCompositeQuads(int nestingLevel, float size, std::shared_ptr<composite> pCompositeParent) {
	RESULT r = R_PASS;

	std::shared_ptr<composite> pCompositeChild = nullptr;
	std::shared_ptr<quad> pQuad = nullptr;

	float size2 = size / 2.0f;
	float size4 = size2 / 2.0f;
	float padding = 0.8f;
	float depth = 0.1f;

	if (nestingLevel > 0) {
		for (int i = 0; i < 4; i++) {
			pCompositeChild = pCompositeParent->AddComposite();
			CN(pCompositeChild);
			CR(pCompositeChild->InitializeOBB());

			switch (i) {
				case 0: {
					// top left
					pCompositeChild->SetPosition(point(-size4, 0.0f, size4));
				} break;

				case 1: {
					// top right
					pCompositeChild->SetPosition(point(size4, 0.0f, size4));
				} break;

				case 2: {
					// bottom left
					pCompositeChild->SetPosition(point(-size4, 0.0f, -size4));
				} break;

				case 3: {
					// bottom right
					pCompositeChild->SetPosition(point(size4, 0.0f, -size4));
				} break;
			}

			CR(AddNestedCompositeQuads((nestingLevel - 1), size2, pCompositeChild));
		}
	}
	else if (nestingLevel == 0) {
		for (int i = 0; i < 4; i++) {
			pQuad = pCompositeParent->AddQuad(size2 * padding, size2 * padding);
			CN(pQuad);

			switch (i) {
				case 0: {
					// top left
					pQuad->SetPosition(point(-size4, 0.0f, size4));
				} break;

				case 1: {
					// top right
					pQuad->SetPosition(point(size4, 0.0f, size4));
				} break;

				case 2: {
					// bottom left
					pQuad->SetPosition(point(-size4, 0.0f, -size4));
				} break;

				case 3: {
					// bottom right
					pQuad->SetPosition(point(size4, 0.0f, -size4));
				} break;
			}
		}
	}

Error:
	return r;
}
RESULT InteractionEngineTestSuite::InitializeRayCompositeTest(void* pContext) {
	RESULT r = R_PASS;
	m_pDreamOS->SetGravityState(false);

	// Params for this test
	int nNesting = 2;
	float size = 5.0f;
	double yPos = -2.0f;

	RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
	std::shared_ptr<composite> pChildComposite = nullptr;
	composite *pComposite = nullptr;
	std::shared_ptr<sphere> pSphere = nullptr;

	// Create a complex composite
	pComposite = m_pDreamOS->AddComposite();
	CN(pComposite);

	pTestContext->pComposite = pComposite;

	pComposite->InitializeOBB();
	//pComposite->SetMass(1.0f);

	///*
	pChildComposite = pComposite->AddComposite();
	CN(pChildComposite);
	CR(pChildComposite->InitializeOBB());
	//*/

	pSphere = pChildComposite->AddSphere(0.5f, 10, 10);
	CN(pSphere);
	pSphere->SetPosition(point(2.0f, 2.0f, 0.0f));

	// Create the nested composites / quads
	CR(AddNestedCompositeQuads(nNesting, size, pChildComposite));

	pComposite->SetPosition(point(0.0f, yPos, 0.0f));
	pComposite->RotateXByDeg(90.0f);

	// The Ray
	pTestContext->pRay = m_pDreamOS->AddRay(point(-size / 2, size / 2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
	CN(pTestContext->pRay);

	// Add composite to interaction
	CR(m_pDreamOS->AddInteractionObject(pComposite));

	// Collide point spheres
	for (int i = 0; i < 4; i++) {
		pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
		CN(pTestContext->pCollidePoint[i]);
		pTestContext->pCollidePoint[i]->SetVisible(false);
	}

	// Add Ray to interaction
	//CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay));
Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestCompositeRayController() {
	RESULT r = R_PASS;

	RayCompositeTestContext *pTestContext = new RayCompositeTestContext();

	auto fnInitialize = [&](void* pContext) {
		RESULT r = R_PASS;
		CR(InitializeRayCompositeTest(pContext));
	Error:
		return r;
	};

	auto fnUpdate = [&](void* pContext) {
		RESULT r = R_PASS;

		RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);

		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		// Get Ray from controller

		hand *pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr) {
			point p0 = pRightHand->GetPosition() - point(0.0f, 0.0f, 0.25f);
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			//TODO: this isn't perfectly accurate, especially when the head is rotated
			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());
			rCast = ray(p0, v2);

			CollisionManifold manifold = pTestContext->pComposite->Collide(rCast);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}

			pTestContext->pRay->UpdateFromRay(rCast);
			CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

		}


	Error:
		return r;
	};

	auto fnTest = [&](void* pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void* pContext) {
		RESULT r = R_PASS;

		RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		CR(ResetTest(pContext));
	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Controller Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(10000.0);
	pNewTest->SetTestRepeats(1);

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestCompositeRay() {
	RESULT r = R_PASS;

	double sTestTime = 60.0f;
	int nRepeats = 1;

	RayCompositeTestContext *pTestContext = new RayCompositeTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		CR(InitializeRayCompositeTest(pContext));
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

		RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);
		
		// Get ray from mouse
		CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
		//pTestContext->pRay->UpdateFromRay(rCast);
		//CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

		///*
		for (int i = 0; i < 4; i++) {
			pTestContext->pCollidePoint[i]->SetVisible(false);
		}

		// Check for composite collisions using the ray
		{
			CollisionManifold manifold = pTestContext->pComposite->Collide(rCast);

			if (manifold.NumContacts() > 0) {
				for (int i = 0; i < manifold.NumContacts(); i++) {
					pTestContext->pCollidePoint[i]->SetVisible(true);
					pTestContext->pCollidePoint[i]->SetOrigin(manifold.GetContactPoint(i).GetPoint());
				}
			}
		}
		//*/

	Error:
		return r;
	};

	// Update Code 
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);

		if (pTestContext != nullptr) {
			delete pTestContext;
			pTestContext = nullptr;
		}

		CR(ResetTest(pContext));

	Error:
		return r;
	};

	// Add the test
	auto pNewTest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pNewTest);

	pNewTest->SetTestName("Ray Events Test");
	pNewTest->SetTestDescription("Event handling test");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}