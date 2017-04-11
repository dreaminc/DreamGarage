#include "DreamUIBar.h"
#include "DreamOS.h"

#include "Cloud/Menu/MenuNode.h"
#include "InteractionEngine/AnimationItem.h"

DreamUIBar::DreamUIBar(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUIBar>(pDreamOS, pContext),
	UIBar()
{
	// empty
}

DreamUIBar::~DreamUIBar() 
{
	// empty
}

RESULT DreamUIBar::RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback) {
	m_callbacks[type] = fnCallback;
	return R_PASS;
}

RESULT DreamUIBar::SetParams(const IconFormat& iconFormat, const LabelFormat& labelFormat, const RadialLayerFormat& menuFormat, const RadialLayerFormat& titleFormat) {
	return UIBar::SetParams(iconFormat, labelFormat, menuFormat, titleFormat);
}

RESULT DreamUIBar::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = GetDOS();

	SetAppName("DreamUIBar");
	SetAppDescription("User Interface");

	UIModule::Initialize(GetComposite());

	std::shared_ptr<texture> pJPG = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icons_600\\icon_jpg_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_images[MenuNode::MimeType::IMAGE_JPG] = pJPG;

	std::shared_ptr<texture> pPNG = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icons_600\\icon_png_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_images[MenuNode::MimeType::IMAGE_PNG] = pPNG;

	std::shared_ptr<texture> pGIF = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icons_600\\icon_gif_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_images[MenuNode::MimeType::IMAGE_GIF] = pGIF;

	std::shared_ptr<texture> pBMP = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icons_600\\icon_bmp_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_images[MenuNode::MimeType::IMAGE_BMP] = pBMP;

	std::shared_ptr<texture> pFolder = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icons_600\\icon_folder_600.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_images[MenuNode::MimeType::FOLDER] = pFolder;

	CR(SetVisible(false));
	// Initialize the OBB (collisions)
	CR(GetComposite()->InitializeOBB());
	CR(GetDOS()->AddInteractionObject(GetComposite()));

	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(pDreamOS->RegisterEventSubscriber((InteractionEventType)(i), this));
	}

	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_BEGAN,
		std::bind(&DreamUIBar::HandleTouchStart, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_ENDED,
		std::bind(&DreamUIBar::HandleTouchEnd, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::INTERACTION_EVENT_SELECT,
		std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::INTERACTION_EVENT_MENU,
		std::bind(&DreamUIBar::HandleMenuUp, this, std::placeholders::_1)));

	m_pCloudController = pDreamOS->GetCloudController();

	MenuControllerProxy *pMenuControllerProxy = nullptr;
	CN(m_pCloudController);

	m_pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
	CNM(m_pMenuControllerProxy, "Failed to get menu controller proxy");

	CRM(m_pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
Error:
	return r;
}

RESULT DreamUIBar::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUIBar::HandleTouchStart(void* pContext) {
	RESULT r = R_PASS;

	UIMenuItem* pItem = reinterpret_cast<UIMenuItem*>(pContext);
	CN(pItem);
	if (GetCurrentLayer()->ContainsMenuItem(pItem)) {
		VirtualObj *pObj = pItem->GetQuad().get();
		GetDOS()->GetInteractionEngineProxy()->
			PushAnimationItem(pObj,
				pObj->GetPosition(),
				pObj->GetOrientation(),
				vector(m_menuFormat.itemScaleSelected),
				0.1,
				AnimationCurveType::EASE_OUT_QUAD,
				AnimationFlags());
	}

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
	if (GetCurrentLayer()->ContainsMenuItem(pItem)) {
		VirtualObj *pObj = pItem->GetQuad().get();
		GetDOS()->GetInteractionEngineProxy()->
			PushAnimationItem(pObj,
				pObj->GetPosition(),
				pObj->GetOrientation(),
				vector(m_menuFormat.itemScale),
				0.1,
				AnimationCurveType::EASE_OUT_QUAD,
				AnimationFlags());
	}

Error:
	return r;
}

RESULT DreamUIBar::HandleMenuUp(void* pContext) {
	RESULT r = R_PASS;
	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Enironment socket not connected");

	if (m_pathStack.empty()) {
		m_pMenuControllerProxy->RequestSubMenu("", "", "Menu");
		ToggleVisible();
	}
	else {
		m_pathStack.pop();
		if (!m_pathStack.empty()) {
			auto pNode = m_pathStack.top();
			m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle());
		}
		else {
			ToggleVisible();
		}
	}
Error:
	return r;
}

RESULT DreamUIBar::HandleSelect(void* pContext) {
	RESULT r = R_PASS;

//	auto pSelected = GetCurrentItem();

	UIMenuItem* pSelected = reinterpret_cast<UIMenuItem*>(pContext);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Enironment socket not connected");
	CBR(pSelected, R_OBJECT_NOT_FOUND);
	CBR(m_pMenuNode, R_OBJECT_NOT_FOUND);

	//hack - need to make sure the root node is added to the path
	// even though it is not selected through this method
	// ideally, some kind of path is managed in the cloud instead
	if (m_pathStack.empty()) m_pathStack.push(m_pMenuNode);

	for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
		if (pSelected->GetName() == pSubMenuNode->GetTitle()) {
			const std::string& strScope = pSubMenuNode->GetScope();
			const std::string& strPath = pSubMenuNode->GetPath();
			const std::string& strTitle = pSubMenuNode->GetTitle();

			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				m_pathStack.push(pSubMenuNode);
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
				auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestShareAsset(strScope, strPath, strTitle), "Failed to share environment asset");
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBar::Update(void *pContext) {
	RESULT r = R_PASS;
	UILayerInfo info;
	UILayerInfo titleInfo;
	DreamOS *pDreamOS = GetDOS();
	CR(pDreamOS->UpdateInteractionPrimitive(GetHandRay(pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT))));

	if (m_pMenuNode && m_pMenuNode->CheckAndCleanDirty()) {
		
		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			info.labels.emplace_back(pSubMenuNode->GetTitle());
			info.icons.emplace_back(m_images[pSubMenuNode->MimeTypeFromString(pSubMenuNode->GetMIMEType())]);
		}

		titleInfo.labels = { m_pMenuNode->GetTitle() };
		titleInfo.icons = { m_images[MenuNode::MimeType::FOLDER] };

		//TODO: There are several RenderToTexture calls and object creates
		// that cause a brief timing delay

		SetUpdateParams(info, titleInfo);
		auto fnCallback = [&](void *pContext) {
			RESULT r = R_PASS;
			DreamUIBar *pUIBar = reinterpret_cast<DreamUIBar*>(pContext);
			CN(pUIBar);
			CR(pUIBar->UpdateUILayers());
			//composite *pC = pUIBar->UIModule::m_pCompositeContext;
			composite *pC = pUIBar->DreamApp::GetComposite();
			pC->SetOrientation(pC->GetOrientation() * quaternion::MakeQuaternionWithEuler(0.0f, -(float)(M_PI), 0.0f));
		Error:
			return r;
		};

		pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			GetComposite(),
			GetComposite()->GetPosition(),
			GetComposite()->GetOrientation() * quaternion::MakeQuaternionWithEuler(0.0f, (float)(M_PI_2), 0.0f),
			GetComposite()->GetScale(),
			0.5f,
			AnimationCurveType::EASE_OUT_QUART, // may want to try ease_in here
			AnimationFlags(),
			fnCallback,
			this
		);

		quaternion q = GetComposite()->GetOrientation();
		pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
			GetComposite(),
			GetComposite()->GetPosition(),
			q,
			GetComposite()->GetScale(),
			0.5f,
			AnimationCurveType::EASE_OUT_QUART,
			AnimationFlags()
		);
	}

Error:
	return r;
}

RESULT DreamUIBar::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUIBar::Notify(InteractionObjectEvent *event) {
	RESULT r = R_PASS;

	std::function<RESULT(void*)> fnCallback;
	std::shared_ptr<UIMenuItem> pItem = GetMenuItem(event->m_pObject);

	CBR(m_callbacks.count(event->m_eventType) > 0, R_OBJECT_NOT_FOUND);
	bool menuUp = (event->m_eventType == InteractionEventType::INTERACTION_EVENT_MENU);
	CBR(menuUp || (pItem != nullptr), R_OBJECT_NOT_FOUND);

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

DreamUIBar* DreamUIBar::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUIBar *pDreamApp = new DreamUIBar(pDreamOS, pContext);
	return pDreamApp;
}
