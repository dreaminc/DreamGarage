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

	CN(m_pDreamOS);
	m_pCompositeContext = m_pDreamOS->AddComposite();
	CN(m_pCompositeContext);

	CR(m_pCompositeContext->InitializeOBB());

Error:
	return r;
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
