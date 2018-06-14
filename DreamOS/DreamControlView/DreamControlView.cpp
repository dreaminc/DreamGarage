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
#include "UI/UISurface.h"

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


	// Texture needs to be upside down, and flipped on y-axis
	m_pLoadingScreenTexture = GetDOS()->MakeTexture(k_wszLoadingScreen, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CN(m_pLoadingScreenTexture);

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
		case HMDDeviceType::META: {
			m_pOverlayLeft = GetDOS()->MakeTexture(k_wszViveOverlayLeft, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
			m_pOverlayRight = GetDOS()->MakeTexture(k_wszViveOverlayRight, texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
		} break;
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	m_fMouseDown[0] = false;
	m_fMouseDown[1] = false;

	m_fMalletDirty[0] = dirty();
	m_fMalletDirty[1] = dirty();

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
		
	UIMallet* pLMallet;
	pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);

	UIMallet* pRMallet;
	pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
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

	for (int i = 0; i < 2; i++) {
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

		m_pSurface->UpdateWithMallet(pMallet, fMalletDirty, m_fMouseDown[i], type);

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
	} break;

	case (UserObserverEventType::KB_ENTER): {	
		if (m_fIsShareURL) {
			m_pUserHandle->SendPreserveSharingState(true);
			if (m_pKeyboardHandle != nullptr) {
				CR(HideKeyboard());
			}

			m_fIsShareURL = false;
		}
	} break;

	} 
	
Error:
	return r;
}

RESULT DreamControlView::InitializeWithParent(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	m_pParentApp = pParent;
	auto pDreamOS = GetDOS();

	float width = m_pParentApp->GetBaseWidth();
	float height = m_pParentApp->GetBaseHeight();
	
	m_pSurface = m_pView->AddUISurface();
	m_pSurface->InitializeSurfaceQuad(width, height);
	//m_pViewQuad = m_pView->AddQuad(width, height, 1, 1, nullptr);
	m_pViewQuad = m_pSurface->GetViewQuad();
	CN(m_pViewQuad);

//	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);

	m_ptVisiblePosition = point(0.0f, 0.0f, 0.0f);
	m_qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, 0.0f);

	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);

	m_pViewBackground = m_pView->AddQuad(width * m_borderWidth, width * m_borderHeight);
	CB(m_pViewBackground);
	m_pBackgroundTexture = GetDOS()->MakeTexture(L"control-view-main-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pViewBackground->SetDiffuseTexture(m_pBackgroundTexture);

	m_pViewBackground->SetPosition(point(0.0f, -0.0005f, 0.0f));

Error:
	return R_PASS;
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
	/* This screws with animations too much, taking it out for now
	if (GetDOS()->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get())) {
		GetDOS()->GetInteractionEngineProxy()->RemoveAnimationObject(m_pViewQuad.get());
	}
	//*/

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

		UIMallet* pLMallet;
		pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
		CNR(pLMallet, R_SKIPPED);

		UIMallet* pRMallet;
		pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
		CNR(pRMallet, R_SKIPPED);

		pLMallet->SetDirty();
		pRMallet->SetDirty();

		m_fMouseDown[0] = false;
		m_fMouseDown[1] = false;

	Error:
		return r;
	};

	// This flag is never set anymore?
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

	//CR(ResetAppComposite());
	GetComposite()->SetVisible(true);
//	m_pViewQuad->SetVisible(true);
	CR(ShowView());

//	m_pViewBackground->SetVisible(true);
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewBackground.get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

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
	//	GetComposite()->SetVisible(false);
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

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pViewBackground->SetVisible(false);

		return r;
	};

	CR(HideView());
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewBackground.get(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_pParentApp->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags(),
		nullptr,
		fnEndCallback,
		this
	));

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

	m_pSurface->ResetLastEvent();

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pView.get(),
		m_ptVisiblePosition,	
		m_qViewQuadOrientation,
		vector(1.0f, 1.0f, 1.0f),
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

	CBR(IsVisible(), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);
	CBR(m_pKeyboardHandle == nullptr, R_SKIPPED);

	float viewHeight;
	viewHeight = m_pViewQuad->GetHeight();

	// currently always fully shown
	textBoxYOffset = viewHeight/2.0f;

	ptTypingOffset = point(0.0f, 0.0f, -m_pViewBackground->GetHeight() * 0.5f);	// so that it'll appear past the keyboard quad

	ptTypingPosition = ptTypingOffset +point(0.0f, sin(TYPING_ANGLE) * textBoxYOffset, -cos(TYPING_ANGLE) * textBoxYOffset);

	if (m_pKeyboardHandle == nullptr) {
		CR(ShowKeyboard());
		CR(m_pKeyboardHandle->PopulateTextBox(strTextField));
	}

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pView.get(),
		ptTypingPosition,
		quaternion::MakeQuaternionWithEuler((float)TYPING_ANGLE, 0.0f, 0.0f),
		//vector(m_visibleScale, m_visibleScale, m_visibleScale),
		vector(1.0f, 1.0f, 1.0f),
		m_keyboardAnimationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));

Error:
	return r;
}

point DreamControlView::GetLastEvent() {
	return m_pSurface->GetLastEvent();
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

std::shared_ptr<UISurface> DreamControlView::GetViewSurface() {
	return m_pSurface;
}

float DreamControlView::GetBackgroundWidth() {
	return m_pViewBackground->GetWidth();
}