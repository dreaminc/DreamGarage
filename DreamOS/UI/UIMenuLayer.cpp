#include "UIMenuLayer.h"

UIMenuLayer::UIMenuLayer(composite* pParentContext) :
	m_pParentContext(pParentContext),
	m_pContextComposite(nullptr)
{
	RESULT r = R_PASS;

	CR(Initialize());

// Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

UIMenuLayer::~UIMenuLayer() {
	// empty
}

RESULT UIMenuLayer::Initialize() {
	RESULT r = R_PASS;

	CN(m_pParentContext);
	m_pContextComposite = m_pParentContext->AddComposite();
	CN(m_pContextComposite);

	CR(m_pContextComposite->InitializeOBB());

Error:
	return r;
}

std::shared_ptr<UIMenuItem> UIMenuLayer::CreateMenuItem() {
	RESULT r = R_PASS;

	std::shared_ptr<UIMenuItem> pMenuItem = std::make_shared<UIMenuItem>(m_pContextComposite);
	CN(pMenuItem);
	CV(pMenuItem);

	m_menuItems.emplace_back(pMenuItem);
	
Error:
	return pMenuItem;
}

RESULT UIMenuLayer::Clear() {
	m_menuItems.clear();
	return R_PASS;
}

std::vector<std::shared_ptr<UIMenuItem>> UIMenuLayer::GetMenuItems() {
	return m_menuItems;
}

std::shared_ptr<UIMenuItem> UIMenuLayer::GetMenuItem(VirtualObj *pObj) {
	for (auto& pItem : m_menuItems) {
		if (pItem->Contains(pObj)) {
			return pItem;
		}
	}
	return nullptr;
}
