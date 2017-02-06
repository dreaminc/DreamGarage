#include "UIMenuLayer.h"

UIMenuLayer::UIMenuLayer(composite* c) :
	m_context(c)
{
	// empty
}

UIMenuLayer::~UIMenuLayer() 
{
	// empty
}

std::shared_ptr<UIMenuItem> UIMenuLayer::CreateMenuItem()
{
	std::shared_ptr<UIMenuItem> pMenuItem = std::make_shared<UIMenuItem>(UIMenuItem(m_context));
	m_pMenuItems.emplace_back(pMenuItem);
	return pMenuItem;
}

RESULT UIMenuLayer::Clear()
{
	m_pMenuItems.clear();
	return R_PASS;
}

std::vector<std::shared_ptr<UIMenuItem>> UIMenuLayer::GetMenuItems() {
	return m_pMenuItems;
}
