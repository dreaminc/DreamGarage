#include "UITestSuite.h"
#include "DreamOS.h"
#include "UI/UIMenuItem.h"
#include "DreamGarage/DreamUIBar.h"
#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/InteractionObjectEvent.h"

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
	UIBarFormat barFormat;

	m_pDreamUIBar = std::make_shared<DreamUIBar>(m_pDreamOS,iconFormat,labelFormat,barFormat);
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

RESULT UITestSuite::AddTests() {
	RESULT r = R_PASS;
	//CR(AddTestUI());
	CR(AddTestInteractionUI());
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
			CR(m_pDreamUIBar->HandleTriggerUp(m_menu, m_path));
		}
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			OVERLAY_DEBUG_SET("event", "menu up");
			CR(m_pDreamUIBar->HandleMenuUp(m_menu, m_path));
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
			
			CR(m_pDreamUIBar->HandleMenuUp(m_menu, m_path));
		} break;

		//TODO: Currently broken
		case SENSE_MOUSE_EVENT_TYPE::SENSE_MOUSE_MOVE: {
			// TODO:
			OVERLAY_DEBUG_SET("event", "mouse move");

			if (m_pDreamUIBar->IsVisible()) {

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
