#include "UIModule.h"
#include "Primitives/camera.h"

#include "DreamOS.h"

UIModule::UIModule(DreamOS *pDreamOS) :
	m_pDreamOS(pDreamOS),
	m_pCompositeContext(nullptr)
{
	RESULT r = R_PASS;

	CR(UIModule::Initialize());

// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}
UIModule::~UIModule() {
	// empty
}

RESULT UIModule::Initialize() {
	RESULT r = R_PASS;

	// Subscribers
	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(m_pDreamOS->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	CN(m_pDreamOS);
	m_pCompositeContext = m_pDreamOS->AddComposite();
	CN(m_pCompositeContext);

	CR(m_pCompositeContext->InitializeOBB());
	CR(m_pDreamOS->AddInteractionObject(m_pCompositeContext));

	m_pCurrentItem = nullptr;

	/*
	m_pTestRayController = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
	m_pTestRayLookV = m_pDreamOS->AddRay(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, -1.0f));
	m_pSphere = m_pDreamOS->AddSphere(0.02f, 10, 10);
	*/

Error:
	return r;
}

ray UIModule::GetHandRay() {
	
	RESULT r = R_PASS;

	ray rCast;
	hand *pRightHand = m_pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);

	CBR(IsVisible(), R_SKIPPED); 
	CBR(pRightHand != nullptr, R_OBJECT_NOT_FOUND)
	{
		point ptHand = pRightHand->GetPosition();

		//GetLookVector
		quaternion qHand = pRightHand->GetHandState().qOrientation;
		qHand.Normalize();

		if (m_pTestRayController != nullptr) {
			m_pTestRayController->SetPosition(ptHand);
			m_pTestRayController->SetOrientation(qHand);
		}

		//TODO: investigate how to properly get look vector for controllers
		//vector vHandLook = qHand.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		vector vCast = vector(-vHandLook.x(), -vHandLook.y(), vHandLook.z());

		if (m_pTestRayController != nullptr) {
			m_pTestRayLookV->SetPosition(ptHand);
			m_pTestRayLookV->SetOrientation(quaternion(vHandLook));
		}

		// Accommodate for composite collision bug
		//ptHand = ptHand + point(-10.0f * vCast);
		//rCast = ray(ptHand, vCast);
		//rCast = m_pTestRayController->GetRayFromVerts();

		rCast = ray(ptHand, vHandLook);
	}
	return rCast;

Error:
	return ray(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, 0.0f));
}

RESULT UIModule::UpdateInteractionPrimitive(ray rCast) {
	return m_pDreamOS->UpdateInteractionPrimitive(rCast);
}

std::shared_ptr<UIMenuLayer> UIModule::CreateMenuLayer() {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuLayer> pUIMenuLayer = std::make_shared<UIMenuLayer>(m_pCompositeContext);;
	CN(pUIMenuLayer);
	CV(pUIMenuLayer);

	m_layers.emplace_back(pUIMenuLayer);
	m_pCurrentUILayer = pUIMenuLayer;

Error:
	return pUIMenuLayer;
}

std::shared_ptr<UIMenuLayer> UIModule::GetCurrentLayer() {
	return m_pCurrentUILayer;
}

std::shared_ptr<UIMenuItem> UIModule::GetMenuItem(VirtualObj *pObj) {
	for (auto& pLayer : m_layers) {
		auto pItem = pLayer->GetMenuItem(pObj);
		if (pItem != nullptr) {
			return pItem;

		}
	}
	return nullptr;
}

std::shared_ptr<UIMenuItem> UIModule::GetCurrentItem() {
	return m_pCurrentItem;
}

RESULT UIModule::ToggleVisible() {

	RESULT r = R_PASS;

	m_pCompositeContext->SetVisible(!m_pCompositeContext->IsVisible());

	// if the context has become visible, reset its position to match the camera
	if (m_pCompositeContext->IsVisible()) {
		quaternion q = m_pCompositeContext->GetCamera()->GetOrientation();
		m_headRotationYDeg = q.ProjectedYRotationDeg();
		quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, m_headRotationYDeg * M_PI / 180.0f, 0.0f);

		m_pCompositeContext->SetPosition(m_pCompositeContext->GetCamera()->GetPosition());
		m_pCompositeContext->SetOrientation(q2);
	}

	return r;
}

RESULT UIModule::Show() {
	RESULT r = R_PASS;
	CR(m_pCompositeContext->SetVisible(true));
Error:
	return R_PASS;
}

RESULT UIModule::Hide() {
	RESULT r = R_PASS;
	CR(m_pCompositeContext->SetVisible(false));
Error:
	return R_PASS;
}

RESULT UIModule::SetVisible(bool fVisible) {
	return m_pCompositeContext->SetVisible(fVisible);
}

bool UIModule::IsVisible() {
	if(m_pCompositeContext != nullptr)
		return m_pCompositeContext->IsVisible();
	
	return false;
}

composite *UIModule::GetComposite() {
	return m_pCompositeContext;
}

RESULT UIModule::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	if (event->m_numContacts > 0 && m_pSphere != nullptr) {
		m_pSphere->SetPosition(event->m_ptContact[0]);
	}

	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);
	CBR(pItem != nullptr, R_OBJECT_NOT_FOUND);

	//TODO stupid hack, can be fixed by incorporating 
	// SenseController into the Interaction Engine
	if (event->m_eventType == InteractionEventType::ELEMENT_INTERSECT_ENDED)
		m_pCurrentItem = nullptr;
	else
		m_pCurrentItem = pItem;

Error:
	return r;
}

CloudController *UIModule::GetCloudController() {
	return m_pDreamOS->GetCloudController();
}
