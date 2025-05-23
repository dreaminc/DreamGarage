#include "DreamUIBarApp.h"

#include "functional"                                   // for function
#include "stdint.h"                                     // for uint8_t
#include "vcruntime_new.h"                              // for operator new
#include <stddef.h>                                     // for size_t

#include "os/DreamOS.h"

#include "cloud/CloudController.h"                      // for CloudController, CLOUD_CONTROLLER_TYPE, CLOUD_CONTROLLER_TYPE::MENU
#include "cloud/Menu/MenuController.h"
#include "cloud/Menu/MenuNode.h"
#include "cloud/HTTP/HTTPController.h"

#include "UI/UIButton.h"
#include "UI/UIMenuItem.h"

#include "apps/DreamUserControlAreaApp/DreamUserControlAreaApp.h"

#include "hal/UIStageProgram.h"

#include "core/matrix/RotationMatrix.h"           
#include "core/primitives/vector.h"               
#include "core/hand/hand.h"

#include "modules/InteractionEngine/InteractionEngine.h"
#include "modules/AnimationEngine/AnimationCurve.h"
#include "modules/AnimationEngine/AnimationItem.h"

class DreamAppHandle;
class InteractionEngineProxy;
class UIKeyboard;
class composite;
class quad;
struct UIEvent;

DreamUIBarApp::DreamUIBarApp(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamUIBarApp>(pDreamOS, pContext)//,
{
	// 
}

RESULT DreamUIBarApp::RegisterEvent(InteractionEventType type, std::function<RESULT(void*)> fnCallback) {
	m_callbacks[type] = fnCallback;
	return R_PASS;
}

RESULT DreamUIBarApp::SetFont(const std::wstring& strFile) {
	m_pFont = GetDOS()->MakeFont(strFile, true);
	//m_pFont->SetLineHeight()
	return R_PASS;
}

RESULT DreamUIBarApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = GetDOS();

	m_pFont = pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);

	SetAppName("DreamUIBar");
	SetAppDescription("User Interface");	

	m_pDefaultThumbnail = std::shared_ptr<texture>(pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"thumbnail-default.png"));
	m_pDefaultIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"icon-default.png"));
	m_pMenuIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"icon-menu.png"));
	m_pOpenIcon = std::shared_ptr<texture>(pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"icon-open.png"));
	m_pMenuItemBg = std::shared_ptr<texture>(pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"thumbnail-text-background.png"));

	//TODO: could move this logic up to DreamUserObserver, and then only 
	if (GetDOS()->GetHMD() != nullptr) {
		switch (GetDOS()->GetHMD()->GetDeviceType()) {
		case HMDDeviceType::OCULUS: {
			m_pOverlayLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"left-controller-overlay-active.png");
			m_pOverlayRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"right-controller-overlay-active.png");
		} break;
		case HMDDeviceType::VIVE: {
			m_pOverlayLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-left-active.png");
			m_pOverlayRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-right-active.png");
		} break;
		case HMDDeviceType::META: {
			m_pOverlayLeft = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-left-active.png");
			m_pOverlayRight = GetDOS()->MakeTexture(texture::type::TEXTURE_2D, L"vive-controller-overlay-right-active.png");
		} break;
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	CR(GetComposite()->SetVisible(false, false));
	// Initialize the OBB (collisions)
	CR(GetComposite()->InitializeOBB());
	CR(GetDOS()->AddObjectToInteractionGraph(GetComposite()));

	m_pVolume = GetComposite()->AddVolume(0.1f);

	// Initialize UISpatialScrollView
	m_pView = GetComposite()->AddUIView(GetDOS());
	//m_pView->SetPosition(0.0f, -0.2f, 0.0f);
	//m_pView->SetPosition(0.0f, -0.2f, 0.1f);
	CN(m_pView);

	m_pScrollView = m_pView->AddUISpatialScrollView();
	CN(m_pScrollView);

	pDreamOS->AddAndRegisterInteractionObject(m_pView.get(), InteractionEventType::INTERACTION_EVENT_MENU, m_pView.get());
	CR(m_pView->RegisterSubscriber(UIEventType::UI_MENU, this));

	m_pRootMenuNode = std::shared_ptr<MenuNode>(new MenuNode(MenuNode::type::FOLDER, "", "", "Menu", "", "", ""));
	m_pOpenMenuNode = std::shared_ptr<MenuNode>(new MenuNode(MenuNode::type::FOLDER, "", "MenuProviderScope.OpenMenuProvider", "Open", "", "", ""));

Error:
	return r;
}

RESULT DreamUIBarApp::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamUIBarApp::HandleTouchStart(UIButton* pButtonContext, void* pContext) {
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
	vSurface = qSurface.RotateVector(pSurface->GetNormal() * -1.0f);
	
	//vector for captured object collisions
	qRotation = pSurface->GetOrientation(true);
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

RESULT DreamUIBarApp::HandleTouchMove(void* pContext) {
	return R_PASS;
}

RESULT DreamUIBarApp::HandleTouchEnd(void* pContext) {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamUIBarApp::PopPath() {

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

	m_fWaitingForMenuResponse = true;
	CR(m_pMenuControllerProxy->RequestSubMenu(pNode->GetScope(), pNode->GetPath(), pNode->GetTitle()));

Error:
	return r;
}

RESULT DreamUIBarApp::ResetAppComposite() {
	RESULT r = R_PASS;

	point ptOrigin;
	quaternion qOrigin;
	vector vCameraToMenu;

	std::shared_ptr<DreamUserApp> pDreamUserApp = GetDOS()->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);

	CR(pDreamUserApp->GetAppBasisPosition(ptOrigin));
	CR(pDreamUserApp->GetAppBasisOrientation(qOrigin));
	
	GetComposite()->SetPosition(ptOrigin);
	GetComposite()->SetOrientation(qOrigin);
	m_pVolume->SetVisible(true);

	CNR(m_pUIStageProgram, R_SKIPPED);

	{
		// Set shader orientation based on seating direction		
		RotationMatrix matLook = RotationMatrix(qOrigin);
		vector vAppLook;
		vector vAppLookXZ;
		vAppLook = matLook * vector(0.0f, 0.0f, -1.0f);
		vAppLook.Normalize();
		vAppLookXZ = vector(vAppLook.x(), 0.0f, vAppLook.z()).Normal();
		m_pUIStageProgram->SetOriginDirection(vAppLookXZ);

		m_pUIStageProgram->SetOriginPoint(m_pScrollView->GetMenuItemsView()->GetPosition(true));

		// For mirror
		if (GetDOS()->GetSandboxConfiguration().f3rdPersonCamera) {
			UIStageProgram* pMirrorUIProgram = GetDOS()->GetMirrorUIStageProgram();
			if (pMirrorUIProgram != nullptr) {
				pMirrorUIProgram->SetOriginDirection(vAppLookXZ);
				pMirrorUIProgram->SetOriginPoint(m_pScrollView->GetMenuItemsView()->GetPosition(true));
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBarApp::ShowMenuLevel(MenuLevel menuLevel, bool fResetComposite) {
	RESULT r = R_PASS;

	CBR(!m_fWaitingForMenuResponse, R_SKIPPED);
	CBR(m_pCloudController != nullptr, R_OBJECT_NOT_FOUND);
	CBR(m_pUserControllerProxy != nullptr, R_OBJECT_NOT_FOUND);

	CBM(m_pCloudController->IsUserLoggedIn(), "User not logged in");
	CBM(m_pCloudController->IsEnvironmentConnected(), "Environment socket not connected");

	m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
	m_pMenuNode = nullptr;

	switch (menuLevel) {
	case MenuLevel::ROOT : {
		m_pathStack.push(m_pRootMenuNode);
	} break;
		
	case MenuLevel::OPEN : {
		m_pathStack.push(m_pOpenMenuNode);
	}break;
	};
	
	m_fWaitingForMenuResponse = true;
	m_pMenuControllerProxy->RequestSubMenu(m_pathStack.top()->GetScope() , m_pathStack.top()->GetPath(), m_pathStack.top()->GetTitle());
	
	RequestIconFile(m_pathStack.top());

Error:
	return r;
}

RESULT DreamUIBarApp::HandleEvent(UserObserverEventType type) {
	RESULT r = R_PASS;

	std::shared_ptr<DreamUserApp> pDreamUserApp = GetDOS()->GetUserApp();
	std::shared_ptr<UIKeyboard> pKeyboardApp = GetDOS()->GetKeyboardApp();
	CNR(pDreamUserApp, R_SKIPPED);
	CNR(pKeyboardApp, R_SKIPPED);

	switch (type) {
		case UserObserverEventType::BACK: {

			CBR(!m_fWaitingForMenuResponse, R_SKIPPED);
			if (pKeyboardApp->IsVisible()) {

				CR(pKeyboardApp->Hide());

				CR(PopPath());
			}

			else if (!m_pathStack.empty()) {
				// TODO: Using the MenuWaitingFlag in PopPath() also covers up a bug where backing out of forms sends a double back.  
				//		 Requires a revision of how apps receive Notify (InteractionObjectEvents)
				CR(PopPath());	
				if (m_pathStack.empty()) {	// if stack is empty after pop, hide the app
					CR(HideApp());
					m_pMenuNode = nullptr;
				}
			}

			else {
				ShowMenuLevel(MenuLevel::ROOT, false);
			}

		} break;

		case UserObserverEventType::DISMISS: {
			if (pKeyboardApp != nullptr) {
				pKeyboardApp->Hide();
				pKeyboardApp = nullptr;
			} 
			CR(HideApp());
			m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
			m_pMenuNode = nullptr;
				
		} break;

		case UserObserverEventType::KB_ENTER: {
			if (pKeyboardApp != nullptr) {
				pKeyboardApp->Hide();
				pKeyboardApp = nullptr;
			} 

			if (m_pParentApp != nullptr) {
				CR(m_pParentApp->CreateBrowserSource(m_pMenuNode->GetScope()));
			}

			m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
			m_pMenuNode = nullptr;
		} break;
	}


Error:
	return r;
}

texture *DreamUIBarApp::GetOverlayTexture(HAND_TYPE type) {
	texture *pTexture = nullptr;

	if (type == HAND_TYPE::HAND_LEFT) {
		pTexture = m_pOverlayLeft;
	}
	else {
		pTexture = m_pOverlayRight;
	}

	return pTexture;
}

RESULT DreamUIBarApp::RequestIconFile(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	if (pMenuNode->GetTitle() == "Menu") {
	//	m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pMenuIcon.get());
		m_pPendingIconTexture = m_pMenuIcon.get();
	}
	else if (pMenuNode->GetTitle() == "Open") {
		m_pPendingIconTexture = m_pOpenIcon.get();
	}
	else {
		auto strURI = pMenuNode->GetIconURL();
		if (strURI != "") {
			MenuNode* pTempMenuNode = new MenuNode();
			pTempMenuNode->SetName(m_strIconTitle);
			CR(m_pHTTPControllerProxy->RequestFile(strURI, GetStringHeaders(), "", std::bind(&DreamUIBarApp::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), pTempMenuNode));
		}
	}

Error:
	return r;
}

RESULT DreamUIBarApp::HandleSelect(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

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

	pDreamUserApp = GetDOS()->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);
	
	hand* pLeftHand;
	pLeftHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT);
	CN(pLeftHand);

	hand* pRightMallet;
	pRightMallet = pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
	CN(pRightMallet);

	GetDOS()->GetInteractionEngineProxy()->ReleaseObjects(pLeftHand->GetMalletHead());
	GetDOS()->GetInteractionEngineProxy()->ReleaseObjects(pRightMallet->GetMalletHead());

	pDreamUserApp->CreateHapticImpulse(pSelected->GetInteractionObject());
	// should check if website
	for (auto &pSubMenuNode : m_pScrollView->GetScrollViewNodes()) {
		if (pSelected->GetKey() == pSubMenuNode->GetKey()) {
			const std::string& strScope = pSubMenuNode->GetScope();
			const std::string& strPath = pSubMenuNode->GetPath();
			const std::string& strTitle = pSubMenuNode->GetTitle();

			CR(RequestIconFile(pSubMenuNode));
			m_pathStack.push(pSubMenuNode);

			m_fWaitingForMenuResponse = true;
			if (pSubMenuNode->GetNodeType() == MenuNode::type::FOLDER) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBarApp::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBarApp::ClearMenuState, this, std::placeholders::_1)));
				m_pMenuControllerProxy->RequestSubMenu(strScope, strPath, strTitle);
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::FILE) {

				if (m_pParentApp != nullptr) {
					CR(m_pParentApp->RequestOpenAsset(strScope, strPath, strTitle));
				}

				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBarApp::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBarApp::ClearMenuState, this, std::placeholders::_1)));
			}
			else if (pSubMenuNode->GetNodeType() == MenuNode::type::ACTION) {
				CR(SelectMenuItem(pSelected,
					std::bind(&DreamUIBarApp::SetMenuStateAnimated, this, std::placeholders::_1),
					std::bind(&DreamUIBarApp::ClearMenuState, this, std::placeholders::_1)));

				if (strScope == SCOPE_DESKTOP || strScope == SCOPE_CAMERA) {
					if (m_pParentApp != nullptr) {
						CR(m_pParentApp->RequestOpenAsset(strScope, strPath, strTitle));
					}
				}
				else if (strScope == SCOPE_WEBSITE) {
					std::shared_ptr<UIKeyboard> pKeyboardApp = GetDOS()->GetKeyboardApp();
					CN(pKeyboardApp);
					CR(pKeyboardApp->Show());
					CR(pKeyboardApp->ShowTitleView());
				}

				else if (strScope == "TeamScope.Switch") {
					auto pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

					//CR(pUserController->SwitchTeam(strPath));
					CR(pUserController->RequestSwitchTeam(strPath));
					m_pathStack = std::stack<std::shared_ptr<MenuNode>>();
				}

				else if (strScope == "SystemScope.SignOut") {
					auto pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

					CR(pUserController->Logout());

				}
				else if (strScope == "SystemScope.Close") {
					CR(GetDOS()->Exit(r));
				}
				else if (strScope == "FormScope.Form") {
					auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetDOS()->GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
					CNM(pEnvironmentControllerProxy, "Failed to get environment controller proxy");
					CR(pEnvironmentControllerProxy->RequestForm(strPath));
					pDreamUserApp->SetPreviousApp(this);
				}
				m_pMenuNode = pSubMenuNode;
				ClearMenuWaitingFlag();
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBarApp::HandleOnFileResponse(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext) {
	RESULT r = R_PASS;

	if (pContext != nullptr) {
		MenuNode* pObj = reinterpret_cast<MenuNode*>(pContext);
		CN(pObj);
		if (pObj->GetTitle() == m_strIconTitle) {
			m_pPendingIconTextureBuffer = pBufferVector;
		}
		else {
			if (pObj->GetParentScope() == "" || (m_pMenuNode != nullptr && pObj->GetParentScope() == m_pMenuNode->GetScope())) {
				m_downloadQueue.push(std::pair<MenuNode*, std::shared_ptr<std::vector<uint8_t>>>(pObj, pBufferVector));
			}
		}
	}

Error:
	return r;
}

RESULT DreamUIBarApp::UpdateMenu(void *pContext) {
	RESULT r = R_PASS;

	DreamUIBarApp *pDreamUIBar = reinterpret_cast<DreamUIBarApp*>(pContext);
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

RESULT DreamUIBarApp::Update(void *pContext) {
	RESULT r = R_PASS;

	DreamOS *pDreamOS = GetDOS();

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
	
	// Got a new MenuNode - moving to a different level in menu from the current one
	// So clear everything and create buttons
	if (m_pMenuNode && m_pMenuNode->IsDirty()) {	// this is iffy, it relies on dirty only being set if we move to a new menu level
		m_downloadQueue = std::queue<std::pair<MenuNode*, std::shared_ptr<std::vector<uint8_t>>>>();
		m_requestQueue = std::queue<std::shared_ptr<MenuNode>>();
		m_pScrollView->ClearScrollViewNodes();
		m_loadedMenuItems = 0;
		m_pendingRequests = 0;

		CR(MakeMenuItems());
	}
	CNR(m_pMenuNode, R_SKIPPED);

	// Processing Title icon
	if (m_pPendingIconTextureBuffer != nullptr) {

		std::shared_ptr<UIKeyboard> pKeyboardApp = GetDOS()->GetKeyboardApp();

		CN(m_pPendingIconTextureBuffer);
		uint8_t* pBuffer = &(m_pPendingIconTextureBuffer->operator[](0));
		size_t pBuffer_n = m_pPendingIconTextureBuffer->size();

		m_pPendingIconTexture = GetDOS()->MakeTextureFromFileBuffer(texture::type::TEXTURE_2D, pBuffer, pBuffer_n);
		CN(m_pPendingIconTexture);

		if (pKeyboardApp != nullptr && pKeyboardApp->IsVisible()) {
			pKeyboardApp->UpdateTitleView(m_pPendingIconTexture, "Website");
		}
		else {
			m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pPendingIconTexture);
		}

		DEBUG_LINEOUT("updated title for %s", m_pMenuNode->GetTitle().c_str());

		m_pPendingIconTextureBuffer = nullptr;
		m_pPendingIconTexture = nullptr;
	}

	// Created buttons, so build the texture request queue
	if (m_fAddNewMenuItems) {
		for (auto& pSubMenuNode : m_pMenuNode->GetSubMenuNodes()) {

			if (pSubMenuNode->GetThumbnailTexture() == nullptr) {	// This should always be true :/
				pSubMenuNode->SetThumbnailTexture(m_pDefaultThumbnail.get());
			}
			m_pScrollView->AddScrollViewNode(pSubMenuNode);
			m_requestQueue.push(pSubMenuNode);
		}
		m_fAddNewMenuItems = false;
		m_fRequestTexture = true;
		ClearMenuWaitingFlag();
	}

	// Send out texture requests - limited by m_concurrentRequestLimit
	if (!m_requestQueue.empty() && m_fRequestTexture) {
		CR(RequestMenuItemTexture());
	}
	
	if (!m_downloadQueue.empty()) {
		CR(ProcessDownloadMenuItemTexture());
	}
	
	CR(m_pScrollView->Update());
	//CR(ResetAppComposite());

Error:
	return r;
}

RESULT DreamUIBarApp::MakeMenuItems() {
	RESULT r = R_PASS;	

	CNR(m_pMenuNode, R_SKIPPED);

	if (m_pMenuNode) {

		int elements = ((int)m_pMenuNode->NumSubMenuNodes() > 8 ? 8 : (int)m_pMenuNode->NumSubMenuNodes());
		std::vector<std::shared_ptr<UIButton>> pButtons;

		for (int i = 0; i < elements; i++) {	
			auto pUIMenuItem = m_pView->MakeUIMenuItem(m_pScrollView->GetWidth(), m_pScrollView->GetWidth() * ASPECT_RATIO);
			CN(pUIMenuItem);

			auto iconFormat = IconFormat();
			iconFormat.pTexture = m_pDefaultThumbnail.get();

			auto labelFormat = LabelFormat();
			labelFormat.strLabel = "";
			labelFormat.pFont = m_pFont;
			labelFormat.pBgTexture = m_pMenuItemBg.get();

			pUIMenuItem->Update(iconFormat, labelFormat);

			//CR(pUIMenuItem->RegisterEvent(UIEventType::UI_SELECT_ENDED,
			CR(pUIMenuItem->RegisterEvent(UIEventType::UI_SELECT_BEGIN,
				std::bind(&DreamUIBarApp::HandleTouchStart, this, std::placeholders::_1, std::placeholders::_2)));
			CR(pUIMenuItem->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
				std::bind(&DreamUIBarApp::HandleSelect, this, std::placeholders::_1, std::placeholders::_2)));	
			//CR(pUIMenuItem->RegisterEvent(UIEventType::UI_SELECT_ENDED,
			//	std::bind(&DreamUIBar::HandleSelect, this, std::placeholders::_1)));

			pButtons.emplace_back(pUIMenuItem);
		}

		CR(m_pScrollView->UpdateMenuButtons(pButtons));
		

		{
			m_pMenuNode->CleanDirty();
			m_pScrollView->GetTitleText()->SetText(m_pMenuNode->GetTitle());

			if (m_pPendingIconTexture != nullptr) {	// Top level menu has it's icon saved as m_pMenuIcon, so it follows this flow :/
				m_pScrollView->GetTitleQuad()->SetDiffuseTexture(m_pPendingIconTexture);
			}

			CR(ShowApp());
		}	
	}

Error:
	return r;
}

RESULT DreamUIBarApp::RequestMenuItemTexture() {
	RESULT r = R_PASS;

	if (m_pMenuNode->NumSubMenuNodes() > 0) {
		auto strHeaders = GetStringHeaders();
		int elements = (m_requestQueue.size() > (m_concurrentRequestLimit - m_pendingRequests) ? (m_concurrentRequestLimit - m_pendingRequests) : (int)m_requestQueue.size());

		for (int i = 0; i < elements; i++) {
			auto strURI = m_requestQueue.front()->GetThumbnailURL();
			if (strURI != "") {
				CR(m_pHTTPControllerProxy->RequestFile(strURI, strHeaders, "", std::bind(&DreamUIBarApp::HandleOnFileResponse, this, std::placeholders::_1, std::placeholders::_2), m_requestQueue.front().get()));
				m_pendingRequests++;
			}
			m_requestQueue.pop();
		}
	}
	m_fRequestTexture = false;

Error:
	return r;
}

RESULT DreamUIBarApp::ProcessDownloadMenuItemTexture() {
	RESULT r = R_PASS;

	int elements = (m_downloadQueue.size() > m_concurrentRequestLimit ? m_concurrentRequestLimit : (int)m_downloadQueue.size());

	for (int i = m_loadedMenuItems; i < elements + m_loadedMenuItems; i++) {
		auto pQueueObj = m_downloadQueue.front();
		auto pMenuNode = pQueueObj.first;
		auto pBufferVector = pQueueObj.second;
		texture *pTexture = nullptr;
		CN(pBufferVector);
		uint8_t* pBuffer = &(pBufferVector->operator[](0));
		size_t pBuffer_n = pBufferVector->size();

		pTexture = GetDOS()->MakeTextureFromFileBuffer(texture::type::TEXTURE_2D, pBuffer, pBuffer_n);
		CN(pTexture);

		pMenuNode->SetThumbnailTexture(pTexture);

		CR(m_pScrollView->UpdateScrollViewNode(pMenuNode));
		
		if (pBufferVector != nullptr) {
			pBufferVector = nullptr;
		}
		m_pendingRequests--;
		m_downloadQueue.pop();
	}

	m_loadedMenuItems += elements;
	m_fRequestTexture = true;

Error:
	return r;
}

RESULT DreamUIBarApp::GetNextPageItems() {
	RESULT r = R_PASS;

	if (!m_fWaitingForMenuResponse) {
		if (m_pMenuNode->GetNextPageToken() != "") {
			m_fWaitingForMenuResponse = true;
			CR(m_pMenuControllerProxy->RequestSubMenu(m_pMenuNode->GetScope(), m_pMenuNode->GetPath(), m_pMenuNode->GetTitle(), m_pMenuNode->GetNextPageToken()));
		}
	}

Error:
	return r;
}

RESULT DreamUIBarApp::SelectMenuItem(UIButton *pPushButton, std::function<RESULT(void*)> fnStartCallback, std::function<RESULT(void*)> fnEndCallback) {
	RESULT r = R_PASS;

	CR(m_pScrollView->HideAllButtons(pPushButton));

Error:
	return r;
}

RESULT DreamUIBarApp::SetMenuStateAnimated(void *pContext) {
	RESULT r = R_PASS;
	m_menuState = MenuState::ANIMATING;
	return r;
}

RESULT DreamUIBarApp::ClearMenuState(void* pContext) {
	RESULT r = R_PASS;
	m_menuState = MenuState::NONE;
	return r;
}

RESULT DreamUIBarApp::ClearMenuWaitingFlag() {
	m_fWaitingForMenuResponse = false;
	return R_PASS;
}

RESULT DreamUIBarApp::HideApp() {
	RESULT r = R_PASS;

	composite *pComposite = m_pScrollView.get();
	m_menuState = MenuState::ANIMATING;

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<UIKeyboard> pKeyboardApp = GetDOS()->GetKeyboardApp();
		DreamUIBarApp *pDreamUIBar = reinterpret_cast<DreamUIBarApp*>(pContext);
		CN(pDreamUIBar);
		CN(pKeyboardApp);

		GetComposite()->SetVisible(false, false);
		m_pScrollView->Hide();
		m_menuState = MenuState::NONE;

		if (pKeyboardApp->IsVisible()) {
			CR(pKeyboardApp->SetVisible(false));
		}

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

RESULT DreamUIBarApp::ShowApp() {
	RESULT r = R_PASS;

	// "Website" is a folder type, we want the "ACTION" one that prompts the url search
	if (m_pMenuNode->GetNodeType() == MenuNode::type::ACTION && m_pMenuNode->GetScope() == SCOPE_WEBSITE) {
		std::shared_ptr<UIKeyboard> pKeyboardApp = GetDOS()->GetKeyboardApp();
		CN(pKeyboardApp);
		CR(pKeyboardApp->SetVisible(true));
		CR(pKeyboardApp->ShowTitleView());
	}

	else {
		composite *pComposite = m_pScrollView.get();
		pComposite->SetPosition(point(0.0f, 0.1f, 0.0f));
		//m_pScrollView->Show();
	//*
		auto fnStartCallback = [&](void *pContext) {
			RESULT r = R_PASS;

			DreamUIBarApp *pDreamUIBar = reinterpret_cast<DreamUIBarApp*>(pContext);
			CN(pDreamUIBar);

			CR(pDreamUIBar->UpdateMenu(pContext));
		Error:
			return r;
		};

		auto fnEndCallback = [&](void *pContext) {
			RESULT r = R_PASS;

			DreamUIBarApp *pDreamUIBar = reinterpret_cast<DreamUIBarApp*>(pContext);
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
	}
//*/

Error:
	return r;
}

RESULT DreamUIBarApp::Shutdown(void *pContext) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT DreamUIBarApp::OnMenuData(std::shared_ptr<MenuNode> pMenuNode) {
	RESULT r = R_PASS;

	CNR(pMenuNode, R_OBJECT_NOT_FOUND);

	if (m_pMenuNode && m_pMenuNode->GetPath() == pMenuNode->GetPath() && m_pMenuNode->GetScope() == pMenuNode->GetScope()) {		// Catch next page
		m_pMenuNode = pMenuNode;
	}
	else {	// opening new layer of the menu
		m_pMenuNode = pMenuNode;	
		m_pMenuNode->SetDirty();
	}
	m_fAddNewMenuItems = true;

Error:
	return r;
}

std::vector<std::string> DreamUIBarApp::GetStringHeaders() {

	std::string strAuthorizationToken = "Authorization: Bearer " + m_pUserControllerProxy->GetUserToken();

	auto strHeaders = HTTPController::ContentHttp();
	strHeaders.push_back(strAuthorizationToken);

	return strHeaders;
}

RESULT DreamUIBarApp::Notify(UIEvent *pEvent) {
	return R_PASS;
}

DreamUIBarApp* DreamUIBarApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamUIBarApp *pDreamApp = new DreamUIBarApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamUIBarApp::SetUIStageProgram(UIStageProgram *pUIStageProgram) {
	m_pUIStageProgram = pUIStageProgram;
	return R_PASS;
}

RESULT DreamUIBarApp::InitializeWithParent(DreamUserControlAreaApp *pParentApp) {
	RESULT r = R_PASS;

	m_pParentApp = pParentApp;
	CNR(m_pParentApp, R_SKIPPED);

	float totalWidth;
	totalWidth = m_pParentApp->GetTotalWidth();

	m_pScrollView->InitializeWithWidth(totalWidth);
	m_pView->SetPosition(point(0.0f, -0.2f, -m_pScrollView->GetWidth() / 2.0f));
	m_pScrollView->RegisterObserver(this);

	if (m_pUIStageProgram != nullptr) {
		m_pUIStageProgram->SetClippingThreshold(m_pScrollView->GetClippingThreshold());
		m_pUIStageProgram->SetClippingRate(m_pScrollView->GetClippingRate());

		if (GetDOS()->GetSandboxConfiguration().f3rdPersonCamera) {
			UIStageProgram* pMirrorUIProgram = GetDOS()->GetMirrorUIStageProgram();
			if (pMirrorUIProgram != nullptr) {
				pMirrorUIProgram->SetClippingThreshold(m_pScrollView->GetClippingThreshold());
				pMirrorUIProgram->SetClippingRate(m_pScrollView->GetClippingRate());
			}
		}
	}

Error:
	return r;
}

bool DreamUIBarApp::IsEmpty() {
	bool fEmpty = false;
	if (m_pathStack.empty() && !m_fWaitingForMenuResponse) {
		fEmpty = true;
	}
	return fEmpty;
}

RESULT DreamUIBarApp::ShouldUpdateMenuShader() {
	m_fShouldResetShader = true;
	return R_PASS;
}

DreamAppHandle* DreamUIBarApp::GetAppHandle() {
	return (DreamAppHandle*)(this);
}
