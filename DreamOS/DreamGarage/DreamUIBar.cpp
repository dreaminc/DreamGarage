#include "DreamUIBar.h"
#include "DreamOS.h"

#include "Cloud/Menu/MenuNode.h"

DreamUIBar::DreamUIBar(DreamOS *pDreamOS, IconFormat& iconFormat, LabelFormat& labelFormat, UIBarFormat& barFormat) :
	UIBar(pDreamOS, iconFormat, labelFormat, barFormat)
{
	RESULT r = R_PASS;

	CR(DreamUIBar::Initialize());

//Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

// TODO: Will be implemented or moved once registration architecture is fleshed out
RESULT DreamUIBar::RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback) {
	m_callbacks[type] = fnCallback;
	return R_PASS;
}

RESULT DreamUIBar::Initialize() {
	RESULT r = R_PASS;

	CR(SetVisible(false));

	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_BEGAN,
		std::bind(&DreamUIBar::HandleTouchStart, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_ENDED,
		std::bind(&DreamUIBar::HandleTouchEnd, this, std::placeholders::_1)));

	m_pCloudController = GetCloudController();

	MenuControllerProxy *pMenuControllerProxy = nullptr;
	CN(m_pCloudController);
	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Enironment socket not connected");

	m_pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
	CNM(m_pMenuControllerProxy, "Failed to get menu controller proxy");

	CRM(m_pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");

Error:
	return r;
}

RESULT DreamUIBar::HandleTouchStart(void* pContext) {
	RESULT r = R_PASS;

	UIMenuItem* pItem = reinterpret_cast<UIMenuItem*>(pContext);
	CN(pItem);
	pItem->GetQuad()->Scale(m_barFormat.itemScaleSelected);

Error:
	return r;
}

RESULT DreamUIBar::HandleTouchMove(void* pContext) {
	return R_PASS;
}

RESULT DreamUIBar::HandleTouchEnd(void* pContext) {
	RESULT r = R_PASS;

	UIMenuItem* pItem = reinterpret_cast<UIMenuItem*>(pContext);
	CN(pItem);
	pItem->GetQuad()->SetScale(m_barFormat.itemScale);

Error:
	return r;
}

RESULT DreamUIBar::HandleMenuUp() {
	RESULT r = R_PASS;

	if (m_path.empty()) {
		m_pMenuControllerProxy->RequestSubMenu();
		ToggleVisible();
	}
	else {
		m_path.pop();
		if (!m_path.empty()) {
			auto pNode = m_path.top();
			m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath());
		}
		else {
			ToggleVisible();
		}
	}

	return r;
}

RESULT DreamUIBar::HandleTriggerUp() {
	RESULT r = R_PASS;
	
	auto pSelected = GetCurrentItem();
	CBR(pSelected, R_OBJECT_NOT_FOUND);
	CBR(m_pMenuNode, R_OBJECT_NOT_FOUND);

	//hack
	if (m_path.empty()) m_path.push(m_pMenuNode);

	for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
		const std::string& s1 = pSelected->GetName();
		const std::string& s2 = pSubMenuNode->GetTitle();
		if (pSelected->GetName() == pSubMenuNode->GetTitle()) {
			m_pMenuControllerProxy->RequestSubMenu(pSubMenuNode->GetScope(), pSubMenuNode->GetPath());
			m_path.push(pSubMenuNode);
		}
	}

Error:
	return r;
}

RESULT DreamUIBar::Update() {
	RESULT r = R_PASS;
	UILayerInfo info;
	CR(UpdateInteractionPrimitive(GetHandRay()));

	if (m_pMenuNode && m_pMenuNode->CheckAndCleanDirty()) {
		
		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			info.labels.emplace_back(pSubMenuNode->GetTitle());
			info.icons.emplace_back(m_pIconTexture);
		}
		info.labels.emplace_back(m_pMenuNode->GetTitle());
		info.icons.emplace_back(m_pIconTexture);
		UpdateCurrentUILayer(info);
	}

Error:
	return r;
}

RESULT DreamUIBar::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	UIModule::Notify(event);

	std::function<RESULT(void*)> fnCallback;
	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);

	CBR(m_callbacks.count(event->m_eventType) > 0, R_OBJECT_NOT_FOUND)
	CBR(pItem != nullptr, R_OBJECT_NOT_FOUND);

	fnCallback = m_callbacks[event->m_eventType];

	fnCallback(pItem.get());

Error:
	return r;
}

RESULT DreamUIBar::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");
		m_pMenuNode = pMenuNode;
		m_pMenuNode->SetDirty();
	}

Error:
	return r;
}