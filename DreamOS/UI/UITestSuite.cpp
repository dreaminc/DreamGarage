#include "UITestSuite.h"
#include "DreamOS.h"
#include "UI/UIMenuItem.h"
#include "DreamGarage/DreamUIBar.h"
#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/InteractionObjectEvent.h"

#include "DreamGarage/DreamContentView.h"

UITestSuite::UITestSuite(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS)
{
	RESULT r = R_PASS;

	CR(Initialize());

	Validate();
	return;
Error:
	Invalidate();
	return;
}

UITestSuite::~UITestSuite() {
	// empty
}

RESULT UITestSuite::AddTests() {
	RESULT r = R_PASS;

	CR(AddTestInteractionFauxUI());
	CR(AddTestSharedContentView());

	//CR(AddTestUI());
	CR(AddTestInteractionUI());

Error:
	return r;
}

RESULT UITestSuite::Initialize() {
	RESULT r = R_PASS;

	
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	m_pDreamOS->AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	m_pDreamOS->AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	point sceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector sceneDirection = vector(0.0f, 0.0f, 0.0f);
/*
	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\env.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	composite* pRiver = m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\river.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	m_pDreamOS->AddModel(L"\\Models\\FloatingIsland\\clouds.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
//*/
	for (int i = 0; i < SenseControllerEventType::SENSE_CONTROLLER_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseControllerEventType)(i), this));
	}

	for (int i = 0; i < SenseMouseEventType::SENSE_MOUSE_INVALID; i++) {
		CR(m_pDreamOS->RegisterSubscriber((SenseMouseEventType)(i), this));
	}

Error:
	return r;
}

RESULT UITestSuite::InitializeUI() {
	RESULT r = R_PASS;

	IconFormat iconFormat;
	LabelFormat labelFormat;
	RadialLayerFormat menuFormat;
	RadialLayerFormat titleFormat;

	// differences from default for title layer
	titleFormat.menuPosZ = -1.15f;
	titleFormat.itemPosY = -0.25f;
	titleFormat.itemAngleX = 75.0f;

	m_pDreamUIBar = std::make_shared<DreamUIBar>(m_pDreamOS,iconFormat,labelFormat,menuFormat,titleFormat);
	m_pDreamUIBar->SetVisible(false);

	m_menu[""] = { "lorem", "ipsum", "dolor", "sit" };
	m_menu["lorem"] = { "Watch", "Listen", "Play", "Whisper", "Present" };
	m_menu["ipsum"] = { "1", "2", "3" };
	m_menu["Play"] = { "a", "b", "c" };

	m_path = {};

	m_pSphere1 = m_pDreamOS->AddSphere(0.02f, 10, 10);
	m_pSphere2 = m_pDreamOS->AddSphere(0.02f, 10, 10);

//Error:
	return r;
}

RESULT UITestSuite::AddTestSharedContentView() {
	RESULT r = R_PASS;

	double sTestTime = 30.0f;
	int nRepeats = 1;

	// Initialize Code 
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		std::shared_ptr<DreamContentView> pDreamContentView = nullptr;

		CN(m_pDreamOS);

		// Create the Shared View App
		pDreamContentView = m_pDreamOS->LaunchDreamApp<DreamContentView>(this);
		CNM(pDreamContentView, "Failed to create dream content view");

		// Set up the view
		pDreamContentView->SetParams(point(0.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

		//pDreamContentView->SetScreenTexture(L"crate_color.png");
		//pDreamContentView->SetScreenURI("https://www.google.com/images/branding/googlelogo/2x/googlelogo_color_272x92dp.png");
		pDreamContentView->SetScreenURI("https://static.dreamos.com/www/image/hero.387eddfc05dc.jpg");

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

	// Reset Code 
	auto fnReset = [&](void *pContext) {
		RESULT r = R_PASS;

		// Will reset the sandbox as needed between tests
		CN(m_pDreamOS);
		CR(m_pDreamOS->RemoveAllObjects());

	Error:
		return r;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, nullptr);
	CN(pUITest);

	pUITest->SetTestName("Local Shared Content View Test");
	pUITest->SetTestDescription("Basic test of shared content view working locally");
	pUITest->SetTestDuration(sTestTime);
	pUITest->SetTestRepeats(nRepeats);

Error:
	return r;
}

RESULT UITestSuite::AddTestInteractionFauxUI() {
	RESULT r = R_PASS;

	struct TestContext {
		DimRay *pRay = nullptr;
		composite *pComposite = nullptr;
	};

	TestContext *pTestContext = new TestContext();

	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		composite *pComposite = nullptr;
		std::shared_ptr<composite> pChildComposite = nullptr;
		std::shared_ptr<composite> pChildItemComposite = nullptr;
		std::shared_ptr<quad> pQuad = nullptr;

		pTestContext->pRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
		CN(pTestContext->pRay);

		
		m_pSphere1 = m_pDreamOS->AddSphere(0.02f, 10, 10);
		m_pSphere2 = m_pDreamOS->AddSphere(0.02f, 10, 10);
		
		// Create Faux UI here

		pComposite = m_pDreamOS->AddComposite();
		CN(pComposite);

		pTestContext->pComposite = pComposite;
		pComposite->InitializeOBB();

		//pComposite->SetMass(1.0f);

		// Layer
		pChildComposite = pComposite->AddComposite();
		CN(pChildComposite);
		CR(pChildComposite->InitializeOBB());

		// Quads
		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(20.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(-1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
		//pChildItemComposite->RotateYByDeg(0.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-10.0f);

		pChildItemComposite = pChildComposite->AddComposite();
		CR(pChildItemComposite->InitializeOBB());
		pQuad = pChildItemComposite->AddQuad(0.5f, 0.5f);
		pQuad->RotateXByDeg(105.0f);
		pChildItemComposite->SetPosition(point(2.0f, 0.0f, 0.0f));
		pChildItemComposite->RotateYByDeg(-20.0f);

		// Move Composite
		pChildComposite->SetPosition(point(0.0f, 0.0f, -4.0f));

		pComposite->SetPosition(point(0.0f, 1.5f, 6.0f));

		// Add composite to interaction
		//CR(m_pDreamOS->AddInteractionObject(pComposite));

	Error:
		return r;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		hand* pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr && pTestContext->pRay != nullptr) {
			pTestContext->pRay->SetPosition(pRightHand->GetPosition());
			pTestContext->pRay->SetOrientation(pRightHand->GetHandState().qOrientation);

			point p0 = pRightHand->GetPosition();
			//GetLookVector
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());
			vector vHandLook = RotationMatrix(q) * vector(0.0f, 0.0f, -1.0f);

			ray rcast = ray(p0, vHandLook);

			CollisionManifold manifold = pTestContext->pComposite->Collide(rcast);

			if (manifold.NumContacts() > 0) {
				int numContacts = manifold.NumContacts();

				if (numContacts > 2)
					numContacts = 2;
				for (int i = 0; i < numContacts; i++) {
					sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

					if (pSphere != nullptr) {
						pSphere->SetVisible(true);
						pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}
		

			m_pDreamOS->UpdateInteractionPrimitive(rcast);

		}
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("UI Faux Interaction Engine Test");
	pUITest->SetTestDescription("UI Basic Testing Environment");
	pUITest->SetTestDuration(10000.0);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::AddTestInteractionUI() {
	RESULT r = R_PASS;

	struct TestContext {
		DimRay *pRay = nullptr;
	};

	TestContext *pTestContext = new TestContext();

	//TODO: once there are multiple tests, may want to share some of this code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		pTestContext->pRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));

		CR(InitializeUI());
	Error:
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		hand* pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr && m_pDreamUIBar->IsVisible() && pTestContext->pRay != nullptr) {
			pTestContext->pRay->SetPosition(pRightHand->GetPosition());
			pTestContext->pRay->SetOrientation(pRightHand->GetHandState().qOrientation);

			point p0 = pRightHand->GetPosition();
			//GetLookVector
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());

			p0 = p0 + point(-10.0f * v2);
			ray rcast = ray(p0, v2);


			CollisionManifold manifold = m_pDreamUIBar->GetComposite()->Collide(rcast);

			if (manifold.NumContacts() > 0) {
				int numContacts = manifold.NumContacts();

				if (numContacts > 2)
					numContacts = 2;
				for (int i = 0; i < numContacts; i++) {
					sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

					pSphere->SetVisible(true);
					pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
				}
			}

			m_pDreamOS->UpdateInteractionPrimitive(rcast);

		}
		return r;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("UI Menu Interaction Engine Test");
	pUITest->SetTestDescription("UI Basic Testing Environment");
	pUITest->SetTestDuration(10000.0);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::AddTestUI() {
	RESULT r = R_PASS;

	struct TestContext {

		DimRay *pRay = nullptr;
	};

	TestContext *pTestContext = new TestContext();

	//TODO: once there are multiple tests, may want to share some of this code
	auto fnInitialize = [&](void *pContext) {
		RESULT r = R_PASS;
		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);
		pTestContext->pRay = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));

		CR(InitializeUI());
	Error:
		return r;
	};

	auto fnTest = [&](void *pContext) {
		return R_PASS;
	};

	auto fnUpdate = [&](void *pContext) {
		RESULT r = R_PASS;

		TestContext *pTestContext = reinterpret_cast<TestContext*>(pContext);

		hand* pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

		if (pRightHand != nullptr && m_pDreamUIBar->IsVisible() && pTestContext->pRay != nullptr) {
			pTestContext->pRay->SetPosition(pRightHand->GetPosition());
			pTestContext->pRay->SetOrientation(pRightHand->GetHandState().qOrientation);

			point p0 = pRightHand->GetPosition();
			//GetLookVector
			quaternion q = pRightHand->GetHandState().qOrientation;
			q.Normalize();

			vector v = q.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();
			vector v2 = vector(-v.x(), -v.y(), v.z());

			p0 = p0 + point(-10.0f * v2);
			ray rcast = ray(p0, v2);


			CollisionManifold manifold = m_pDreamUIBar->GetComposite()->Collide(rcast);

			if (manifold.NumContacts() > 0) {
				int numContacts = manifold.NumContacts();

				if (numContacts > 2)
					numContacts = 2;
				for (int i = 0; i < numContacts; i++) {
					sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

					pSphere->SetVisible(true);
					pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
				}
			}

			if (manifold.NumContacts() > 0) {
				VirtualObj* pObjA = manifold.GetObjectA();
				VirtualObj* pObjB = manifold.GetObjectB();

				if (pObjA && (!m_pPrevSelected || pObjA != m_pPrevSelected)) {
					pObjA->ScaleX(m_pDreamUIBar->GetLargeItemScale());
					pObjA->ScaleZ(m_pDreamUIBar->GetLargeItemScale());

					if (m_pPrevSelected) {
						m_pPrevSelected->ScaleX(1.0f);
						m_pPrevSelected->ScaleZ(1.0f);
					}

					m_pPrevSelected = pObjA;
				}
				else if (pObjB && (!m_pPrevSelected || pObjB != m_pPrevSelected)) {
					pObjB->ScaleX(m_pDreamUIBar->GetLargeItemScale());
					pObjB->ScaleZ(m_pDreamUIBar->GetLargeItemScale());

					if (m_pPrevSelected) {
						m_pPrevSelected->ScaleX(1.0f);
						m_pPrevSelected->ScaleZ(1.0f);
					}

					m_pPrevSelected = pObjB;
				}
				m_pSphere1->SetVisible(true);
				m_pSphere2->SetVisible(true);

				m_pSphere1->SetPosition(manifold.GetContactPoint(0).GetPoint());
				m_pSphere2->SetPosition(manifold.GetContactPoint(1).GetPoint());
			}
			else if (manifold.NumContacts() == 0) {
				if (m_pPrevSelected != nullptr) {
					m_pPrevSelected->ScaleX(1.0f);
					m_pPrevSelected->ScaleZ(1.0f);
				}

				m_pPrevSelected = nullptr;
			}
		}

		return r;
	};

	auto fnReset = [&](void *pContext) {
		return R_PASS;
	};

	auto pUITest = AddTest(fnInitialize, fnUpdate, fnTest, fnReset, pTestContext);
	CN(pUITest);

	pUITest->SetTestName("UI Menu Test");
	pUITest->SetTestDescription("UI Basic Testing Environment");
	pUITest->SetTestDuration(10000.0);
	pUITest->SetTestRepeats(1);

Error:
	return r;
}

RESULT UITestSuite::Notify(SenseControllerEvent *event) {
	RESULT r = R_PASS;

	SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;
	OVERLAY_DEBUG_SET("event", "none");

	if (event->state.type == CONTROLLER_RIGHT) {
		if (eventType == SENSE_CONTROLLER_TRIGGER_MOVE) {
			OVERLAY_DEBUG_SET("event", "trigger move");
		}
		else if (eventType == SENSE_CONTROLLER_PAD_MOVE) {
			OVERLAY_DEBUG_SET("event", "pad move");
		}

		else if (eventType == SENSE_CONTROLLER_TRIGGER_DOWN) {
			OVERLAY_DEBUG_SET("event", "trigger down");
		}

		// TODO:  soon this code will be replaced with api requests, 
		// as opposed to accessing the hardcoded local data structures
		else if (eventType == SENSE_CONTROLLER_TRIGGER_UP) {
			OVERLAY_DEBUG_SET("event", "trigger up");
			CR(m_pDreamUIBar->HandleTriggerUp());
		}
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			OVERLAY_DEBUG_SET("event", "menu up");
			CR(m_pDreamUIBar->HandleMenuUp());
		}
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_DOWN) {
		OVERLAY_DEBUG_SET("event", "grip down");
	}
	else if (eventType == SENSE_CONTROLLER_GRIP_UP) {
		OVERLAY_DEBUG_SET("event", "grip up");
	}
	else if (eventType == SENSE_CONTROLLER_MENU_DOWN) {
		OVERLAY_DEBUG_SET("event", "menu down");
	}
Error:
	return r;
}

RESULT UITestSuite::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;
	CR(r);
Error:
	return r;
}

RESULT UITestSuite::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	switch (mEvent->EventType) {
		case SENSE_MOUSE_EVENT_TYPE::SENSE_MOUSE_RIGHT_BUTTON_UP: {

			OVERLAY_DEBUG_SET("event", "mouse left up");
			
			if (m_pDreamUIBar != nullptr) {
				CR(m_pDreamUIBar->HandleMenuUp());
			}
		} break;

		//TODO: Currently broken
		case SENSE_MOUSE_EVENT_TYPE::SENSE_MOUSE_MOVE: {
			// TODO:
			OVERLAY_DEBUG_SET("event", "mouse move");

			if (m_pDreamUIBar != nullptr && m_pDreamUIBar->IsVisible()) {
				// update ray / test stuff
				ray rCast;
				CR(m_pDreamOS->GetMouseRay(rCast, 0.0f));

				CollisionManifold manifold = m_pDreamUIBar->GetComposite()->Collide(rCast);

				if (manifold.NumContacts() > 0) {
					int numContacts = manifold.NumContacts();

					if (numContacts > 2)
						numContacts = 2;

					for (int i = 0; i < numContacts; i++) {
						sphere *pSphere = (i == 0) ? m_pSphere1 : m_pSphere2;

						pSphere->SetVisible(true);
						pSphere->SetPosition(manifold.GetContactPoint(i).GetPoint());
					}
				}
			}
			
		} break;
	}
Error:
	return r;
}
