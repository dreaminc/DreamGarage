#include "InteractionEngineTestSuite.h"
#include "DreamOS.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

InteractionEngineTestSuite::InteractionEngineTestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	// Subscribers
	/*
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(m_pDreamOS->RegisterEventSubscriber((InteractionEventType)(i), this));
	}
	*/

	CR(r);

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

	CR(AddTestMultiPrimitiveRemove());

	CR(AddTestMultiPrimitiveComposite());

	CR(AddTestMultiPrimitive());

	CR(AddTestObjectBasedEvents());

	CR(AddTestNestedCompositeOBB());

	CR(AddTestCompositeRayNested());

	CR(AddTestCompositeRay());

	CR(AddTestCompositeRayController());

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
			DEBUG_LINEOUT("intersect began state: 0x%x", mEvent->m_activeState);

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);
			
			if (pDimObj != nullptr) {
				//pDimObj->RotateYByDeg(15.0f);
			}

		} break;

		case InteractionEventType::ELEMENT_INTERSECT_MOVED: {
			DEBUG_LINEOUT("intersect moved state: 0x%x", mEvent->m_activeState);
		} break;

		case InteractionEventType::ELEMENT_INTERSECT_ENDED: {
			DEBUG_LINEOUT("intersect ended state: 0x%x", mEvent->m_activeState);

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);

			if (pDimObj != nullptr) {
				//pDimObj->ResetRotation();
				//pDimObj->RotateYByDeg(-15.0f); 
				point ptPosition = pDimObj->GetPosition();
				
				// Remove object 
				m_pDreamOS->RemoveObject(pDimObj);
				m_pDreamOS->UnregisterInteractionObject(pDimObj);

				// Create new one
				auto pQuad = m_pDreamOS->AddQuad(1.0f, 1.0f);
				pQuad->SetPosition(point(0.0f, -2.0f, 0.0f));
				pQuad->SetColor(COLOR_RED);
				pQuad->SetPosition(ptPosition);

				// Add composite to interaction
				CRM(m_pDreamOS->AddObjectToInteractionGraph(pQuad), "Failed to add quad");

				// TODO: Simplify (combine with above)
				for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
					CR(m_pDreamOS->RegisterEventSubscriber(pQuad, (InteractionEventType)(i), this));
				}
			}
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_BEGAN: {
			DEBUG_LINEOUT("collide began state: 0x%x", mEvent->m_activeState);

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);

			if (pDimObj != nullptr) {
				pDimObj->RotateYByDeg(15.0f);
			}

		} break;

		case InteractionEventType::ELEMENT_COLLIDE_MOVED: {
			DEBUG_LINEOUT("collide moved state: 0x%x", mEvent->m_activeState);
		} break;

		case InteractionEventType::ELEMENT_COLLIDE_ENDED: {
			DEBUG_LINEOUT("collide ended state: 0x%x", mEvent->m_activeState);

			DimObj *pDimObj = dynamic_cast<DimObj*>(mEvent->m_pObject);

			if (pDimObj != nullptr) {
				//pDimObj->ResetRotation();
				pDimObj->RotateYByDeg(-15.0f);
			}
		} break;
	}

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestNestedCompositeOBB() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	RayCompositeTestContext *pTestContext = new RayCompositeTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
			std::shared_ptr<composite> pChildComposite = nullptr;
			composite *pComposite = nullptr;
			std::shared_ptr<sphere> pSphere = nullptr;

			// Create a complex composite
			pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);

			pTestContext->pComposite = pComposite;

			pComposite->InitializeOBB();

			///*
			pChildComposite = pComposite->AddComposite();
			CN(pChildComposite);
			CR(pChildComposite->InitializeOBB());

			auto pChildChildComposite = pChildComposite->AddComposite();
			CN(pChildChildComposite);
			CR(pChildChildComposite->InitializeOBB());
			//*/

			auto pActiveComposite = pChildChildComposite;

			auto pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);
			//pQuad->RotateXByDeg(90.0f);

			/*
			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(-1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, -1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pChildComposite->SetPosition(point(1.0f, 0.0f, 0.0f));

			pChildChildComposite->SetPosition(point(0.0f, 1.0f, 0.0f));
			*/
			pChildComposite->RotateYByDeg(45.0f);
			pChildChildComposite->RotateYByDeg(45.0f);

			pComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
			pComposite->RotateXByDeg(90.0f);
			//pComposite->RotateZByDeg(45.0f);

			// The Ray
			//pTestContext->pRay = m_pDreamOS->AddRay(point(-size / 2, size / 2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
			//CN(pTestContext->pRay);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pComposite));

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				//CR(m_pDreamOS->AddAndRegisterInteractionObject(pQuad.get(), (InteractionEventType)(i), this));
				CR(m_pDreamOS->RegisterEventSubscriber(pQuad.get(), (InteractionEventType)(i), this));
			}

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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
		ray rCast;

		// Get ray from mouse
		CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
		//CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

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

RESULT InteractionEngineTestSuite::AddTestMultiPrimitiveComposite() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	struct TestContext {
		composite *pComposite = nullptr;
		sphere *pSphere = nullptr;
		DimRay *pRay[2] = { nullptr, nullptr };
		DimRay *pMouseRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

			// Create a complex composite
			pTestContext->pComposite = m_pDreamOS->AddComposite();
			CN(pTestContext->pComposite);
			pTestContext->pComposite->InitializeOBB();
			pTestContext->pComposite->SetPosition(point(0.0f, -2.0f, 0.0f));

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pComposite));

			auto pQuad = pTestContext->pComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetColor(COLOR_BLUE);
			//pQuad->SetPosition(point(0.0f, -2.0f, 0.0f));
			//pQuad->RotateXByDeg(90.0f);
			//pQuad->RotateYByDeg(45.0f);

			pQuad = pTestContext->pComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetColor(COLOR_BLUE);
			pQuad->SetPosition(point(2.0f, 0.0f, 0.0f));

			pQuad = pTestContext->pComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetColor(COLOR_BLUE);
			pQuad->SetPosition(point(4.0f, 0.0f, 0.0f));

			// The Ray
			pTestContext->pRay[0] = m_pDreamOS->AddRay(point(-2.0f, 0.0f, 0.0f), vector(0.0f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay[0]);

			pTestContext->pRay[1] = m_pDreamOS->AddRay(point(-2.1f, 0.0f, 0.0f), vector(0.0f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay[1]);

			pTestContext->pMouseRay = m_pDreamOS->AddRay(point(-0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f).Normal());
			CN(pTestContext->pMouseRay);

			pTestContext->pSphere = m_pDreamOS->AddSphere(0.05f, 10, 10);
			CN(pTestContext->pSphere);
			//pTestContext->pSphere->SetPosition(point(-2.0f, -1.98f, 0.0f));
			//pTestContext->pSphere->SetPosition(point(-2.0f, -2.55f, 0.0f));
			pTestContext->pSphere->SetPosition(point(-2.0f, -2.02f, 0.0f));
			pTestContext->pSphere->RotateXByDeg(180.0f);

			// Add Ray to interaction engine
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay[0]));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay[1]));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pMouseRay));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pSphere));

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pComposite, (InteractionEventType)(i), this));
			}

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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

		ray rCast;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		// This updates the ray from the mouse
		if (pTestContext != nullptr) {

			CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
			pTestContext->pMouseRay->UpdateFromRay(rCast);

			pTestContext->pRay[0]->translateX(0.00052f);
			pTestContext->pRay[1]->translateX(0.00051f);
			pTestContext->pSphere->translateX(0.00075f);
		}

		CR(r);

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

	pNewTest->SetTestName("Multi Primitive Interaction Composite Test");
	pNewTest->SetTestDescription("Test covering multi interaction objects interacting with composite objects");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestMultiPrimitiveRemove() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	struct TestContext {
		quad *pQuad[4] = { nullptr, nullptr, nullptr, nullptr };
		DimRay *pMouseRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

			pTestContext->pQuad[0] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[0]->SetPosition(point(0.0f, -2.0f, 0.0f));
			pTestContext->pQuad[0]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[0]));

			pTestContext->pQuad[1] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[1]->SetPosition(point(2.0f, -2.0f, 0.0f));
			pTestContext->pQuad[1]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[1]));

			pTestContext->pQuad[2] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[2]->SetPosition(point(4.0f, -2.0f, 0.0f));
			pTestContext->pQuad[2]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[2]));

			pTestContext->pMouseRay = m_pDreamOS->AddRay(point(-0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f).Normal());
			CN(pTestContext->pMouseRay);

			// Add Ray to interaction engine
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pMouseRay));

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				for (int j = 0; j < 3; j++) {
					CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pQuad[j], (InteractionEventType)(i), this));
				}
			}

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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

		ray rCast;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		// This updates the ray from the mouse
		if (pTestContext != nullptr) {

			CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
			pTestContext->pMouseRay->UpdateFromRay(rCast);
		}

		CR(r);

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

	pNewTest->SetTestName("Multi Primitive Interaction Test");
	pNewTest->SetTestDescription("Test covering multi interaction objects interacting with non composite objects");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestMultiPrimitive() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	struct TestContext {
		quad *pQuad[4] = { nullptr, nullptr, nullptr, nullptr };
		sphere *pSphere = nullptr;
		DimRay *pRay[2] = { nullptr, nullptr };
		DimRay *pMouseRay = nullptr;
		sphere *pCollidePoint[4] = { nullptr, nullptr, nullptr, nullptr };
	} *pTestContext = new TestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

			pTestContext->pQuad[0] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[0]->SetPosition(point(0.0f, -2.0f, 0.0f));
			pTestContext->pQuad[0]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[0]));

			pTestContext->pQuad[1] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[1]->SetPosition(point(2.0f, -2.0f, 0.0f));
			pTestContext->pQuad[1]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[1]));

			pTestContext->pQuad[2] = m_pDreamOS->AddQuad(1.0f, 1.0f);
			pTestContext->pQuad[2]->SetPosition(point(4.0f, -2.0f, 0.0f));
			pTestContext->pQuad[2]->SetColor(COLOR_BLUE);
			//pTestContext->pQuad->RotateXByDeg(90.0f);
			//pTestContext->pQuad->RotateYByDeg(45.0f);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pTestContext->pQuad[2]));

			// The Ray
			pTestContext->pRay[0] = m_pDreamOS->AddRay(point(-2.0f, 0.0f, 0.0f), vector(0.0f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay[0]);

			pTestContext->pRay[1] = m_pDreamOS->AddRay(point(-2.1f, 0.0f, 0.0f), vector(0.0f, -1.0f, 0.0f).Normal());
			CN(pTestContext->pRay[1]);

			pTestContext->pMouseRay = m_pDreamOS->AddRay(point(-0.0f, 0.0f, 0.0f), vector(0.0f, 1.0f, 0.0f).Normal());
			CN(pTestContext->pMouseRay);

			pTestContext->pSphere = m_pDreamOS->AddSphere(0.05f, 10, 10);
			CN(pTestContext->pSphere);
			//pTestContext->pSphere->SetPosition(point(-2.0f, -1.98f, 0.0f));
			//pTestContext->pSphere->SetPosition(point(-2.0f, -2.55f, 0.0f));
			pTestContext->pSphere->SetPosition(point(-2.0f, -2.02f, 0.0f));
			//pTestContext->pSphere->RotateXByDeg(180.0f);

			// Add Ray to interaction engine
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay[0]));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pRay[1]));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pMouseRay));
			CR(m_pDreamOS->AddInteractionObject(pTestContext->pSphere));

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				for (int j = 0; j < 3; j++) {
					CR(m_pDreamOS->RegisterEventSubscriber(pTestContext->pQuad[j], (InteractionEventType)(i), this));
				}
			}

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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

		ray rCast;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		// This updates the ray from the mouse
		if (pTestContext != nullptr) {
			
			CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
			pTestContext->pMouseRay->UpdateFromRay(rCast);
			
			pTestContext->pRay[0]->translateX(0.00052f);
			pTestContext->pRay[1]->translateX(0.00051f);
			pTestContext->pSphere->translateX(0.00075f);
		}

		CR(r);

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

	pNewTest->SetTestName("Multi Primitive Interaction Test");
	pNewTest->SetTestDescription("Test covering multi interaction objects interacting with non composite objects");
	pNewTest->SetTestDuration(sTestTime);
	pNewTest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT InteractionEngineTestSuite::AddTestObjectBasedEvents() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	RayCompositeTestContext *pTestContext = new RayCompositeTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
			std::shared_ptr<composite> pChildComposite = nullptr;
			composite *pComposite = nullptr;
			std::shared_ptr<sphere> pSphere = nullptr;

			// Create a complex composite
			pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);

			pTestContext->pComposite = pComposite;

			pComposite->InitializeOBB();

			///*
			pChildComposite = pComposite->AddComposite();
			CN(pChildComposite);
			CR(pChildComposite->InitializeOBB());
			//*/

			auto pChildChildComposite = pChildComposite->AddComposite();
			CN(pChildChildComposite);
			CR(pChildChildComposite->InitializeOBB());

			auto pActiveComposite = pChildChildComposite;

			auto pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(-1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, -1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pChildComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
			pChildComposite->RotateYByDeg(45.0f);

			pChildChildComposite->SetPosition(point(0.0f, 1.0f, 0.0f));
			pChildChildComposite->RotateYByDeg(45.0f);

			pComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
			pComposite->RotateXByDeg(90.0f);
			//pComposite->RotateZByDeg(45.0f);

			// The Ray
			//pTestContext->pRay = m_pDreamOS->AddRay(point(-size / 2, size / 2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
			//CN(pTestContext->pRay);

			// Add composite to interaction
			CR(m_pDreamOS->AddObjectToInteractionGraph(pComposite));

			for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
				//CR(m_pDreamOS->AddAndRegisterInteractionObject(pQuad.get(), (InteractionEventType)(i), this));
				CR(m_pDreamOS->RegisterEventSubscriber(pQuad.get(), (InteractionEventType)(i), this));
				//CR(m_pDreamOS->RegisterEventSubscriber(pComposite, (InteractionEventType)(i), this));
			}

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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
		ray rCast;

		// Get ray from mouse
		CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));
		//CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

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

RESULT InteractionEngineTestSuite::SetupPipeline() {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = m_pDreamOS->GetHALImp();
	Pipeline* pPipeline = pHAL->GetRenderPipelineHandle();

	SinkNode* pDestSinkNode = pPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("blinnphong_text");
	//ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal_texture");
	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("minimal");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));

	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	CR(pHAL->ReleaseCurrentContext());

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

	///*
	pChildComposite = pComposite->AddComposite();
	CN(pChildComposite);
	CR(pChildComposite->InitializeOBB());
	//*/

	pSphere = pChildComposite->AddSphere(0.5f, 10, 10);
	CN(pSphere);
	pSphere->SetPosition(point(2.0f, 2.0f, 0.0f));
	//pSphere->SetColor(COLOR_RED);

	// Create the nested composites / quads
	CR(AddNestedCompositeQuads(nNesting, size, pChildComposite));

	pComposite->SetPosition(point(0.0f, yPos, 0.0f));
	pComposite->RotateXByDeg(90.0f);

	// The Ray
	pTestContext->pRay = m_pDreamOS->AddRay(point(-size / 2, size / 2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
	CN(pTestContext->pRay);

	// Add composite to interaction
	CR(m_pDreamOS->AddObjectToInteractionGraph(pComposite));

	// Collide point spheres
	for (int i = 0; i < 4; i++) {
		pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
		CN(pTestContext->pCollidePoint[i]);
		pTestContext->pCollidePoint[i]->SetVisible(false);
		pTestContext->pCollidePoint[i]->SetColor(COLOR_RED);
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
			//CR(m_pDreamOS->UpdateInteractionPrimitive(rCast));

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

		CN(m_pDreamOS);

		CR(SetupPipeline());

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

RESULT InteractionEngineTestSuite::AddTestCompositeRayNested() {
	RESULT r = R_PASS;

	double sTestTime = 100.0f;
	int nRepeats = 1;

	RayCompositeTestContext *pTestContext = new RayCompositeTestContext();

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;

		CN(m_pDreamOS);

		m_pDreamOS->SetGravityState(false);

		CR(SetupPipeline());

		{
			RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
			std::shared_ptr<composite> pChildComposite = nullptr;
			composite *pComposite = nullptr;
			std::shared_ptr<sphere> pSphere = nullptr;

			// Create a complex composite
			pComposite = m_pDreamOS->AddComposite();
			CN(pComposite);

			pTestContext->pComposite = pComposite;

			pComposite->InitializeOBB();

			///*
			pChildComposite = pComposite->AddComposite();
			CN(pChildComposite);
			CR(pChildComposite->InitializeOBB());
			//*/

			auto pChildChildComposite = pChildComposite->AddComposite();
			CN(pChildChildComposite);
			CR(pChildChildComposite->InitializeOBB());

			auto pActiveComposite = pChildChildComposite;

			auto pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(-1.5f, 0.0f, 0.0f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, 1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pQuad = pActiveComposite->AddQuad(1.0f, 1.0f);
			pQuad->SetPosition(point(0.0f, 0.0f, -1.5f));
			pQuad->SetColor(COLOR_BLUE);

			pChildComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
			pChildComposite->RotateYByDeg(45.0f);

			pChildChildComposite->SetPosition(point(0.0f, 1.0f, 0.0f));
			pChildChildComposite->RotateYByDeg(45.0f);

			pComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
			pComposite->RotateXByDeg(90.0f);
			//pComposite->RotateZByDeg(45.0f);

			// The Ray
			//pTestContext->pRay = m_pDreamOS->AddRay(point(-size / 2, size / 2, 2.0f), vector(0.0f, 0.0f, -1.0f).Normal());
			//CN(pTestContext->pRay);

			// Add composite to interaction
			//CR(m_pDreamOS->AddInteractionObject(pComposite));

			// Collide point spheres
			for (int i = 0; i < 4; i++) {
				pTestContext->pCollidePoint[i] = m_pDreamOS->AddSphere(0.025f, 10, 10);
				CN(pTestContext->pCollidePoint[i]);
				pTestContext->pCollidePoint[i]->SetVisible(false);
			}
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

		RayCompositeTestContext *pTestContext = reinterpret_cast<RayCompositeTestContext*>(pContext);
		ray rCast;

		CN(pTestContext->pComposite);
		//CN(pTestContext->pRay);
		
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