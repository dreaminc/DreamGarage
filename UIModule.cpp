#include "UIModule.h"

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
	std::shared_ptr<UIMenuLayer> pUIMenuLayer = nullptr;
	CN(pComposite);

	pUIMenuLayer = std::make_shared<UIMenuLayer>(pComposite);
	m_layers.emplace_back();

Error:
	return pUIMenuLayer;
}