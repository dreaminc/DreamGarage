#include "DreamControlView.h"
#include "DreamBrowser.h"
#include "DreamUIBar.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "DreamConsole/DreamConsole.h"

#include "UI/UIMallet.h"
#include "UI/UIView.h"

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

	m_pViewQuad = m_pView->AddQuad(CONTROL_VIEWQUAD_WIDTH, CONTROL_VIEWQUAD_HEIGHT, 1, 1, nullptr);
	CN(m_pViewQuad);

	m_qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(CONTROL_VIEWQUAD_ANGLE * (float)(M_PI) / 180.0f, 0.0f, 0.0f);
	m_pViewQuad->SetOrientation(m_qViewQuadOrientation);
	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	// Texture needs to be upside down, and flipped on y-axis
	m_pLoadingScreenTexture = GetComposite()->MakeTexture(L"client-loading-1366-768.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

	m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
	m_viewState = DreamControlView::state::HIDDEN;
	

	m_ptVisiblePosition = point(0.0f, CONTROL_VIEW_HEIGHT, CONTROL_VIEW_DEPTH);

	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;	// changing this breaks things - change height and width too / instead.

	m_keyboardAnimationDuration = KEYBOARD_ANIMATION_DURATION_SECONDS;	

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
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this);

	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);
	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_DOWN, this);

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
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
		
	CBR((m_viewState == DreamControlView::state::VISIBLE || m_viewState == DreamControlView::state::TYPING), R_SKIPPED);

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

	CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

	//  Note: this duplicates predictive collision implementation from Keyboard
	if (m_viewState == DreamControlView::state::VISIBLE) {
		int i = 0;
		for (auto &pMallet : { pLMallet, pRMallet })
		{
			point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
			point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
			ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));

			if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
				if (pMallet->CheckAndCleanDirty() && m_fMouseDown) {
					m_fMouseDown = false;
					WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
					CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptContact, m_fMouseDown));
				}

				if (pMallet == pLMallet) {
					m_ptLMalletPointing = GetRelativePointofContact(ptSphereOrigin);
				}
				else {
					m_ptRMalletPointing = GetRelativePointofContact(ptSphereOrigin);
				}
			}
			
			if (ptSphereOrigin.y() < pMallet->GetRadius() && m_fMouseDown) {
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				CR(m_pBrowserHandle->SendMalletMoveEvent(ptContact));
			}

			// if the sphere is lower than its own radius, there must be an interaction
			if (ptSphereOrigin.y() < pMallet->GetRadius() && !pMallet->IsDirty()) {
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				CR(pMallet->SetDirty());
				if (ptContact.x > m_pBrowserHandle->GetWidthOfBrowser() || ptContact.x < 0 ||
					ptContact.y > m_pBrowserHandle->GetHeightOfBrowser() || ptContact.y < 0) continue;
				
				m_fMouseDown = true;

				if (pMallet == pLMallet) {
					CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_LEFT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
				}
				else {
					CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_RIGHT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
				}

				CR(m_pBrowserHandle->SendContactToBrowserAtPoint(ptContact, m_fMouseDown));
			}
			i++;
		}
		
	}

	if (m_viewState == DreamControlView::state::TYPING) {
		int i = 0;
		for (auto &pMallet : { pLMallet, pRMallet })
		{
			point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
			point ptSphereOrigin = pMallet->GetMalletHead()->GetOrigin(true);
			ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));

			if (ptSphereOrigin.y() >= pMallet->GetRadius()) {
				pMallet->CheckAndCleanDirty();
			}

			// if the sphere is lower than its own radius, there must be an interaction
			if (ptSphereOrigin.y() < pMallet->GetRadius() && !pMallet->IsDirty()) {
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				CR(pMallet->SetDirty());
				if (ptContact.x > m_pBrowserHandle->GetWidthOfBrowser() || ptContact.x < 0 ||
					ptContact.y > m_pBrowserHandle->GetHeightOfBrowser() || ptContact.y < 0) continue;

				HandleKeyboardDown();
			}
			i++;
		}
	}

Error:
	return r;
}

RESULT DreamControlView::Notify(InteractionObjectEvent *pInteractionEvent) {
	RESULT r = R_PASS;

	switch (m_viewState) {
	case(DreamControlView::state::HIDDEN): {
		if (pInteractionEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) {
			char chkey = (char)(pInteractionEvent->m_value);

			CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

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
	} break;

	case(DreamControlView::state::TYPING): {
		if (pInteractionEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) {
			char chkey = (char)(pInteractionEvent->m_value);
			
			CBR(chkey != SVK_SHIFT, R_SKIPPED);		// don't send these key codes to browser (capital letters and such have different values already)
			CBR(chkey != 0, R_SKIPPED);
			CBR(chkey != SVK_CONTROL, R_SKIPPED);
			// CBR(chkey != SVK_RETURN, R_SKIPPED);		// might be necessary to prevent dupe returns being sent to browser.

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);
			CR(m_pBrowserHandle->SendKeyCharacter(chkey, true));
		}
	} break;

	}

Error:
	return r;
}

RESULT DreamControlView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;
	if (IsVisible()) {
		switch (pEvent->type) {
		case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
			int pxXDiff = pEvent->state.ptTouchpad.x() * BROWSER_SCROLL_CONSTANT;
			int pxYDiff = pEvent->state.ptTouchpad.y() * BROWSER_SCROLL_CONSTANT;

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);
			if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_LEFT)
			{
				if (m_ptLMalletPointing.x < m_pBrowserHandle->GetWidthOfBrowser() && m_ptLMalletPointing.x > 0 &&
					m_ptLMalletPointing.y < m_pBrowserHandle->GetHeightOfBrowser() && m_ptLMalletPointing.y > 0) {
					CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, m_ptLMalletPointing));
				}
				else {
					WebBrowserPoint middleOfBrowser;
					middleOfBrowser.x = m_pBrowserHandle->GetWidthOfBrowser() / 2;
					middleOfBrowser.y = m_pBrowserHandle->GetHeightOfBrowser() / 2;
					CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, middleOfBrowser));
				}
			}
			else if (pEvent->state.type == CONTROLLER_TYPE::CONTROLLER_RIGHT)
			{
				if (m_ptRMalletPointing.x < m_pBrowserHandle->GetWidthOfBrowser() && m_ptRMalletPointing.x > 0 &&
					m_ptRMalletPointing.y < m_pBrowserHandle->GetHeightOfBrowser() && m_ptRMalletPointing.y > 0) {
					CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, m_ptRMalletPointing));
				}
				else {
					WebBrowserPoint middleOfBrowser;
					middleOfBrowser.x = m_pBrowserHandle->GetWidthOfBrowser() / 2;
					middleOfBrowser.y = m_pBrowserHandle->GetHeightOfBrowser() / 2;
					CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, middleOfBrowser));
				}
			}
		} break;
		}	
	}
Error:
	return r;
}

RESULT DreamControlView::HandleEvent(UserObserverEventType type) {
	RESULT r = R_PASS;

	switch (type) {
	case (UserObserverEventType::BACK): {

		if (m_viewState == DreamControlView::state::VISIBLE) {
			CR(Hide());
			CN(m_pUserHandle);
			CR(m_pUserHandle->SendClearFocusStack());

			// if the user is streaming show the menu
			{
				bool fStreaming = false;
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
		}

		if (m_viewState == DreamControlView::state::TYPING) {
			HandleKeyboardDown();
		}
	} break;

	case (UserObserverEventType::DISMISS): {
		CR(Dismiss());
	}

	case (UserObserverEventType::KB_ENTER): {
		if (m_viewState == DreamControlView::state::TYPING) {
			CN(m_pBrowserHandle);
			CR(m_pBrowserHandle->SendKeyCharacter(SVK_RETURN, true));	// ensures browser gets a return key before controlview changes state
			
			HandleKeyboardDown();
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
		m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture.get());
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

	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		SetViewState(DreamControlView::state::SHOW);
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
		m_fMouseDown = false;

		SetViewState(DreamControlView::state::VISIBLE);
		
	Error:
		return r;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		m_ptVisiblePosition,
		m_qViewQuadOrientation,
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

RESULT DreamControlView::Dismiss() {
	RESULT r = R_PASS;

	switch (m_viewState) {
		case DreamControlView::state::TYPING: {
			CN(m_pUserHandle);
			m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
			CN(m_pKeyboardHandle);
			if (m_pKeyboardHandle->IsVisible()) {
				CR(m_pKeyboardHandle->Hide());
			}
			CR(m_pUserHandle->SendReleaseKeyboard());
			m_pKeyboardHandle = nullptr;
		} //break;
		case DreamControlView::state::SHOW:
		case DreamControlView::state::VISIBLE: {
			CR(Hide());
			CN(m_pUserHandle);
			CR(m_pUserHandle->SendClearFocusStack());
		} break;
	}

Error:
	return r;
}

RESULT DreamControlView::Hide() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		SetViewState(DreamControlView::state::HIDE);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		SetViewState(DreamControlView::state::HIDDEN);
		m_strURL = "";
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		m_pViewQuad->GetPosition(),
		m_qViewQuadOrientation,
		vector(m_hiddenScale, m_hiddenScale, m_hiddenScale),
		0.1f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	CR(GetDOS()->ReleaseApp(m_pBrowserHandle, m_browserUID, this)); // release browser

Error:
	return r;
}

bool DreamControlView::IsVisible() {
	bool fIsVisible = false;
	
	if (m_viewState == DreamControlView::state::SHOW || m_viewState == DreamControlView::state::VISIBLE || m_viewState == DreamControlView::state::TYPING) {
		fIsVisible = true;
	}
	
	return fIsVisible;
}

RESULT DreamControlView::SetKeyboardAnimationDuration(float animationDuration) {
	m_keyboardAnimationDuration = animationDuration;
	return R_PASS;
}

RESULT DreamControlView::HandleKeyboardDown() {
	RESULT r = R_PASS;
	
	CN(m_pUserHandle);
	m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
	CN(m_pKeyboardHandle);

	CR(m_pKeyboardHandle->Hide());
	CR(m_pUserHandle->SendReleaseKeyboard());
	m_pKeyboardHandle = nullptr;

	CN(m_pBrowserHandle);			// This unfocuses the text box so that node change event
	WebBrowserPoint unFocusText;	// will fire if user closes keyboard and then wants
	unFocusText.x = -1;				// to go back into the same textbox
	unFocusText.y = -1;
	CR(m_pBrowserHandle->SendContactToBrowserAtPoint(unFocusText, false));
	CR(m_pBrowserHandle->SendContactToBrowserAtPoint(unFocusText, true));

	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(DreamControlView::state::VISIBLE);
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		m_ptVisiblePosition,	
		m_qViewQuadOrientation,
		vector(m_visibleScale, m_visibleScale, m_visibleScale),
		m_keyboardAnimationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
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

	// TODO: get textbox location from node, for now just defaulting to the middle
	if (ptTextBox.y() == -1) {
		ptTextBox.y() = m_pBrowserHandle->GetHeightOfBrowser() / 2.0f;
	}

	textBoxYOffset = ptTextBox.y() / (m_pBrowserHandle->GetHeightOfBrowser() / CONTROL_VIEWQUAD_HEIGHT);	// scaled with ControlViewQuad dimensions
	ptTypingOffset = point(0.0f, -CONTROL_VIEWQUAD_HEIGHT / 2.0f, -0.05f);	// so that it'll appear past the keyboard quad

	ptTypingPosition = ptTypingOffset + point(0.0f, textBoxYOffset, 0.0f);

	if (m_viewState != DreamControlView::state::TYPING) {
		CN(m_pUserHandle);
		m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
		CN(m_pKeyboardHandle);
		CR(m_pKeyboardHandle->PopulateTextBox(strTextField));
		CR(m_pKeyboardHandle->Show());

		CR(m_pUserHandle->SendReleaseKeyboard());
		m_pKeyboardHandle = nullptr;
	}

	auto fnStartCallback = [&](void *pContext) {
		SetViewState(DreamControlView::state::SHOW);	// might want to just make an "ANIMATING" state

		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(DreamControlView::state::TYPING);
		return R_PASS;
	};
	
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewQuad.get(),
		ptTypingPosition,
		quaternion::MakeQuaternionWithEuler((float)TYPING_ROTATION, 0.0f, 0.0f),
		vector(m_visibleScale, m_visibleScale, m_visibleScale),
		m_keyboardAnimationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

Error:
	return r;
}

/*
RESULT DreamControlView::SetSharedViewContext() {
	RESULT r = R_PASS;

	CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

	CR(m_pViewQuad->SetDiffuseTexture(m_pBrowserHandle->GetBrowserTexture().get()));
	
Error:
	return r;
}
//*/

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

RESULT DreamControlView::SetViewState(DreamControlView::state viewState) {
	m_viewState = viewState;
	return R_PASS;
}
