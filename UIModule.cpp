#include "UIModule.h"
#include "Primitives/camera.h"

UIModule::UIModule(composite* pComposite) :
	m_pContext(pComposite)
{
	// empty
}
UIModule::~UIModule()
{
	// empty
}

std::shared_ptr<UIMenuLayer> UIModule::CreateMenuLayer() {
	RESULT r = R_PASS;

	composite* pComposite = m_pContext->AddComposite().get();
	std::shared_ptr<UIMenuLayer> pUIMenuLayer;
	CN(pComposite);

	pUIMenuLayer = std::make_shared<UIMenuLayer>(pComposite);
	m_layers.emplace_back(pUIMenuLayer);

	m_currentUILayer = pUIMenuLayer;

Error:
	return pUIMenuLayer;
}

RESULT UIModule::ToggleVisible() {

	RESULT r = R_PASS;

	m_pContext->SetVisible(!m_pContext->IsVisible());

	// if the context has become visible, reset its position to match the camera
	if (m_pContext->IsVisible()) {
		quaternion q = m_pContext->GetCamera()->GetOrientation();
		m_headRotationYDeg = q.ProjectedYRotationDeg();
		quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, m_headRotationYDeg * M_PI / 180.0f, 0.0f);

		m_pContext->SetPosition(m_pContext->GetCamera()->GetPosition());
		m_pContext->SetOrientation(q2);
	}

	return r;
}
