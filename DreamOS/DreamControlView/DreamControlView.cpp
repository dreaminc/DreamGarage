#include "DreamControlView.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamShareView/DreamShareView.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIMallet.h"
//#include "UI/UIView.h"
//#include "UI/UIControlBar.h"
#include "UI/UIButton.h"

RESULT DreamControlViewHandle::SetControlViewTexture(std::shared_ptr<texture> pBrowserTexture) {
	RESULT r = R_PASS;	// This is just an option, currently Texture is retrieved through Browser Handle
	CB(GetAppState());

	return SetViewQuadTexture(pBrowserTexture);

Error:
	return r;
}

RESULT DreamControlViewHandle::SendContentType(std::string strContentType) {
	RESULT r = R_PASS;	
	CB(GetAppState());

	return SetContentType(strContentType);

Error:
	return r;
}

RESULT DreamControlViewHandle::ShowApp() {
	RESULT r = R_PASS;	// This is just an option, currently Texture is retrieved through Browser Handle

	CB(GetAppState());
	CR(Show());

Error:
	return r;
}

RESULT DreamControlViewHandle::HideApp() {
	RESULT r = R_PASS;	// This is just an option, currently Texture is retrieved through Browser Handle

	CB(GetAppState());
	CR(Hide());

Error:
	return r;
}

RESULT DreamControlViewHandle::DismissApp() {
	RESULT r = R_PASS;	

	CB(GetAppState());
	CR(Dismiss());

Error:
	return r;
}

RESULT DreamControlViewHandle::SendURLtoBrowser() {
	RESULT r = R_PASS;

	CB(GetAppState());
	CR(SendURL());

Error:
	return r;
}

RESULT DreamControlViewHandle::SendBrowserScopeAndPath(std::string strScope, std::string strPath) {
	RESULT r = R_PASS;

	CB(GetAppState());
	CR(SetBrowserScopeAndPath(strScope, strPath));

Error:
	return r;
}

bool DreamControlViewHandle::IsAppVisible() {
	RESULT r = R_PASS;	// This is just an option, currently Texture is retrieved through Browser Handle

	CB(GetAppState());
	return IsVisible();

Error:
	return false;
}

RESULT DreamControlViewHandle::SendURLText(std::string strURL) {
	RESULT r = R_PASS;

	CB(GetAppState());
	CR(SetURLText(strURL));

Error:
	return r;
}

DreamControlView::DreamControlView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlView>(pDreamOS, pContext)
{
	//empty
}

RESULT DreamControlView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();

	SetAppName("DreamControlView");
	GetDOS()->AddObjectToUIGraph(GetComposite());	

	m_pView = GetComposite()->AddUIView(pDreamOS);
	CN(m_pView);

	m_pViewQuad = m_pView->AddQuad(VIEW_WIDTH, VIEW_HEIGHT, 1, 1, nullptr);
	CN(m_pViewQuad);

	float viewAngleRad = VIEW_ANGLE * (float)(M_PI) / 180.0f;

	m_ptVisiblePosition = point(0.0f, VIEW_POS_HEIGHT, VIEW_POS_DEPTH);

	m_qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(viewAngleRad, 0.0f, 0.0f);
	m_pView->SetOrientation(m_qViewQuadOrientation);
	m_pView->SetPosition(m_ptVisiblePosition);

	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	// Texture needs to be upside down, and flipped on y-axis
	m_pLoadingScreenTexture = GetDOS()->MakeTexture(k_wszLoadingScreen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

	m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);


	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;	// changing this breaks things - change height and width too / instead.

	m_keyboardAnimationDuration = KEYBOARD_ANIMATION_DURATION_SECONDS;	

	if (GetDOS()->GetHMD() != nullptr) {
		switch (GetDOS()->GetHMD()->GetDeviceType()) {
		case HMDDeviceType::OCULUS: {
			m_pOverlayLeft = GetDOS()->MakeTexture(k_wszOculusOverlayLeft, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(k_wszOculusOverlayRight, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		case HMDDeviceType::VIVE: {
			m_pOverlayLeft = GetDOS()->MakeTexture(k_wszViveOverlayLeft, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(k_wszViveOverlayRight, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this);

	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);
	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_DOWN, this);

	m_fMouseDown[0] = false;
	m_fMouseDown[1] = false;

	m_fMalletDirty[0] = dirty();
	m_fMalletDirty[1] = dirty();

	m_fCanPressButton[0] = dirty();
	m_fCanPressButton[1] = dirty();

	m_ptLastEvent.x = -1;
	m_ptLastEvent.y = -1;

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;


//Error:
	return r;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;	

	if (m_pUserHandle == nullptr) {
		auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");

		CBR(userUIDs.size() == 1, R_SKIPPED);
		m_userUID = userUIDs[0];
		m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userUID, this));
		CN(m_pUserHandle);
	}
		
	UIMallet* pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);
	UIMallet* pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);	

	// skip mallet update while keyboard is active
	//*
	if (IsAnimating()) {
		m_fMouseDown[0] = false;
		m_fMouseDown[1] = false;
	}
	//*/
	CBR(!IsAnimating(), R_SKIPPED);
	CBR(IsVisible(), R_SKIPPED);
	// if a url is being shared through the keyboard don't update the control view surface
	CBR(!m_fIsShareURL, R_SKIPPED);

	for (int i = 0; i < 2; i++)
	{
		UIMallet *pMallet;
		HAND_TYPE type;
		if (i == 0) {
			pMallet = pLMallet;
			type = HAND_TYPE::HAND_LEFT;
		}
		else {
			pMallet = pRMallet;
			type = HAND_TYPE::HAND_RIGHT;
		}

		bool fMalletDirty = m_fMalletDirty[i].IsDirty();
		bool fControlBarDirty = m_fCanPressButton[i].IsDirty();

		UpdateWithMallet(pMallet, fMalletDirty, fControlBarDirty, m_fMouseDown[i], type);

		if (fMalletDirty) {
			m_fMalletDirty[i].SetDirty();
		} 
		else {
			m_fMalletDirty[i].CheckAndCleanDirty();
		}

		if (fControlBarDirty) {
			m_fCanPressButton[i].SetDirty();
		}
		else {
			m_fCanPressButton[i].CheckAndCleanDirty();
		}
	}

Error:
	return r;
}

RESULT DreamControlView::UpdateWithMallet(UIMallet *pMallet, bool &fMalletDirty, bool &fControlBarDirty, bool &fMouseDown, HAND_TYPE handType) {
	RESULT r = R_PASS;

	point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
	point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
	ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));

	// if keyboard is up, touching the view quad is always a dismiss
	if (m_pKeyboardHandle != nullptr && !m_fIsShareURL) {
		if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
			fMalletDirty = false;
		}
		if (ptSphereOrigin.y() < pMallet->GetRadius() && !fMalletDirty) {
			CR(HandleKeyboardDown());
			fMalletDirty = true;
		}
	}

	else {
		if (ptSphereOrigin.y() >= pMallet->GetRadius()) {

			fMalletDirty = false;
			fControlBarDirty = false;

			if (fMouseDown) {
				fMouseDown = false;
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				//WebBrowserPoint ptContact = GetRelativePointofContact(m_ptClick);
				if (m_pParentApp != nullptr) {
					CR(m_pParentApp->SendContactAtPoint(ptContact, fMouseDown));
				}
			}

			if (handType == HAND_TYPE::HAND_LEFT) {
				m_ptLMalletPointing = GetRelativePointofContact(ptSphereOrigin);
			}
			else {
				m_ptRMalletPointing = GetRelativePointofContact(ptSphereOrigin);
			}

		}

		float xDistance = ptSphereOrigin.x() - m_ptClick.x();
		float zDistance = ptSphereOrigin.z() - m_ptClick.z();
		float squaredDistance = xDistance * xDistance + zDistance * zDistance;

		if (ptSphereOrigin.y() < pMallet->GetRadius() && fMouseDown && squaredDistance > m_dragThresholdSquared) {
			WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
			if (m_pParentApp != nullptr) {
				CR(m_pParentApp->SendMalletMoveEvent(ptContact));
			}
			//m_ptClick = ptSphereOrigin;
		}

		// if the sphere is lower than its own radius, there must be an interaction
		if (ptSphereOrigin.y() < pMallet->GetRadius() && !fMalletDirty) {
			WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);

			float browserWidth = m_pParentApp->GetPXWidth();
			float browserHeight = m_pParentApp->GetPXHeight();

			bool fNotInBrowserQuad = ptContact.x > browserWidth || ptContact.x < 0 ||
				ptContact.y > browserHeight || ptContact.y < 0;

			fMalletDirty = true;

			fNotInBrowserQuad = fNotInBrowserQuad || m_fIsMinimized;
			CBR(!fNotInBrowserQuad, R_SKIPPED);

			m_ptClick = ptSphereOrigin;
			fMouseDown = true;

			if (handType == HAND_TYPE::HAND_LEFT) {
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_LEFT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
			}
			else {
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_RIGHT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
			}

			CR(m_pParentApp->SendContactAtPoint(ptContact, fMouseDown));
			m_ptLastEvent = ptContact;
		}
	}

Error:
	return r;
}

RESULT DreamControlView::Notify(InteractionObjectEvent *pInteractionEvent) {
	RESULT r = R_PASS;

	if (pInteractionEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) {
		char chkey = (char)(pInteractionEvent->m_value);

		CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

		if (m_pKeyboardHandle != nullptr && !m_fIsShareURL) {
			CBR(chkey != SVK_SHIFT, R_SKIPPED);		// don't send these key codes to browser (capital letters and such have different values already)
			CBR(chkey != 0, R_SKIPPED);
			CBR(chkey != SVK_CONTROL, R_SKIPPED);
			// CBR(chkey != SVK_RETURN, R_SKIPPED);		// might be necessary to prevent dupe returns being sent to browser.

			CNR(m_pParentApp, R_OBJECT_NOT_FOUND);
			CR(m_pParentApp->SendKeyCharacter(chkey, true));
		}
		else {
			if (chkey == 0x01) {	// dupe filters from UIKeyboard to properly build URL based on what is in Keyboards textbox
				m_strURL = "";		// could be scraped if we exposed keyboards textbox and pulled it via a keyboard handle
			}

			else if (chkey == SVK_BACK) {
				if (m_strURL.size() > 0) {
					m_strURL.pop_back();
				}
			}
			
			else {
				m_strURL += chkey;
			}
		}
	}

Error:
	return r;
}

RESULT DreamControlView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;
	CBR(IsVisible(), R_SKIPPED);
	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		int pxXDiff = pEvent->state.ptTouchpad.x() * BROWSER_SCROLL_CONSTANT;
		int pxYDiff = pEvent->state.ptTouchpad.y() * BROWSER_SCROLL_CONSTANT;

		WebBrowserPoint ptScroll;
		if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT) {
			ptScroll = m_ptLMalletPointing;
		}
		else if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT) {
			ptScroll = m_ptRMalletPointing;
		}

		CNR(m_pParentApp, R_OBJECT_NOT_FOUND);

		if (ptScroll.x < m_pParentApp->GetPXWidth() && ptScroll.x > 0 &&
			ptScroll.y < m_pParentApp->GetPXHeight() && ptScroll.y > 0) {
			CR(m_pParentApp->ScrollByDiff(pxXDiff, pxYDiff, ptScroll));
		}
		else {
			WebBrowserPoint middleOfBrowser;
			middleOfBrowser.x = m_pParentApp->GetPXWidth() / 2;
			middleOfBrowser.y = m_pParentApp->GetPXHeight() / 2;
			CR(m_pParentApp->ScrollByDiff(pxXDiff, pxYDiff, middleOfBrowser));
		}

	} break;
	}
Error:
	return r;
}

RESULT DreamControlView::HandleEvent(UserObserverEventType type) {
	RESULT r = R_PASS;

	switch (type) {
	case (UserObserverEventType::BACK): {

		if (m_pKeyboardHandle != nullptr) {

			if (m_fIsShareURL) {
				CR(ShowView());

				if (m_pKeyboardHandle != nullptr) {
					CR(HideKeyboard());
				}
				m_fIsShareURL = false;
			}

			else {
				CR(HandleKeyboardDown());
			}
		}

		else if (IsVisible()) {
			bool fStreaming = false;

			CR(Hide());
			CN(m_pUserHandle);
			CR(m_pUserHandle->SendClearFocusStack());

		}


	} break;

	case (UserObserverEventType::DISMISS): {
		CR(Dismiss());
	}

	case (UserObserverEventType::KB_ENTER): {	


		if (m_fIsShareURL) {
			//CR(ShowView());
			m_pUserHandle->SendPreserveSharingState(true);
			if (m_pKeyboardHandle != nullptr) {
				CR(HideKeyboard());
			}

			m_fIsShareURL = false;

			//TODO: when using the control bar, we know that a website will be shared on enter,
			// need a way to not have the scope and path hardcoded here

			CR(SendURL());

			//TODO: bypass making a request to help smooth the loading
			//CR(SendURI());
		}
		else {
			if (m_pParentApp != nullptr) {
				CR(m_pParentApp->SendKeyCharacter(SVK_RETURN, true));	// ensures browser gets a return key before controlview changes state
			}

			CR(HandleKeyboardDown());

		}	


		} break;

	} 
	
Error:
	return r;
}

RESULT DreamControlView::InitializeWithParent(DreamUserControlArea *pParent) {
	m_pParentApp = pParent;

	return R_PASS;
}

RESULT DreamControlView::SendURL() {
	RESULT r = R_PASS;

	if (m_pParentApp != nullptr) {
		CR(m_pParentApp->SetScope("WebsiteProviderScope.WebsiteProvider"));
		CR(m_pParentApp->SetPath(""));
		if (m_strURL != "") {
			m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);
			CR(m_pParentApp->SendURL(m_strURL));
		}
	}

Error:
	return r;
}

RESULT DreamControlView::SetBrowserScopeAndPath(std::string strScope, std::string strPath) {
	RESULT r = R_PASS;

	if (m_pParentApp != nullptr) {
		CR(m_pParentApp->SetScope(strScope));
		CR(m_pParentApp->SetPath(strPath));
	}

Error:
	return r;
}

texture *DreamControlView::GetOverlayTexture(HAND_TYPE type) {
	texture *pTexture = nullptr;

	if (type == HAND_TYPE::HAND_LEFT) {
		pTexture = m_pOverlayLeft;
	}
	else {
		pTexture = m_pOverlayRight;
	}

	return pTexture;
}

RESULT DreamControlView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamAppHandle* DreamControlView::GetAppHandle() {
	return (DreamControlViewHandle*)(this);
}

RESULT DreamControlView::SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) {
	m_pViewQuad->SetDiffuseTexture(pBrowserTexture.get());	//Control view texture to be set by Browser
	return R_PASS;
}

RESULT DreamControlView::SetContentType(std::string strContentType) {


	m_currentControlBarType = UIControlBar::ControlBarTypeFromString(strContentType);

	//TODO: don't need to do this until different types of control bars are defined

	return R_PASS;
}

DreamControlView *DreamControlView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlView *pDreamControlView = new DreamControlView(pDreamOS, pContext);
	return pDreamControlView;
}

RESULT DreamControlView::ShowView() {
	RESULT r = R_PASS;

	if (GetDOS()->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get())) {
		GetDOS()->GetInteractionEngineProxy()->RemoveAnimationObject(m_pViewQuad.get());
	}

	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		

		if (m_pUserHandle == nullptr) {
			auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");
			CB(userUIDs.size() == 1);
			m_userUID = userUIDs[0];

			//Capture user app
			m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userUID, this));
			CN(m_pUserHandle);
		}

		UIMallet* pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
		CNR(pLMallet, R_SKIPPED);
		UIMallet* pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
		CNR(pRMallet, R_SKIPPED);

		pLMallet->SetDirty();
		pRMallet->SetDirty();
		m_fMouseDown[0] = false;
		m_fMouseDown[1] = false;

	Error:
		return r;
	};

	CBR(!m_fIsMinimized, R_SKIPPED);
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		m_pViewQuad->GetPosition(),
		m_pViewQuad->GetOrientation(),
		vector(m_visibleScale, m_visibleScale, m_visibleScale),
		0.1f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamControlView::ResetAppComposite() {
	RESULT r = R_PASS;

	point ptAppBasisPosition;
	quaternion qAppBasisOrientation;	

	CN(m_pUserHandle);

	CR(m_pUserHandle->RequestAppBasisPosition(ptAppBasisPosition));
	CR(m_pUserHandle->RequestAppBasisOrientation(qAppBasisOrientation));

	GetComposite()->SetPosition(ptAppBasisPosition);
	GetComposite()->SetOrientation(qAppBasisOrientation);
Error:
	return r;
}

RESULT DreamControlView::Show() {
	RESULT r = R_PASS;

	CR(ResetAppComposite());

	CR(ShowView());

Error:
	return r;
}

RESULT DreamControlView::Dismiss() {
	RESULT r = R_PASS;

	if (m_pKeyboardHandle != nullptr) {
		CR(HideKeyboard());
	}

	CR(Hide());
	CN(m_pUserHandle);
	CR(m_pUserHandle->SendClearFocusStack());

Error:
	return r;
}

RESULT DreamControlView::HideView() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		m_strURL = "";
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		m_pViewQuad->GetPosition(),
		m_pViewQuad->GetOrientation(),
		vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		0.1f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

Error:
	return r;
}

RESULT DreamControlView::Hide() {
	RESULT r = R_PASS;

	CR(HideView());

	//SetIsMinimizedFlag(false);

Error:
	return r;
}

bool DreamControlView::IsVisible() {

	//the keyboard handle is available while the keyboard is visible
	bool fKeyboardVisible = m_pKeyboardHandle != nullptr;

	//TODO: replace with GetComposite()->IsVisible() if possible
	bool fViewVisible = m_pViewQuad->IsVisible();
	
	// this function is closer to IsAppBeingUsed
	return fKeyboardVisible || fViewVisible;
}

bool DreamControlView::IsAnimating() {
	auto pProxy = GetDOS()->GetInteractionEngineProxy();

	bool fViewAnimating = pProxy->IsAnimating(m_pView.get());
	bool fQuadAnimating = pProxy->IsAnimating(m_pViewQuad.get());

	return fViewAnimating || fQuadAnimating;
}

RESULT DreamControlView::SetURLText(std::string strURL) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pURLText;

	//CNR(pURLText, R_SKIPPED);
	//pURLText->SetDirty();
	m_strText = strURL;

//Error:
	return r;
}

RESULT DreamControlView::SetKeyboardAnimationDuration(float animationDuration) {
	m_keyboardAnimationDuration = animationDuration;
	return R_PASS;
}

RESULT DreamControlView::ShowKeyboard() {
	RESULT r = R_PASS;

	CNM(m_pUserHandle, "user app not found");

	//maintain the keyboard handle until the keyboard is hidden
	m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
	CNM(m_pKeyboardHandle, "keyboard handle not available");

	CR(m_pKeyboardHandle->Show());

Error:
	return r;
}

RESULT DreamControlView::HideKeyboard() {
	RESULT r = R_PASS;

	CNR(m_pKeyboardHandle, R_OBJECT_NOT_FOUND);
	CNR(m_pUserHandle, R_OBJECT_NOT_FOUND);

	CR(m_pKeyboardHandle->Hide());
	CR(m_pUserHandle->SendReleaseKeyboard());

Error:
	m_pKeyboardHandle = nullptr;
	
	return r;
}

RESULT DreamControlView::HandleKeyboardDown() {
	RESULT r = R_PASS;
	
	CR(HideKeyboard());

	CN(m_pParentApp);			// This unfocuses the text box so that node change event
	WebBrowserPoint ptUnFocusText;	// will fire if user closes keyboard and then wants
	ptUnFocusText.x = -1;				// to go back into the same textbox
	ptUnFocusText.y = -1;
	CR(m_pParentApp->SendContactAtPoint(ptUnFocusText, false));
	CR(m_pParentApp->SendContactAtPoint(ptUnFocusText, true));
	m_ptLastEvent = ptUnFocusText;

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pView.get(),
		m_ptVisiblePosition,	
		m_qViewQuadOrientation,
		m_pView->GetScale(),
		m_keyboardAnimationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));
	
Error:
	return r;
}

RESULT DreamControlView::HandleKeyboardUp(std::string strTextField, point ptTextBox) {
	RESULT r = R_PASS;

	point ptTypingPosition;
	float textBoxYOffset;
	// Position the ControlView behind the keyboard with a slight height offset (center should be above keyboard textbox).
	point ptTypingOffset;

	CN(m_pParentApp);
	CBR(IsVisible(), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);
	CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);
	CBR(m_ptLastEvent.x != -1 || m_ptLastEvent.y != -1, R_SKIPPED);

	// TODO: get textbox location from node, for now just defaulting to the middle
	if (ptTextBox.y() == -1) {
		ptTextBox.y() = m_pParentApp->GetPXHeight() / 2.0f;
	}
	//CBR(ptTextBox.y() != -1, R_SKIPPED);

	textBoxYOffset = ptTextBox.y() / (m_pParentApp->GetPXHeight() / VIEW_HEIGHT);	// scaled with ControlViewQuad dimensions
	ptTypingOffset = point(0.0f, -VIEW_HEIGHT / 2.0f, -0.05f);	// so that it'll appear past the keyboard quad

	ptTypingPosition = ptTypingOffset + point(0.0f, textBoxYOffset, 0.0f);

	if (m_pKeyboardHandle == nullptr) {
		CR(ShowKeyboard());
		CR(m_pKeyboardHandle->PopulateTextBox(strTextField));
	}

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pView.get(),
		ptTypingPosition,
		quaternion::MakeQuaternionWithEuler((float)TYPING_ANGLE, 0.0f, 0.0f),
		//vector(m_visibleScale, m_visibleScale, m_visibleScale),
		m_pView->GetScale(),
		m_keyboardAnimationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));

Error:
	return r;
}

WebBrowserPoint DreamControlView::GetRelativePointofContact(point ptContact) {
	point ptIntersectionContact = ptContact;
	ptIntersectionContact.w() = 1.0f;
	WebBrowserPoint ptRelative;

	// First apply transforms to the ptIntersectionContact 
	//point ptAdjustedContact = inverse(m_pViewQuad->GetModelMatrix()) * ptIntersectionContact;
	point ptAdjustedContact = ptIntersectionContact;
	
	float width = m_pViewQuad->GetWidth();
	float height = m_pViewQuad->GetHeight();

	float posX = ptAdjustedContact.x() / (width / 2.0f);	
	float posY = ptAdjustedContact.z() / (height / 2.0f);

	//float posZ = ptAdjustedContact.z();	// 3D browser when

	posX = (posX + 1.0f) / 2.0f;	// flip it
	posY = (posY + 1.0f) / 2.0f;  
	
	ptRelative.x = posX * m_pParentApp->GetPXWidth();
	ptRelative.y = posY * m_pParentApp->GetPXHeight();

	return ptRelative;
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}