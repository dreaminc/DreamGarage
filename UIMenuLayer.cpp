#include "UIMenuLayer.h"

UIMenuLayer::UIMenuLayer(composite* pContext) :
	m_pContext(pContext)
{
	// empty
}

UIMenuLayer::~UIMenuLayer() 
{
	// empty
}

std::shared_ptr<UIMenuItem> UIMenuLayer::CreateMenuItem()
{
	std::shared_ptr<UIMenuItem> pMenuItem = std::make_shared<UIMenuItem>(UIMenuItem(m_pContext->AddComposite().get()));
	m_menuItems.emplace_back(pMenuItem);
	return pMenuItem;
}

RESULT UIMenuLayer::Clear()
{
	m_menuItems.clear();
	return R_PASS;
}

std::vector<std::shared_ptr<UIMenuItem>> UIMenuLayer::GetMenuItems() {
	return m_menuItems;
}
