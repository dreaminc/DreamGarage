#include "UIModule.h"
#include "Primitives/camera.h"

#include "DreamOS.h"

UIModule::UIModule() {

}

UIModule::~UIModule() {
	// empty
}

RESULT UIModule::Initialize(composite *pComposite) {
	RESULT r = R_PASS;

	m_pCompositeContext = pComposite;

//Error:
	return r;
}

ray UIModule::GetHandRay(hand* pHand) {
	
	RESULT r = R_PASS;

	ray rCast;

	CBR(IsVisible(), R_SKIPPED); 
	CBR(pHand != nullptr, R_OBJECT_NOT_FOUND)
	{
		point ptHand = pHand->GetPosition();

		//GetLookVector
		quaternion qHand = pHand->GetHandState().qOrientation;
		qHand.Normalize();

		//TODO: investigate how to properly get look vector for controllers
		//vector vHandLook = qHand.RotateVector(vector(0.0f, 0.0f, -1.0f)).Normal();

		vector vHandLook = RotationMatrix(qHand) * vector(0.0f, 0.0f, -1.0f);
		vHandLook.Normalize();

		rCast = ray(ptHand, vHandLook);
	}
	return rCast;

Error:
	return ray(point(0.0f, 0.0f, 0.0f), vector(0.0f, 0.0f, 0.0f));
}

std::shared_ptr<UIMenuLayer> UIModule::CreateMenuLayer() {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuLayer> pUIMenuLayer = std::make_shared<UIMenuLayer>(m_pCompositeContext);
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