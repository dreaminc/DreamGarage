#include "DreamUIBar.h"
#include "DreamOS.h"

#include "Cloud/Menu/MenuController.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/Environment/EnvironmentController.h"

#include "InteractionEngine/AnimationItem.h"

#include "UI/UIKeyboard.h"
#include "UI/UIButton.h"
#include "UI/UIMenuItem.h"
#include "UI/UIScrollView.h"
#include "UI/UIMallet.h"

#include "Primitives/font.h"

#include <vector>

#include "Cloud/HTTP/HTTPController.h"

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

RESULT DreamUIBar::SetFont(const std::wstring& strFile) {
	m_pFont = GetDOS()->MakeFont(strFile, true);
	//m_pFont->SetLineHeight()
	return R_PASS;
}

RESULT DreamUIBar::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = GetDOS();

	m_pFont = pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	pDreamOS->AddObjectToUIGraph(GetComposite());

	SetAppName("DreamUIBar");
	SetAppDescription("User Interface");

	m_pDefaultThumbnail = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"thumbnail-default.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pDefaultIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icon-default.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pShareIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));
	m_pMenuItemBg = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"thumbnail-text-background.png", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	CR(GetComposite()->SetVisible(false, false));
	// Initialize the OBB (collisions)
	CR(GetComposite()->InitializeOBB());
	CR(GetDOS()->AddObjectToInteractionGraph(GetComposite()));

	// Initialize UIScrollView
	m_pView = GetComposite()->AddUIView(GetDOS());
	CN(m_pView);

	m_pScrollView = m_pView->AddUIScrollView();
	CN(m_pScrollView);

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());

	pDreamOS->AddAndRegisterInteractionObject(m_pView.get(), InteractionEventType::INTERACTION_EVENT_MENU, m_pView.get());
	CR(m_pView->RegisterSubscriber(UIEventType::UI_MENU, this));

	m_pCloudController = pDreamOS->GetCloudController();
	CN(m_pCloudController);

	m_pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
	CNM(m_pMenuControllerProxy, "Failed to get menu controller proxy");

	CRM(m_pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");

	m_pHTTPControllerProxy = (HTTPControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
	CNM(m_pHTTPControllerProxy, "Failed to get http controller proxy");

	m_pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
	CNM(m_pUserControllerProxy, "Failed to get user controller proxy");

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

	auto pItemsView = m_pScrollView->GetMenuItemsView();

auto pKeyboard = GetDOS()->GetKeyboard();
CN(pKeyboard);

CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
CBM(m_pCloudController->IsEnvironmentConnected(), "Enironment socket not connected");


if (m_pathStack.empty()) {
	m_pMenuControllerProxy->RequestSubMenu("", "", "Share");
	m_pScrollView->GetTitleQuad()->UpdateColorTexture(m_pShareIcon.get());
//	UpdateCompositeWithCameraLook(0.0f, m_menuHeight);
	UpdateCompositeWithHands(m_menuHeight, Axes::Z);
	GetDOS()->GetKeyboard()->UpdateComposite();
}
else {
	m_pathStack.pop();
	if (pKeyboard->IsVisible()) pKeyboard->HideKeyboard();

	if (!m_pathStack.empty()) {
		auto pNode = m_pathStack.top();
		if (pNode->GetTitle() == "Share") {
			m_pScrollView->GetTitleQuad()->UpdateColorTexture(m_pShareIcon.get());
		}
		else {
			auto strURI = pNode->GetThumbnailURL();
			if (strURI != "") {// && pSubMenuNode->MimeTypeFromString(pSubMenuNode->GetMIMEType()) == MenuNode::MimeType::IMAGE_PNG) {
				MenuNode* pTempMenuNode = new MenuNode(pNode->GetNodeType(), pNode->GetPath(), pNode->GetScope(), pNode->GetTitle(), pNode->GetMIMEType());
				pTempMenuNode->SetName("root_menu_title");
				CR(m_pHTTPControllerProxy->RequestFile(strURI, GetStringHeaders(), "", std::bind(&DreamUIBar::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), pTempMenuNode));
			}
		}

		SelectMenuItem();
		m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle());
	}
	else {
		CR(HideMenu());
	}
}
Error:
return r;
}

RESULT DreamUIBar::HandleSelect(void* pContext) {
	RESULT r = R_PASS;

	//	auto pSelected = GetCurrentItem();
	CBR(m_pScrollView->GetState() != ScrollState::SCROLLING, R_PASS);

	UIMenuItem* pSelected = reinterpret_cast<UIMenuItem*>(pContext);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");
	CBR(pSelected, R_OBJECT_NOT_FOUND);
	CBR(m_pMenuNode, R_OBJECT_NOT_FOUND);

	//TODO: Invisible objects potentially should not receive interaction events
	CBR(pSelected->IsVisible(), R_OBJECT_NOT_FOUND);

	CBR(m_menuState == MenuState::NONE, R_PASS);

	//hack - need to make sure the root node is added to the path
	// even though it is not selected through this method
	// ideally, some kind of path is managed in the cloud instead
	if (m_pathStack.empty()) m_pathStack.push(m_pMenuNode);

	auto pMalletHead = pSelected->GetInteractionObject();
	if (pMalletHead != nullptr) {
		if (pMalletHead == m_pLeftMallet->GetMalletHead()) {
			CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
		else if (pMalletHead == m_pRightMallet->GetMalletHead()) {
			CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(1), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
		}
	}

	for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
		if (pSelected->GetName() == pSubMenuNode->GetTitle()) {
			const std::string& strScope = pSubMenuNode->GetScope();
			const std::string& strPath = pSubMenuNode->GetPath();
			const std::string& strTitle = pSubMenuNode->GetTitle();

			auto strURI = pSubMenuNode->GetIconURL();
			if (strURI != "") {
				MenuNode* pTempMenuNode = new MenuNode();
				pTempMenuNode->SetName("root_menu_title");
				CR(m_pHTTPControllerProxy->RequestFile(strURI, GetStringHeaders(), "", std::bind(&DreamUIBar::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), pTempMenuNode));
			}

			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				auto pTempMenuNode = std::shared_ptr<MenuNode>(new MenuNode(pSubMenuNode->GetNodeType(), pSubMenuNode->GetPath(), pSubMenuNode->GetScope(), pSubMenuNode->GetTitle(), pSubMenuNode->GetMIMEType(), pSubMenuNode->GetIconURL(), pSubMenuNode->GetThumbnailURL()));
				m_pathStack.push(pTempMenuNode);
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {
				m_pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
				CNM(m_pEnvironmentControllerProxy, "Failed to get environment controller proxy");

				CRM(m_pEnvironmentControllerProxy->RequestShareAsset(strScope, strPath, strTitle), "Failed to share environment asset");
				//brow
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));
				m_pLeftMallet->Hide();
				m_pRightMallet->Hide();
				m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::ACTION) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));

				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				m_pathStack.push(pSubMenuNode);

				// TODO: This is temporary until we have better IPC
				GetDOS()->GetKeyboard()->SetPath(strPath);
				GetDOS()->GetKeyboard()->SetScope(strScope);

				GetDOS()->GetKeyboard()->ShowKeyboard();
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBar::HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext) {
	RESULT r = R_PASS;

	if (pContext != nullptr) {
		MenuNode* pObj = reinterpret_cast<MenuNode*>(pContext);
		std::string strTitleCopy = pObj->GetTitle();

		m_downloadQueue.push_back(std::pair<std::string, std::shared_ptr<std::vector<uint8_t>>>(strTitleCopy, pBufferVector));

		if (pObj != nullptr) {
			delete pObj;
			pObj = nullptr;
		}
	}

//Error:
	return r;
}

RESULT DreamUIBar::UpdateMenu(void *pContext) {
	RESULT r = R_PASS;

	DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
	CN(pDreamUIBar);

	GetComposite()->SetVisible(true, false);
	m_pScrollView->SetScrollVisible(true);
	m_pScrollView->SetPosition(m_ptMenuShowOffset);
	m_pScrollView->ShowTitle();

	m_pLeftMallet->Show();
	m_pRightMallet->Show();
	m_menuState = MenuState::ANIMATING;
Error:
	return r;
}

RESULT DreamUIBar::Update(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();

	// Copy into temp vector
	std::vector<std::pair<std::string, std::shared_ptr<std::vector<uint8_t>>>> downloadQueueCopy = m_downloadQueue;
	m_downloadQueue.clear();

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

	//while (!m_downloadQueue.empty()) {
	for(auto &pQueueObj : downloadQueueCopy) {
		auto pMenuNodeTitle = pQueueObj.first;
		auto pBufferVector = pQueueObj.second;
		auto pChildren = m_pScrollView->GetMenuItemsView()->GetChildren();

		texture *pTexture = nullptr;

		CN(pBufferVector);
		uint8_t* pBuffer = &(pBufferVector->operator[](0));
		size_t pBuffer_n = pBufferVector->size();

		pTexture = GetDOS()->MakeTextureFromFileBuffer(pBuffer, pBuffer_n, texture::TEXTURE_TYPE::TEXTURE_COLOR);
		CN(pTexture);

		for (auto& pChild : pChildren) {
			auto pObj = dynamic_cast<UIMenuItem*>(pChild.get());
			if (pObj != nullptr && pMenuNodeTitle.size() > 0 && pObj->GetName() == pMenuNodeTitle) {
				pObj->GetSurface()->UpdateColorTexture(pTexture);
			}
		}

		if (pMenuNodeTitle == "root_menu_title") {
			m_pScrollView->GetTitleQuad()->UpdateColorTexture(pTexture);
			//TODO: temporary, should be revisited during menu cleanup
			GetDOS()->GetKeyboard()->UpdateTitle(pTexture, "Website");
		}
		
		if (pBufferVector != nullptr) {
			pBufferVector = nullptr;
		}

		//m_downloadQueue.pop();
	}

	if (m_pMenuNode && m_pMenuNode->CheckAndCleanDirty()) {

		std::vector<std::shared_ptr<UIButton>> pButtons;

		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			auto pButton = m_pView->MakeUIMenuItem();
			CN(pButton);

			auto iconFormat = IconFormat();
			iconFormat.pTexture = m_pDefaultThumbnail.get();

			auto labelFormat = LabelFormat();
			labelFormat.strLabel = pSubMenuNode->GetTitle();
			labelFormat.pFont = m_pFont;
			labelFormat.pBgTexture = m_pMenuItemBg.get();

			pButton->Update(iconFormat, labelFormat);

			CR(pButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
				std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

			pButtons.emplace_back(pButton);
		}

		m_pScrollView->GetTitleText()->SetText(m_pMenuNode->GetTitle());
		
		CR(m_pScrollView->UpdateMenuButtons(pButtons));



		CR(ShowMenu(std::bind(&DreamUIBar::UpdateMenu, this, std::placeholders::_1), 
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));
	}

Error:
	return r;
}

RESULT DreamUIBar::SelectMenuItem(UIButton *pPushButton, std::function<RESULT(void*)> fnStartCallback, std::function<RESULT(void*)> fnEndCallback) {
	RESULT r = R_PASS;

	CR(m_pScrollView->HideAllButtons(pPushButton));

Error:
	return r;
}

RESULT DreamUIBar::SetMenuStateAnimated(void *pContext) {
	RESULT r = R_PASS;
	m_menuState = MenuState::ANIMATING;
	return r;
}

RESULT DreamUIBar::ClearMenuState(void* pContext) {
	RESULT r = R_PASS;
	m_menuState = MenuState::NONE;
	return r;
}

RESULT DreamUIBar::HideMenu(std::function<RESULT(void*)> fnStartCallback) {
	RESULT r = R_PASS;

	composite *pComposite = m_pScrollView.get();
	m_menuState = MenuState::ANIMATING;
	m_pLeftMallet->Hide();
	m_pRightMallet->Hide();

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
		CN(pDreamUIBar);

		GetComposite()->SetVisible(false, false);
		m_menuState = MenuState::NONE;
	Error:
		return r;
	};
//*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pComposite,
		//pComposite->GetPosition() - (point(0.0f, 0.0f, m_scrollViewDepth) - m_ptMenuShowOffset),
		pComposite->GetPosition(),
		pComposite->GetOrientation(),
		pComposite->GetScale(),
		m_animationDuration,
		AnimationCurveType::EASE_OUT_QUART, // may want to try ease_in here
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
Error:
	return r;
}

RESULT DreamUIBar::ShowMenu(std::function<RESULT(void*)> fnStartCallback, std::function<RESULT(void*)> fnEndCallback) {
	RESULT r = R_PASS;

	composite *pComposite = m_pScrollView.get();
	//pComposite->SetPosition(point(0.0f, 0.0f, m_scrollViewDepth));
	pComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
//*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pComposite,
		pComposite->GetPosition(),
		pComposite->GetOrientation(),
		pComposite->GetScale(),
		m_animationDuration,
		//AnimationCurveType::EASE_OUT_BACK,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
//*/

Error:
	return r;
}

RESULT DreamUIBar::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED;
}

RESULT DreamUIBar::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CNR(pMenuNode, R_OBJECT_NOT_FOUND);

	if (pMenuNode->NumSubMenuNodes() > 0) {

		m_pMenuNode = pMenuNode;
		if (m_pathStack.empty()) m_pathStack.push(m_pMenuNode);
		m_pMenuNode->SetDirty();

		auto strHeaders = GetStringHeaders();

		for (auto& pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {

			auto strURI = pSubMenuNode->GetThumbnailURL();
			if (strURI != "") {// && pSubMenuNode->MimeTypeFromString(pSubMenuNode->GetMIMEType()) == MenuNode::MimeType::IMAGE_PNG) {
				MenuNode* pTempMenuNode = new MenuNode(pSubMenuNode->GetNodeType(), pSubMenuNode->GetPath(), pSubMenuNode->GetScope(), pSubMenuNode->GetTitle(), pSubMenuNode->GetMIMEType());
				CR(m_pHTTPControllerProxy->RequestFile(strURI, strHeaders, "", std::bind(&DreamUIBar::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), pTempMenuNode));
			}
		}

		auto strURI = m_pMenuNode->GetIconURL();
		if (strURI != "") {
			MenuNode* pTempMenuNode = new MenuNode();
			pTempMenuNode->SetName("root_menu_title");
			CR(m_pHTTPControllerProxy->RequestFile(strURI, strHeaders, "", std::bind(&DreamUIBar::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), m_pMenuNode.get()));
		}
	}

Error:
	return r;
}

std::vector<std::string> DreamUIBar::GetStringHeaders() {

	std::string strAuthorizationToken = "Authorization: Token " + m_pUserControllerProxy->GetUserToken();

	auto strHeaders = HTTPController::ContentHttp();
	strHeaders.push_back(strAuthorizationToken);

	return strHeaders;
}

RESULT DreamUIBar::Notify(UIEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->m_eventType) {
	case (UIEventType::UI_MENU): {
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
