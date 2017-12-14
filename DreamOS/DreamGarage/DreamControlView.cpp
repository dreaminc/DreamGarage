#include "DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamUIBar.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIMallet.h"
//#include "UI/UIView.h"
#include "UI/UIControlBar.h"
#include "UI/UIButton.h"

RESULT DreamControlViewHandle::SetControlViewTexture(std::shared_ptr<texture> pBrowserTexture) {
	RESULT r = R_PASS;	// This is just an option, currently Texture is retrieved through Browser Handle
	CB(GetAppState());

	return SetViewQuadTexture(pBrowserTexture);

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

	m_pControlBar = m_pView->AddUIControlBar();
	m_pControlBar->SetVisible(false);
	CN(m_pControlBar);

	CR(m_pControlBar->GetStopButton()->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
		std::bind(&DreamControlView::HandleStopSharing, this, std::placeholders::_1, std::placeholders::_2)));

	CR(m_pControlBar->GetToggleButton()->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
		std::bind(&DreamControlView::HandleToggleControlBar, this, std::placeholders::_1, std::placeholders::_2)));

	CR(m_pControlBar->GetBackButton()->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
		std::bind(&DreamControlView::HandleBack, this, std::placeholders::_1, std::placeholders::_2)));

	CR(m_pControlBar->GetForwardButton()->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
		std::bind(&DreamControlView::HandleForward, this, std::placeholders::_1, std::placeholders::_2)));

	CR(m_pControlBar->GetURLButton()->RegisterEvent(UIEventType::UI_SELECT_TRIGGER,
		std::bind(&DreamControlView::HandleEnterURL, this, std::placeholders::_1, std::placeholders::_2)));

	// Texture needs to be upside down, and flipped on y-axis
	m_pLoadingScreenTexture = GetDOS()->MakeTexture(k_wszLoadingScreen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

	m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);

	{
		float controlBarOffset = m_pControlBar->GetSpacingOffset() - (VIEW_HEIGHT / 2.0f);
		m_pControlBar->SetPosition(point(0.0f, 0.0f, -controlBarOffset));
		m_pControlBar->SetOrientation(quaternion::MakeQuaternionWithEuler(-(float)(M_PI) / 2.0f, 0.0f, 0.0f));
	}

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
	m_ptLastEvent.x = -1;
	m_ptLastEvent.y = -1;

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;	

	auto pText = m_pControlBar->GetURLText();
	if (pText->CheckAndCleanDirty()) {
		//auto strText = pText->GetText();
		//pText->SetText("");
		pText->SetText(m_strText);
	}

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

	CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

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
		UpdateWithMallet(pMallet, fMalletDirty, m_fMouseDown[i], type);
		if (fMalletDirty) {
			m_fMalletDirty[i].SetDirty();
		} 
		else {
			m_fMalletDirty[i].CheckAndCleanDirty();
		}
	}

Error:
	return r;
}

RESULT DreamControlView::UpdateWithMallet(UIMallet *pMallet, bool &fMalletDirty, bool &fMouseDown, HAND_TYPE handType) {
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

			if (fMouseDown) {
				fMouseDown = false;
				WebBrowserPoint ptContact = GetRelativePointofContact(m_ptClick);
				CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptContact, fMouseDown));
			}

			if (handType == HAND_TYPE::HAND_LEFT) {
				m_ptLMalletPointing = GetRelativePointofContact(ptSphereOrigin);
			}
			else {
				m_ptRMalletPointing = GetRelativePointofContact(ptSphereOrigin);
			}
		}

		if (ptSphereOrigin.y() < pMallet->GetRadius() && fMouseDown) {
			WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
			CR(m_pBrowserHandle->SendMalletMoveEvent(ptContact));
		}

		// if the sphere is lower than its own radius, there must be an interaction
		if (ptSphereOrigin.y() < pMallet->GetRadius() && !fMalletDirty) {
			WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
			bool fInBrowserQuad = ptContact.x > m_pBrowserHandle->GetWidthOfBrowser() || ptContact.x < 0 ||
				ptContact.y > m_pBrowserHandle->GetHeightOfBrowser() || ptContact.y < 0;
			CBR(!fInBrowserQuad, R_SKIPPED);

			fMalletDirty = true;

			m_ptClick = ptSphereOrigin;
			fMouseDown = true;

			if (handType == HAND_TYPE::HAND_LEFT) {
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_LEFT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
			}
			else {
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_RIGHT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
			}

			CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptContact, fMouseDown));
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

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);
			CR(m_pBrowserHandle->SendKeyCharacter(chkey, true));
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

		CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

		if (ptScroll.x < m_pBrowserHandle->GetWidthOfBrowser() && ptScroll.x > 0 &&
			ptScroll.y < m_pBrowserHandle->GetHeightOfBrowser() && ptScroll.y > 0) {
			CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, ptScroll));
		}
		else {
			WebBrowserPoint middleOfBrowser;
			middleOfBrowser.x = m_pBrowserHandle->GetWidthOfBrowser() / 2;
			middleOfBrowser.y = m_pBrowserHandle->GetHeightOfBrowser() / 2;
			CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, middleOfBrowser));
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
				m_pControlBar->GetURLText()->SetVisible(true);

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

			// if the user is streaming show the menu
			CR(m_pUserHandle->RequestStreamingState(fStreaming));

			if (fStreaming) {
				auto pDreamOS = GetDOS();

				auto menuUIDs = pDreamOS->GetAppUID("DreamUIBar");
				CB(menuUIDs.size() == 1);
				auto pMenuHandle = dynamic_cast<DreamUIBarHandle*>(pDreamOS->CaptureApp(menuUIDs[0], this));
				if (pMenuHandle != nullptr) {
					CR(m_pUserHandle->RequestResetAppComposite());
					CR(pMenuHandle->SendShowRootMenu());
					CR(m_pUserHandle->SendPushFocusStack(pMenuHandle));
				}
				pDreamOS->ReleaseApp(pMenuHandle, menuUIDs[0], this);
			}
		}


	} break;

	case (UserObserverEventType::DISMISS): {
		CR(Dismiss());
	}

	case (UserObserverEventType::KB_ENTER): {

		if (m_fIsShareURL) {
			CR(ShowView());

			if (m_pKeyboardHandle != nullptr) {
				CR(HideKeyboard());
			}

			m_fIsShareURL = false;
			CR(SendURL());
		}
		else {
			CR(HandleKeyboardDown());

			if (m_pBrowserHandle != nullptr) {
				CR(m_pBrowserHandle->SendKeyCharacter(SVK_RETURN, true));	// ensures browser gets a return key before controlview changes state
			}
		}	

	} break;

	} 
	
Error:
	return r;
}

RESULT DreamControlView::SendURL() {
	RESULT r = R_PASS;

	if (m_pBrowserHandle == nullptr) {
		std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");	// capture browser
		CB(uids.size() == 1);
		m_browserUID = uids[0];

		m_pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(m_browserUID, this));
		CN(m_pBrowserHandle);
	}

	if (m_strURL != "") {
		m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);
		CR(m_pBrowserHandle->SendURL(m_strURL));
		m_strURL = "";
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

DreamControlView *DreamControlView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlView *pDreamControlView = new DreamControlView(pDreamOS, pContext);
	return pDreamControlView;
}

RESULT DreamControlView::ShowView() {
	RESULT r = R_PASS;


	//TODO: animation for m_pControlBar
	m_pControlBar->SetVisible(true);
	
	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		//m_pControlBar->GetURLText()->SetText(m_strURL);
		//m_pControlBar->SetVisible(true);
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

RESULT DreamControlView::Show() {
	RESULT r = R_PASS;

	point ptAppBasisPosition;
	quaternion qAppBasisOrientation;	

	std::vector<UID> uids = GetDOS()->GetAppUID("DreamBrowser");	// capture browser
	CB(uids.size() == 1);
	m_browserUID = uids[0];

	m_pBrowserHandle = dynamic_cast<DreamBrowserHandle*>(GetDOS()->CaptureApp(m_browserUID, this));
	CN(m_pBrowserHandle);

	CR(m_pBrowserHandle->RequestBeginStream());

	//SetSharedViewContext();

	CN(m_pUserHandle);
	CR(m_pUserHandle->RequestAppBasisPosition(ptAppBasisPosition));
	CR(m_pUserHandle->RequestAppBasisOrientation(qAppBasisOrientation));

	GetComposite()->SetPosition(ptAppBasisPosition);
	GetComposite()->SetOrientation(qAppBasisOrientation);

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
		m_pControlBar->SetVisible(false);
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
	CR(GetDOS()->ReleaseApp(m_pBrowserHandle, m_browserUID, this)); // release browser

Error:
	return r;
}

bool DreamControlView::IsVisible() {

	//the keyboard handle is available while the keyboard is visible
	bool fKeyboardVisible = m_pKeyboardHandle != nullptr;

	//TODO: replace with GetComposite()->IsVisible() if possible
	bool fViewVisible = m_pViewQuad->IsVisible() || m_pControlBar->IsVisible();
	
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
	CNR(m_pControlBar, R_SKIPPED);

	pURLText = m_pControlBar->GetURLText();
	CNR(pURLText, R_SKIPPED);
	pURLText->SetDirty();
	m_strText = strURL;

Error:
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

	//TODO: move this when possible
	m_pKeyboardHandle = nullptr;

Error:
	return r;
}

RESULT DreamControlView::HandleKeyboardDown() {
	RESULT r = R_PASS;
	
	CR(HideKeyboard());

	CN(m_pBrowserHandle);			// This unfocuses the text box so that node change event
	WebBrowserPoint ptUnFocusText;	// will fire if user closes keyboard and then wants
	ptUnFocusText.x = -1;				// to go back into the same textbox
	ptUnFocusText.y = -1;
	CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptUnFocusText, false));
	CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptUnFocusText, true));
	m_ptLastEvent = ptUnFocusText;
	//CR(m_pBrowserHandle->SendKeyCharacter(SVK_ESCAPE, true));	// ensures browser gets a return key before controlview changes state

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

	CN(m_pBrowserHandle);
	CBR(IsVisible(), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);
	CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);
	CBR(m_ptLastEvent.x != -1 || m_ptLastEvent.y != -1, R_SKIPPED);

	// TODO: get textbox location from node, for now just defaulting to the middle
	if (ptTextBox.y() == -1) {
		ptTextBox.y() = m_pBrowserHandle->GetHeightOfBrowser() / 2.0f;
	}
	//CBR(ptTextBox.y() != -1, R_SKIPPED);

	textBoxYOffset = ptTextBox.y() / (m_pBrowserHandle->GetHeightOfBrowser() / VIEW_HEIGHT);	// scaled with ControlViewQuad dimensions
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
	
	ptRelative.x = posX * m_pBrowserHandle->GetWidthOfBrowser();
	ptRelative.y = posY * m_pBrowserHandle->GetHeightOfBrowser();

	return ptRelative;
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

bool DreamControlView::CanPressButton(UIButton *pButtonContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	CBR(!m_fMalletDirty[0].IsDirty() && !m_fMalletDirty[1].IsDirty(), R_SKIPPED);

	CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pView.get()), R_SKIPPED);
	CBR(!pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get()), R_SKIPPED);

	CR(m_fMalletDirty[0].SetDirty());
	CR(m_fMalletDirty[1].SetDirty());

	//only allow button presses while keyboard isn't active
	CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);

	CR(m_pUserHandle->RequestHapticImpulse(pButtonContext->GetInteractionObject()));

	return true;
Error:
	return false;
}

RESULT DreamControlView::HandleStopSharing(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(CanPressButton(pButtonContext), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);

	CR(m_pBrowserHandle->SendStopEvent());

Error:
	return r;
}

RESULT DreamControlView::HandleToggleControlBar(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(CanPressButton(pButtonContext), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);

	if (m_pViewQuad->IsVisible()) {
		auto fnStartCallback = [&](void *pContext) {
			return R_PASS;
		};

		auto fnEndCallback = [&](void *pContext) {
			GetViewQuad()->SetVisible(false);
			m_pControlBar->GetToggleButton()->GetSurface()->SetDiffuseTexture(m_pControlBar->GetShowTexture());
			m_fIsMinimized = true;
			m_strURL = "";
			return R_PASS;
		};

		CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
			m_pViewQuad.get(),
			m_pViewQuad->GetPosition(),
			m_pViewQuad->GetOrientation(),
			vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
			0.1f,
			AnimationCurveType::SIGMOID,
			AnimationFlags(),
			fnStartCallback,
			fnEndCallback,
			this
		));

	} 

	else {
		auto fnStartCallback = [&](void *pContext) {
			GetViewQuad()->SetVisible(true);
			m_fIsMinimized = false;
			return R_PASS;
		};

		auto fnEndCallback = [&](void *pContext) {
			RESULT r = R_PASS;
			
			m_fMouseDown[0] = false;
			m_fMouseDown[1] = false;

			m_pControlBar->GetToggleButton()->GetSurface()->SetDiffuseTexture(m_pControlBar->GetHideTexture());
//		Error:
			return r;
		};

		CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
			m_pViewQuad.get(),
			m_pViewQuad->GetPosition(),
			m_pViewQuad->GetOrientation(),
			vector(m_visibleScale, m_visibleScale, m_visibleScale),
			0.1f,
			AnimationCurveType::SIGMOID,
			AnimationFlags(),
			fnStartCallback,
			fnEndCallback,
			this
		));
	}
	

Error:
	return r;
}

RESULT DreamControlView::HandleEnterURL(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	auto pDreamOS = GetDOS();

	CBR(CanPressButton(pButtonContext), R_SKIPPED);

	CR(pDreamOS->GetInteractionEngineProxy()->ResetObjects(pButtonContext->GetInteractionObject()));
	CR(pDreamOS->GetInteractionEngineProxy()->ReleaseObjects(pButtonContext->GetInteractionObject()));

	if (m_pKeyboardHandle == nullptr) {
		CR(ShowKeyboard());
		CR(m_pKeyboardHandle->ShowTitleView());
	}

	CR(HideView());
	m_fIsShareURL = true;

Error:
	return r;
}

RESULT DreamControlView::HandleBack(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pBrowserHandle->SendBackEvent());

Error:
	return r;
}

RESULT DreamControlView::HandleForward(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(CanPressButton(pButtonContext), R_SKIPPED);
	CR(m_pBrowserHandle->SendForwardEvent());

Error:
	return r;
}
