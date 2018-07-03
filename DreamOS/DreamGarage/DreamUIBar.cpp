#include "DreamUIBar.h"
#include "DreamOS.h"

#include "Cloud/Menu/MenuController.h"
#include "Cloud/Menu/MenuNode.h"
#include "Cloud/Environment/EnvironmentController.h"

#include "InteractionEngine/AnimationItem.h"

#include "UI/UIKeyboard.h"
#include "UI/UIButton.h"
#include "UI/UIMenuItem.h"
#include "UI/UISpatialScrollView.h"
#include "UI/UIMallet.h"

#include "DreamControlView/DreamControlView.h"
#include "DreamUserControlArea/DreamUserControlArea.h"
#include "DreamBrowser.h"

#include "Primitives/font.h"

#include <vector>

#include "Cloud/HTTP/HTTPController.h"

#include "HAL/UIStageProgram.h"

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

	SetAppName("DreamUIBar");
	SetAppDescription("User Interface");	

	m_pDefaultThumbnail = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"thumbnail-default.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pDefaultIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icon-default.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pShareIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"icon-share.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	m_pMenuItemBg = std::shared_ptr<texture>(pDreamOS->MakeTexture(L"thumbnail-text-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

	//TODO: could move this logic up to DreamUserObserver, and then only 
	if (GetDOS()->GetHMD() != nullptr) {
		switch (GetDOS()->GetHMD()->GetDeviceType()) {
		case HMDDeviceType::OCULUS: {
			m_pOverlayLeft = GetDOS()->MakeTexture(L"left-controller-overlay-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(L"right-controller-overlay-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		case HMDDeviceType::VIVE: {
			m_pOverlayLeft = GetDOS()->MakeTexture(L"vive-controller-overlay-left-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(L"vive-controller-overlay-right-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		case HMDDeviceType::META: {
			m_pOverlayLeft = GetDOS()->MakeTexture(L"vive-controller-overlay-left-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(L"vive-controller-overlay-right-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	CR(GetComposite()->SetVisible(false, false));
	// Initialize the OBB (collisions)
	CR(GetComposite()->InitializeOBB());
	CR(GetDOS()->AddObjectToInteractionGraph(GetComposite()));

	// Initialize UISpatialScrollView
	m_pView = GetComposite()->AddUIView(GetDOS());
	m_pView->SetPosition(0.0f, 0.0f, m_menuDepth);
	CN(m_pView);

	m_pScrollView = m_pView->AddUISpatialScrollView();
	//m_pScrollView->SetPosition(0.0f, 0.0f, m_menuDepth);
	CN(m_pScrollView);

	pDreamOS->AddAndRegisterInteractionObject(m_pView.get(), InteractionEventType::INTERACTION_EVENT_MENU, m_pView.get());
	CR(m_pView->RegisterSubscriber(UIEventType::UI_MENU, this));

Error:
	return r;
}

RESULT DreamUIBar::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUIBar::HandleTouchStart(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<quad> pSurface = nullptr;
	vector vSurface;
	vector vRotation;
	quaternion qRotation;
	quaternion qSurface;

	CNR(pButtonContext, R_SKIPPED);
	CBR(m_menuState != MenuState::ANIMATING, R_SKIPPED);

	UIMenuItem* pSelected;
	pSelected = reinterpret_cast<UIMenuItem*>(pButtonContext);
	pSurface = pSelected->GetSurface();

	//vector for captured object movement
	qSurface = pSelected->GetOrientation() * (pSurface->GetOrientation());
	qSurface.Reverse();
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);
	//vector for captured object collisions
	qRotation = pSurface->GetOrientation(true);
	qRotation.Reverse();
	vRotation = qRotation.RotateVector(pSurface->GetNormal() * -1.0f);
	CBR(m_pScrollView->IsVisible(), R_SKIPPED);
	CBR(m_pScrollView->GetState() != ScrollState::SCROLLING, R_PASS);

	//don't capture buttons that are out of view
	CBR(m_pScrollView->IsCapturable(pButtonContext), R_OBJECT_NOT_FOUND);

	//DreamOS *pDreamOS = GetDOS();
	InteractionEngineProxy *pInteractionProxy;
	pInteractionProxy = GetDOS()->GetInteractionEngineProxy();
	CN(pInteractionProxy);

	pInteractionProxy->ResetObjects(pSelected->GetInteractionObject());
	pInteractionProxy->ReleaseObjects(pSelected->GetInteractionObject());

	pInteractionProxy->CaptureObject(
		pSelected,
		pSelected->GetInteractionObject(), 
		pSelected->GetContactPoint(), 
		vRotation,
		vSurface,
		m_actuationDepth);

Error:
	return r;
}

RESULT DreamUIBar::HandleTouchMove(void* pContext) {
	return R_PASS;
}

RESULT DreamUIBar::HandleTouchEnd(void* pContext) {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamUIBar::PopPath() {

	RESULT r = R_PASS;

	std::shared_ptr<MenuNode> pNode = nullptr;

	CBR(m_pCloudController != nullptr, R_OBJECT_NOT_FOUND);
	CBR(m_pUserControllerProxy != nullptr, R_OBJECT_NOT_FOUND);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");

	CBR(!m_pathStack.empty(), R_SKIPPED);

	CR(m_pScrollView->HideAllButtons());

	m_pathStack.pop();

	CBR(!m_pathStack.empty(), R_SKIPPED);
	pNode = m_pathStack.top();

	CR(RequestIconFile(pNode));

	CR(m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle()));

Error:
	return r;
}

RESULT DreamUIBar::ResetAppComposite() {
	RESULT r = R_PASS;

	point ptOrigin;
	quaternion qOrigin;
	vector vCameraToMenu;
	//*
	if (m_pUserHandle != nullptr) {
	//	CR(m_pUserHandle->RequestAppBasisPosition(ptOrigin));
		CR(m_pUserHandle->RequestAppBasisOrientation(qOrigin));
	}
	//*/
	if (m_pParentApp != nullptr) {
		ptOrigin = m_pParentApp->GetComposite()->GetOrigin(true);
		//qOrigin = m_pParentApp->GetComposite()->GetOrientation(true);
	}
	
	GetComposite()->SetPosition(ptOrigin);
	GetComposite()->SetOrientation(qOrigin);

	CNR(m_pUIStageProgram, R_SKIPPED);
	vCameraToMenu = GetComposite()->GetPosition(true) - GetDOS()->GetCameraPosition();
	vCameraToMenu.y() = 0.0f;
	vCameraToMenu.Normalize();

	m_pUIStageProgram->SetOriginPoint(m_pScrollView->GetMenuItemsView()->GetPosition(true));
	m_pUIStageProgram->SetOriginDirection(vCameraToMenu);

Error:
	return r;
}

RESULT DreamUIBar::ShowRootMenu(bool fResetComposite) {
	RESULT r = R_PASS;

	CBR(m_pCloudController != nullptr, R_OBJECT_NOT_FOUND);
	CBR(m_pUserControllerProxy != nullptr, R_OBJECT_NOT_FOUND);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");

	//CBR(m_pathStack.empty(), R_SKIPPED);
	m_pathStack = std::stack<std::shared_ptr<MenuNode>>();

	m_pMenuControllerProxy->RequestSubMenu("", "", "Share");
	//m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pShareIcon.get());
	m_pPendingIconTexture = m_pShareIcon.get();
	if (fResetComposite) {
		CR(ResetAppComposite());
	}

Error:
	return r;
}

RESULT DreamUIBar::HandleEvent(UserObserverEventType type) {
	RESULT r = R_PASS;

	switch (type) {
		case UserObserverEventType::BACK: {
		//	CBR(m_menuState != MenuState::ANIMATING, R_SKIPPED);
			CBR(!m_fWaitingForMenuResponse, R_SKIPPED);
			if (m_pKeyboardHandle != nullptr) {
				CR(m_pKeyboardHandle->Hide());
				CR(m_pUserHandle->SendReleaseKeyboard());
				m_pKeyboardHandle = nullptr;

				CR(RequestMenu());
				//break;
			}

			else if (!m_pathStack.empty()) {
				CR(PopPath());

				// if the stack is empty after popping from the path, hide the app
				CBR(m_pathStack.empty(), R_SKIPPED);
				CR(HideApp());
			}

		} break;

		case UserObserverEventType::DISMISS: {
			if (m_pKeyboardHandle != nullptr) {
				m_pKeyboardHandle->Hide();
				m_pUserHandle->SendReleaseKeyboard();
				m_pKeyboardHandle = nullptr;
			} 
			CR(m_pUserHandle->SendClearFocusStack());
			CR(HideApp());
			m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
				
		} break;

		case UserObserverEventType::KB_ENTER: {
			if (m_pKeyboardHandle != nullptr) {
				m_pKeyboardHandle->Hide();
				m_pUserHandle->SendReleaseKeyboard();
				m_pKeyboardHandle = nullptr;
			} 
			m_pUserHandle->SendPreserveSharingState(true);
			m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
			if (m_pParentApp != nullptr) {
				CR(m_pParentApp->CreateBrowserSource());
			}
		} break;
	}


Error:
	return r;
}

texture *DreamUIBar::GetOverlayTexture(HAND_TYPE type) {
	texture *pTexture = nullptr;

	if (type == HAND_TYPE::HAND_LEFT) {
		pTexture = m_pOverlayLeft;
	}
	else {
		pTexture = m_pOverlayRight;
	}

	return pTexture;
}

RESULT DreamUIBar::RequestMenu() {
	RESULT r = R_PASS;

	std::shared_ptr<MenuNode> pNode;
	CBR(!m_pathStack.empty(), R_SKIPPED);

	pNode = m_pathStack.top();

	CR(RequestIconFile(pNode));

	m_pathStack.pop();
	m_fWaitingForMenuResponse = true;
	m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle());

Error:
	return r;
}

RESULT DreamUIBar::RequestIconFile(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	if (pMenuNode->GetTitle() == "Share") {
	//	m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pShareIcon.get());
		m_pPendingIconTexture = m_pShareIcon.get();
	}
	else {
		auto strURI = pMenuNode->GetIconURL();
		if (strURI != "") {
			MenuNode* pTempMenuNode = new MenuNode();
			pTempMenuNode->SetName("root_menu_title");
			CR(m_pHTTPControllerProxy->RequestFile(strURI, GetStringHeaders(), "", std::bind(&DreamUIBar::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), pTempMenuNode));
		}
	}

Error:
	return r;
}

RESULT DreamUIBar::HandleSelect(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;
	//	auto pSelected = GetCurrentItem();
	CBR(m_pScrollView->GetState() != ScrollState::SCROLLING, R_PASS);

	UIMenuItem* pSelected;
	pSelected = reinterpret_cast<UIMenuItem*>(pButtonContext);
	CN(pSelected);
	
	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");
	CBR(pSelected, R_OBJECT_NOT_FOUND);
	CBR(m_pMenuNode, R_OBJECT_NOT_FOUND);

	//TODO: Invisible objects potentially should not receive interaction events
	CBR(GetComposite()->IsVisible(), R_OBJECT_NOT_FOUND);
	CBR(pSelected->IsVisible(), R_OBJECT_NOT_FOUND);

	CBR(m_menuState == MenuState::NONE, R_PASS);

	CNR(m_pUserHandle, R_SKIPPED);
	
	UIMallet* pLeftMallet;
	pLeftMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
	CN(pLeftMallet);

	UIMallet* pRightMallet;
	pRightMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
	CN(pRightMallet);

	GetDOS()->GetInteractionEngineProxy()->ReleaseObjects(pLeftMallet->GetMalletHead());
	GetDOS()->GetInteractionEngineProxy()->ReleaseObjects(pRightMallet->GetMalletHead());

	m_pUserHandle->RequestHapticImpulse(pSelected->GetInteractionObject());
	// should check if website
	for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
		if (pSelected->GetName() == pSubMenuNode->GetTitle()) {
			const std::string& strScope = pSubMenuNode->GetScope();
			const std::string& strPath = pSubMenuNode->GetPath();
			const std::string& strTitle = pSubMenuNode->GetTitle();

			CR(RequestIconFile(pSubMenuNode));

			m_fWaitingForMenuResponse = true;
			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
				auto pTempMenuNode = std::shared_ptr<MenuNode>(new MenuNode(pSubMenuNode->GetNodeType(), pSubMenuNode->GetPath(), pSubMenuNode->GetScope(), pSubMenuNode->GetTitle(), pSubMenuNode->GetMIMEType(), pSubMenuNode->GetIconURL(), pSubMenuNode->GetThumbnailURL()));
				m_pathStack.push(pTempMenuNode);
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {

				if (m_pParentApp != nullptr) {
					CR(m_pParentApp->RequestOpenAsset(strScope, strPath, strTitle));
				}

				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));
				m_pathStack = std::stack<std::shared_ptr<MenuNode>>();

			}
//*
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::ACTION) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBar::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBar::ClearMenuState, this, std::placeholders::_1)));

				//m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);

				if (strTitle == "Desktop") {
					if (m_pParentApp != nullptr) {
						CR(m_pParentApp->RequestOpenAsset(strScope, strPath, strTitle));
					}
				}
				else if (strTitle == "Website") {
					m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
					CN(m_pKeyboardHandle);
					CR(m_pKeyboardHandle->Show());
					ClearMenuWaitingFlag();
				}
				else if (strTitle == "Settings") {
					m_pUserControllerProxy->RequestSettingsForm(strPath);
					m_pathStack = std::stack<std::shared_ptr<MenuNode>>(); // clear stack
					ClearMenuWaitingFlag();
				}

			}
//*/
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
	m_pScrollView->Show();
	m_pScrollView->SetScrollVisible(true);
	m_pScrollView->SetPosition(m_ptMenuShowOffset);
	m_pScrollView->ShowTitle();

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

	// Makes sense for UIBar to always have a user run with it for now
	if(m_pUserHandle == nullptr) {
		auto userUIDs = pDreamOS->GetAppUID("DreamUserApp");

		CB(userUIDs.size() == 1);
		m_userUID = userUIDs[0];

		m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userUID, this));
		CN(m_pUserHandle)
	}

	if (m_pCloudController == nullptr) {
		m_pCloudController = pDreamOS->GetCloudController();
		if (m_pCloudController != nullptr) {

			m_pMenuControllerProxy = (MenuControllerProxy*)(m_pCloudController->GetControllerProxy(CLOUD_CONTROLLER_TYPE::MENU));
			//CNM(m_pMenuControllerProxy, "Failed to get menu controller proxy");
			if (m_pMenuControllerProxy != nullptr) {
				CRM(m_pMenuControllerProxy->RegisterControllerObserver(this), "Failed to register Menu Controller Observer");
			}

			m_pHTTPControllerProxy = (HTTPControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
			//CNM(m_pHTTPControllerProxy, "Failed to get http controller proxy");

			m_pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
			//CNM(m_pUserControllerProxy, "Failed to get user controller proxy");
		}
	}

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

		pTexture = GetDOS()->MakeTextureFromFileBuffer(pBuffer, pBuffer_n, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		CN(pTexture);

		for (auto& pChild : pChildren) {
			auto pObj = dynamic_cast<UIMenuItem*>(pChild.get());
			if (pObj != nullptr && pMenuNodeTitle.size() > 0 && pObj->GetName() == pMenuNodeTitle) {
				pObj->GetSurface()->SetDiffuseTexture(pTexture);
			}
		}

		if (pMenuNodeTitle == "root_menu_title") {
			//m_pScrollView->GetTitleQuad()->SetDiffuseTexture(pTexture);
			m_pPendingIconTexture = pTexture;
			//TODO: May want to move downloading outside of DreamUIBar
			//TODO: the only time the title view is used is to show the chrome icon and "Website" title
			if (m_pKeyboardHandle != nullptr) {
				m_pKeyboardHandle->UpdateTitleView(pTexture, "Website");
			}
		}
		
		if (pBufferVector != nullptr) {
			pBufferVector = nullptr;
		}

	}

	if (m_pMenuNode && m_pMenuNode->CheckAndCleanDirty()) {

		//std::vector<std::shared_ptr<UIMenuItem>> pButtons;
		std::vector<std::shared_ptr<UIButton>> pButtons;

		for (auto &pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {
			auto pButton = m_pView->MakeUIMenuItem(m_pScrollView->GetWidth(), m_pScrollView->GetWidth() * ASPECT_RATIO);
			CN(pButton);

			auto iconFormat = IconFormat();
			iconFormat.pTexture = m_pDefaultThumbnail.get();

			auto labelFormat = LabelFormat();
			labelFormat.strLabel = pSubMenuNode->GetTitle();
			labelFormat.pFont = m_pFont;
			labelFormat.pBgTexture = m_pMenuItemBg.get();

			pButton->Update(iconFormat, labelFormat);

			//CR(pButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
			//*
			CR(pButton->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
				std::bind(&DreamUIBar::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
			CR(pButton->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
				std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1, std::placeholders::_2)));
				//*/
			//CR(pButton->RegisterEvent(UIEventType::UI_SELECT_ENDED,
			//	std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

			pButtons.emplace_back(pButton);
		}

		if (m_pMenuNode->GetNodeType() != MenuNode::type::ACTION) {

			m_pScrollView->GetTitleText()->SetText(m_pMenuNode->GetTitle());
			
			CR(m_pScrollView->UpdateMenuButtons(pButtons));
			
			if (m_pPendingIconTexture != nullptr) {
				m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pPendingIconTexture);
				m_pPendingIconTexture = nullptr;
			}
		}


		CR(ShowApp());

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

RESULT DreamUIBar::ClearMenuWaitingFlag() {
	m_fWaitingForMenuResponse = false;
	return R_PASS;
}

RESULT DreamUIBar::HideApp() {
	RESULT r = R_PASS;

	composite *pComposite = m_pScrollView.get();
	m_menuState = MenuState::ANIMATING;

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
		CN(pDreamUIBar);

		GetComposite()->SetVisible(false, false);
		m_pScrollView->Hide();
		m_menuState = MenuState::NONE;
	Error:
		return r;
	};
//*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pComposite,
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

RESULT DreamUIBar::ShowApp() {
	RESULT r = R_PASS;

	composite *pComposite = m_pScrollView.get();
	pComposite->SetPosition(point(0.0f, 0.0f, 0.0f));
	//m_pScrollView->Show();
//*
	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
		CN(pDreamUIBar);

		CR(pDreamUIBar->UpdateMenu(pContext));
	Error:
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		DreamUIBar *pDreamUIBar = reinterpret_cast<DreamUIBar*>(pContext);
		CN(pDreamUIBar);

		CR(pDreamUIBar->ClearMenuState(pContext));
	Error:
		return r;
	};

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
	if (m_fWaitingForMenuResponse) {
		m_fWaitingForMenuResponse = false;
	}

	if (pMenuNode->NumSubMenuNodes() > 0) {

		m_pMenuNode = pMenuNode;
		if (m_pathStack.empty()) {
			m_pathStack.push(m_pMenuNode);
		}
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
	return R_PASS;
}

DreamUIBar* DreamUIBar::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUIBar *pDreamApp = new DreamUIBar(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamUIBar::SetUIStageProgram(UIStageProgram *pUIStageProgram) {
	m_pUIStageProgram = pUIStageProgram;
	return R_PASS;
}

RESULT DreamUIBar::InitializeWithParent(DreamUserControlArea *pParentApp) {
	RESULT r = R_PASS;

	m_pParentApp = pParentApp;
	CNR(m_pParentApp, R_SKIPPED);

	float totalWidth;
	totalWidth = m_pParentApp->GetTotalWidth();

	m_pScrollView->InitializeWithWidth(totalWidth);

	if (m_pUIStageProgram != nullptr) {
		m_pUIStageProgram->SetClippingThreshold(m_pScrollView->GetClippingThreshold());
		m_pUIStageProgram->SetClippingRate(m_pScrollView->GetClippingRate());
	}

Error:
	return r;
}

RESULT DreamUIBar::UpdateWidth(float totalWidth) {
	RESULT r = R_PASS;

	m_pScrollView->UpdateWithWidth(totalWidth);
	if (m_pUIStageProgram != nullptr) {
		m_pUIStageProgram->SetClippingThreshold(m_pScrollView->GetClippingThreshold());
		m_pUIStageProgram->SetClippingRate(m_pScrollView->GetClippingRate());
	}

	return r;
}

bool DreamUIBar::IsEmpty() {
	bool fEmpty = false;
	if (m_pathStack.empty() && !m_fWaitingForMenuResponse) {
		fEmpty = true;
	}
	return fEmpty;
}

DreamAppHandle* DreamUIBar::GetAppHandle() {
	return (DreamAppHandle*)(this);
}
