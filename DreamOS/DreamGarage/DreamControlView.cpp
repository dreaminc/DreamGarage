#include "DreamControlView.h"
#include "DreamBrowser.h"
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

	auto keyUIDs = pDreamOS->GetAppUID("UIKeyboard");
	auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");

	CB(userUIDs.size() == 1);
	m_userUID = userUIDs[0];
	m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userUID, this));
	CN(m_pUserHandle);
	
	CB(keyUIDs.size() == 1);
	m_keyboardUID = keyUIDs[0];

	m_pView = GetComposite()->AddUIView(pDreamOS);
	CN(m_pView);

	m_pViewQuad = m_pView->AddQuad(CONTROL_VIEWQUAD_WIDTH, CONTROL_VIEWQUAD_HEIGHT, 1, 1, nullptr);
	CN(m_pViewQuad);

	m_qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(CONTROL_VIEWQUAD_ANGLE * (float)(M_PI) / 180.0f, 0.0f, 0.0f);
	m_pViewQuad->SetOrientation(m_qViewQuadOrientation);
	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	m_viewState = State::HIDDEN;		

	m_ptVisiblePosition = point(0.0f, CONTROL_VIEW_HEIGHT, CONTROL_VIEW_DEPTH);

	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;	// changing this breaks things - change height and width too / instead.

	m_keyboardAnimationDuration = KEYBOARD_ANIMATION_DURATION_SECONDS;	

	m_pOverlayLeft = GetDOS()->MakeTexture(L"left-controller-overlay-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pOverlayRight = GetDOS()->MakeTexture(L"right-controller-overlay-active.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pOverlayLeft);
	CN(m_pOverlayRight);

	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), INTERACTION_EVENT_KEY_DOWN, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), ELEMENT_INTERSECT_BEGAN, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), ELEMENT_INTERSECT_MOVED, this);
	pDreamOS->AddAndRegisterInteractionObject(GetComposite(), ELEMENT_INTERSECT_ENDED, this);

	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);
	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_MENU_DOWN, this);

	{
		composite *pComposite = GetDOS()->AddComposite();
		CN(pComposite);
		pComposite->SetPosition(GetDOS()->GetCameraPosition() - point(0.0f, -2.5f, 0.0f));	//with hmd

		auto pView = pComposite->AddUIView(GetDOS());
		CN(pView);

		//Setup textbox
		auto pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		{
			pTextBoxText = std::shared_ptr<text>(GetDOS()->MakeText(
				pFont,
				"hi",
				2.5f,
				.050,
				text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));
			CN(pTextBoxText);
			pView->AddObject(pTextBoxText);
			pTextBoxText->SetPosition(point(0.0f, 0.0f, 0.0f));
			pTextBoxText->RotateXByDeg(180.0f);

		}
	}

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;
	//  Note: this duplicates predictive collision implementation from Keyboard
		
	CBR((m_viewState == State::VISIBLE || m_viewState == State::TYPING), R_SKIPPED);

	if (m_pUserHandle == nullptr) {
		auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");
		CB(userUIDs.size() == 1);
		m_userUID = userUIDs[0];

		//Capture user app
		m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userUID, this));
		CN(m_pUserHandle);
	}

	int i = 0;
	UIMallet* pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);
	UIMallet* pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);

	if (m_pMalletRay == nullptr) {
		m_pMalletRay = GetDOS()->AddRay(pRMallet->GetMalletHead()->GetPosition(true), 
			(m_pViewQuad->GetNormal().RotateByQuaternion(m_qViewQuadOrientation.GetConjugate())), 1.0f);
		CN(m_pMalletRay);
		m_pMalletRay->SetVisible(true);

		CR(GetDOS()->AddInteractionObject(m_pMalletRay));
	}

	if (m_pMalletRay != nullptr && pRMallet != nullptr)
	{
		m_pMalletRay->SetPosition(pRMallet->GetMalletHead()->GetPosition() - point(-0.2f, 4.5f, -0.75f));
		m_pMalletRay->SetOrientation(m_pViewQuad->GetNormal().RotateByQuaternion(m_qViewQuadOrientation.GetConjugate()));
	//	m_pMalletRay->SetRayVertices();
	}
	

	CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

	if (m_viewState == State::VISIBLE) {
		point ptCollisions[2];
		for (auto &mallet : { pLMallet, pRMallet })
		{
			point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
			point ptSphereOrigin = mallet->GetMalletHead()->GetOrigin(true);
			ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));
			ptCollisions[i] = ptSphereOrigin;

			if (ptSphereOrigin.y() >= mallet->GetRadius()) {
				mallet->CheckAndCleanDirty();
			}

			// if the sphere is lower than its own radius, there must be an interaction
			if (ptSphereOrigin.y() < mallet->GetRadius() && !mallet->IsDirty()) {
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				CR(mallet->SetDirty());
				pTextBoxText->SetText(std::to_string(ptContact.x) + ", " + std::to_string(ptContact.y));
				if (ptContact.x > m_pBrowserHandle->GetWidthOfBrowser() || ptContact.x < 0 ||
					ptContact.y > m_pBrowserHandle->GetHeightOfBrowser() || ptContact.y < 0) continue;

				if (mallet == pLMallet) {
					CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_LEFT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
				}
				else {
					CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_RIGHT, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
				}

				(m_pBrowserHandle->SendClickToBrowserAtPoint(ptContact));
			}
			i++;
		}
		
	}

	// This is really dirty
	if (m_viewState == State::TYPING) {
		point ptCollisions[2];
		for (auto &mallet : { pLMallet, pRMallet })
		{
			point ptBoxOrigin = m_pViewQuad->GetOrigin(true);
			point ptSphereOrigin = mallet->GetMalletHead()->GetOrigin(true);
			ptSphereOrigin = (point)(inverse(RotationMatrix(m_pViewQuad->GetOrientation(true))) * (ptSphereOrigin - m_pViewQuad->GetOrigin(true)));
			ptCollisions[i] = ptSphereOrigin;

			if (ptSphereOrigin.y() >= mallet->GetRadius()) {
				mallet->CheckAndCleanDirty();
			}

			// if the sphere is lower than its own radius, there must be an interaction
			if (ptSphereOrigin.y() < mallet->GetRadius() && !mallet->IsDirty()) {
				WebBrowserPoint ptContact = GetRelativePointofContact(ptSphereOrigin);
				CR(mallet->SetDirty());
				pTextBoxText->SetText(std::to_string(ptContact.x) + ", " + std::to_string(ptContact.y));
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
	case(State::HIDDEN): {
		if (pInteractionEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) {
			char chkey = (char)(pInteractionEvent->m_value);

			CBR(chkey != 0x00, R_SKIPPED);	// To catch empty chars used to refresh textbox	

			m_strURL += chkey;
		}
	} break;

	case(State::TYPING): {
		if (pInteractionEvent->m_eventType == INTERACTION_EVENT_KEY_DOWN) {
			char chkey = (char)(pInteractionEvent->m_value);

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);
			CR(m_pBrowserHandle->SendKeyCharacter(chkey, true));
		}
	} break;

	case(State::VISIBLE): {
		if (pInteractionEvent->m_eventType == ELEMENT_INTERSECT_BEGAN) {
			m_pMalletRay->SetVisible(true);
		}
		if (pInteractionEvent->m_eventType == ELEMENT_INTERSECT_MOVED) {
			m_ptMalletPointing = GetRelativePointofContact(pInteractionEvent->m_ptContact[0]);
			pTextBoxText->SetText(std::to_string(m_ptMalletPointing.x) + ", " + std::to_string(m_ptMalletPointing.y));
		}
		if (pInteractionEvent->m_eventType == ELEMENT_INTERSECT_ENDED) {
			//m_pMalletRay->SetVisible(false);
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

			if (GetDOS()->GetHMD() != nullptr) {
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 2.0f, 1));
			}

			CNR(m_pBrowserHandle, R_OBJECT_NOT_FOUND);

			CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff));
			//CR(m_pBrowserHandle->ScrollByDiff(pxXDiff, pxYDiff, m_ptMalletPointing));

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
		if (m_viewState == State::VISIBLE) {
			CR(Hide());
			CN(m_pUserHandle);
			CR(m_pUserHandle->SendClearFocusStack());
		}
		if (m_viewState == State::TYPING) {
			CN(m_pBrowserHandle);			// This unfocuses the text box so that node change event
			WebBrowserPoint unFocusText;	// will fire if user closes keyboard and then wants
			unFocusText.x = -1;				// to go back into the same textbox
			unFocusText.y = -1;
			CR(m_pBrowserHandle->SendClickToBrowserAtPoint(unFocusText));

			HandleKeyboardDown();
		}
	}

	case (UserObserverEventType::KB_ENTER): {
		if (m_viewState == State::TYPING) {
			HandleKeyboardDown();
		}
	} break;

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

	if (m_strURL != "") {
		CR(m_pBrowserHandle->SendURL(m_strURL));
		m_strURL = "";
	}

	CR(m_pBrowserHandle->RequestBeginStream());

	SetSharedViewContext();

	CN(m_pUserHandle);
	CR(m_pUserHandle->RequestAppBasisPosition(ptAppBasisPosition));
	CR(m_pUserHandle->RequestAppBasisOrientation(qAppBasisOrientation));

	GetComposite()->SetPosition(ptAppBasisPosition);
	GetComposite()->SetOrientation(qAppBasisOrientation);

	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		SetViewState(State::SHOW);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(State::VISIBLE);
		return R_PASS;
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
		case State::TYPING: {
			CN(m_pUserHandle);
			m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
			CN(m_pKeyboardHandle);
			if (m_pKeyboardHandle->IsVisible()) {
				CR(m_pKeyboardHandle->Hide());
			}
			CR(m_pUserHandle->SendReleaseKeyboard());
			m_pKeyboardHandle = nullptr;
		} //break;
		case State::SHOW:
		case State::VISIBLE: {
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
		SetViewState(State::HIDE);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		SetViewState(State::HIDDEN);
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
	
	if (m_viewState == State::SHOW || m_viewState == State::VISIBLE || m_viewState == State::TYPING) {
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

	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(State::VISIBLE);	
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

	CN(m_pBrowserHandle);
	CBR(IsVisible(), R_SKIPPED);

	textBoxYOffset = ptTextBox.y() / (m_pBrowserHandle->GetHeightOfBrowser() / CONTROL_VIEWQUAD_HEIGHT);	// scaled with ControlViewQuad dimensions
	ptTypingPosition = point(0.0f, -0.10f, -0.35f) + point(0.0f, textBoxYOffset, 0.0f);

	if (m_viewState != State::TYPING) {
		CN(m_pUserHandle);
		m_pKeyboardHandle = m_pUserHandle->RequestKeyboard();
		CN(m_pKeyboardHandle);
		CR(m_pKeyboardHandle->PopulateTextBox(strTextField));
		CR(m_pKeyboardHandle->Show());

		CR(m_pUserHandle->SendReleaseKeyboard());
		m_pKeyboardHandle = nullptr;
	}

	auto fnStartCallback = [&](void *pContext) {
		SetViewState(State::SHOW);	// might want to just make an "ANIMATING" state

		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		SetViewState(State::TYPING);
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

///*
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

RESULT DreamControlView::SetViewState(State state) {
	m_viewState = state;
	return R_PASS;
}
