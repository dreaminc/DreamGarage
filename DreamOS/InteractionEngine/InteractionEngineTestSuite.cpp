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

	CR(AddTestCompositeRay());

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

RESULT InteractionEngineTestSuite::AddTestCompositeRay() {
	RESULT r = R_PASS;

	double sTestTime = 60.0f;
	int nRepeats = 1;

	struct TestContext {
		composite *pComposite = nullptr;
		DimRay *pRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	};

	TestContext *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		m_pDreamOS->SetGravityState(false);

		// Params for this test
		int nNesting = 2;
		float size = 5.0f;
		double yPos = -2.0f;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		std::shared_ptr<composite> pChildComposite = nullptr;
		composite *pComposite = nullptr;

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

		// Create the nested composites / quads
		CR(AddNestedCompositeQuads(nNesting, size, pChildComposite));

		//pComposite->SetPosition(point(0.0f, yPos, 0.0f));
		pComposite->RotateXByDeg(90.0f);

		// The Ray
		pTestContext->pRay = m_pDreamOS->AddRay(point(-size/2, size/2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
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
	};

	// Test Code (this evaluates the test upon completion)
	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	// Update Code 
	auto fnUpdate = [=](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pComposite);
		CN(pTestContext->pRay);
		
		// Get ray from mouse
		CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
		//pTestContext->pRay->UpdateFromRay(rCast);
		CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

		/*

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

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

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