#include "DreamUIBar.h"
#include "DreamOS.h"

#include "Cloud/Menu/MenuNode.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIKeyboard.h"
#include "UI/UIButton.h"
#include "UI/UIMenuItem.h"

#include <vector>

DreamUIBar::DreamUIBar(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUIBar>(pDreamOS, pContext)//,
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

//Generating the font texture is expensive, only use this function during initialization
RESULT DreamUIBar::SetFont(const std::wstring& strFile) {
//	m_labelFormat.pFont = std::make_shared<Font>(strFile, GetComposite(), true);
	m_pFont = std::make_shared<Font>(strFile, GetComposite(), true);
	return R_PASS;
}

RESULT DreamUIBar::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = GetDOS();

	SetAppName("DreamUIBar");
	SetAppDescription("User Interface");

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

	CR(GetComposite()->SetVisible(false));
	// Initialize the OBB (collisions)
	CR(GetComposite()->InitializeOBB());
	CR(GetDOS()->AddObjectToInteractionGraph(GetComposite()));

	// Initialize UIScrollView
	m_pView = GetComposite()->AddUIView();
	CN(m_pView);
	m_pView->SetPosition(point(0.0f, 0.0f, 1.0f));

	m_pScrollView = m_pView->AddUIScrollView();
	CN(m_pScrollView);
	CR(m_pScrollView->InitializeWithDOS(pDreamOS));

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());

	pDreamOS->AddAndRegisterInteractionObject(m_pView.get(), InteractionEventType::INTERACTION_EVENT_MENU, m_pView.get());
	CR(m_pView->RegisterSubscriber(UIEventType::UI_MENU, this));
	
/*
	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_BEGAN,
		std::bind(&DreamUIBar::HandleTouchStart, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::ELEMENT_INTERSECT_ENDED,
		std::bind(&DreamUIBar::HandleTouchEnd, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::INTERACTION_EVENT_SELECT_UP,
		std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

	CR(RegisterEvent(InteractionEventType::INTERACTION_EVENT_MENU,
		std::bind(&DreamUIBar::HandleMenuUp, this, std::placeholders::_1)));
	//*/


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

	return r;
}

RESULT DreamUIBar::HandleTouchMove(void* pContext) {
	return R_PASS;
}

RESULT DreamUIBar::HandleTouchEnd(void* pContext) {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamUIBar::HandleMenuUp(void* pContext) {
	RESULT r = R_PASS;

	auto pKeyboard = GetDOS()->GetKeyboard();
	CN(pKeyboard);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Enironment socket not connected");

	if (m_pathStack.empty()) {
		m_pMenuControllerProxy->RequestSubMenu("", "", "Menu");
		GetComposite()->SetVisible(!GetComposite()->IsVisible());
		//move to anim
		UpdateCompositeWithCameraLook(0.0f, -1.0f);
	}
	else {
		m_pathStack.pop();
		if (pKeyboard->IsVisible()) pKeyboard->HideKeyboard();

		if (!m_pathStack.empty()) {
			auto pNode = m_pathStack.top();
			m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle());
			HideMenu();
		}
		else {
			GetComposite()->SetVisible(!GetComposite()->IsVisible());
			UpdateCompositeWithCameraLook(0.0f, -1.0f);
		}
	}
Error:
	return r;
}

RESULT DreamUIBar::HandleSelect(void* pContext) {
	RESULT r = R_PASS;

//	auto pSelected = GetCurrentItem();
	CBR(m_pScrollView->GetState() != MenuState::SCROLLING, R_PASS);

	UIMenuItem* pSelected = reinterpret_cast<UIMenuItem*>(pContext);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");
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
				HideMenu();
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				m_pathStack.push(pSubMenuNode);
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
				auto m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestShareAsset(strScope, strPath, strTitle), "Failed to share environment asset");
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::ACTION) {
				HideMenu();
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				m_pathStack.push(pSubMenuNode);
				GetDOS()->GetKeyboard()->ShowKeyboard();
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBar::UpdateMenu(void *pContext) {
	RESULT r = R_PASS;
	DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
	CN(pDreamUIBar);
	//CR(pDreamUIBar->UpdateUILayers());
Error:
	return r;
}

RESULT DreamUIBar::Update(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();

	RotationMatrix qOffset = RotationMatrix();
	hand *pHand = pDreamOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
	CN(pHand);
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	pHand = pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);
	CN(pHand);

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

	CR(m_pScrollView->Update());

	if (m_pMenuNode && m_pMenuNode->CheckAndCleanDirty()) {
	
		std::vector<std::shared_ptr<UIButton>> pButtons; 

		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			auto pButton = m_pView->MakeUIMenuItem();
			CN(pButton);

			auto iconFormat = IconFormat();
			iconFormat.pTexture = m_images[pSubMenuNode->MimeTypeFromString(pSubMenuNode->GetMIMEType())];

			auto labelFormat = LabelFormat();
			labelFormat.strLabel = pSubMenuNode->GetTitle();
			labelFormat.pFont = m_pFont;

			pButton->Update(iconFormat, labelFormat);

			CR(pButton->RegisterEvent(UI_SELECT_ENDED,
				std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

			pButtons.emplace_back(pButton);
		}

		m_pScrollView->UpdateMenuButtons(pButtons);
	/**
		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			info.labels.emplace_back(pSubMenuNode->GetTitle());
			info.icons.emplace_back(m_images[pSubMenuNode->MimeTypeFromString(pSubMenuNode->GetMIMEType())]);
		}

		titleInfo.labels = { m_pMenuNode->GetTitle() };
		titleInfo.icons = { m_images[MenuNode::MimeType::FOLDER] };
//*/
		//TODO: There are several RenderToTexture calls and object creates
		// that cause a brief timing delay

		//CR(SetUpdateParams(info, titleInfo));
		CR(ShowMenu(std::bind(&DreamUIBar::UpdateMenu, this, std::placeholders::_1), nullptr));
		//CR(ShowMenu(nullptr, nullptr));
	}

Error:
	return r;
}

RESULT DreamUIBar::HideMenu(std::function<RESULT(void*)> fnStartCallback, std::function<RESULT(void*)> fnEndCallback) {
	RESULT r = R_PASS;

	//composite *pComposite = GetComposite();
	m_pLeftMallet->Hide();
	m_pRightMallet->Hide();
/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pComposite,
		pComposite->GetPosition(),
		m_qMenuOrientation * quaternion::MakeQuaternionWithEuler(0.0f, (float)(M_PI_2), 0.0f),
		pComposite->GetScale(),
		0.5f,
		AnimationCurveType::EASE_OUT_QUART, // may want to try ease_in here
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
//Error:
	return r;
}

RESULT DreamUIBar::ShowMenu(std::function<RESULT(void*)> fnStartCallback, std::function<RESULT(void*)> fnEndCallback) {
	RESULT r = R_PASS;

	//composite *pComposite = GetComposite();
	m_pLeftMallet->Show();
	m_pRightMallet->Show();
/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pComposite,
		pComposite->GetPosition(),
		m_qMenuOrientation,
		pComposite->GetScale(),
		0.5f,
		AnimationCurveType::EASE_OUT_QUART,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/

//Error:
	return r;
}

RESULT DreamUIBar::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUIBar::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	if (pMenuNode->NumSubMenuNodes() > 0) {
		auto pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
		CNM(pMenuControllerProxy, "Failed to get menu controller proxy");
		m_pMenuNode = pMenuNode;
		if (m_pathStack.empty()) m_pathStack.push(m_pMenuNode);
		m_pMenuNode->SetDirty();
	}

Error:
	return r;
}

RESULT DreamUIBar::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->m_eventType) {
	case (UI_MENU): {
		CR(HandleMenuUp(pEvent->m_pObj));
	} break;
	}

Error:
	return r;
}

DreamUIBar* DreamUIBar::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUIBar *pDreamApp = new DreamUIBar(pDreamOS, pContext);
	return pDreamApp;
}