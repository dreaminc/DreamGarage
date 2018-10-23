#include "UIControlView.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamShareView/DreamShareView.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIMallet.h"
//#include "UI/UIView.h"
#include "UI/UIButton.h"
#include "UI/UISurface.h"

UIControlView::UIControlView(HALImp *pHALImp, DreamOS *pDreamOS) :
	UIView(pHALImp, pDreamOS)
{
	//empty
}

RESULT UIControlView::Initialize() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);

	m_pView = AddUIView();
	CN(m_pView);

	// Texture needs to be upside down, and flipped on y-axis
	m_pLoadingScreenTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszLoadingScreen);
	CN(m_pLoadingScreenTexture);

	m_hiddenScale = 0.2f;
	m_visibleScale = 1.0f;	// changing this breaks things - change height and width too / instead.

	m_keyboardAnimationDuration = KEYBOARD_ANIMATION_DURATION_SECONDS;	

	if (m_pDreamOS->GetHMD() != nullptr) {
		switch (m_pDreamOS->GetHMD()->GetDeviceType()) {
		case HMDDeviceType::OCULUS: {
			m_pOverlayLeft = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOculusOverlayLeft);
			m_pOverlayRight = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszOculusOverlayRight);
		} break;
		case HMDDeviceType::VIVE: {
			m_pOverlayLeft = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszViveOverlayLeft);
			m_pOverlayRight = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszViveOverlayRight);
		} break;
		case HMDDeviceType::META: {
			m_pOverlayLeft = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszViveOverlayLeft);
			m_pOverlayRight = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, k_wszViveOverlayRight);
		} break;
		}

		CN(m_pOverlayLeft);
		CN(m_pOverlayRight);
	}

	m_fMouseDown[0] = false;
	m_fMouseDown[1] = false;

	m_fMalletDirty[0] = dirty();
	m_fMalletDirty[1] = dirty();

	float width;
	width = pDreamUserApp->GetBaseWidth();

	float height;
	height = pDreamUserApp->GetBaseHeight();
	
	m_pUISurface = m_pView->AddUISurface();
	m_pUISurface->InitializeSurfaceQuad(width, height);
	//m_pViewQuad = m_pView->AddQuad(width, height, 1, 1, nullptr);
	m_pViewQuad = m_pUISurface->GetViewQuad();
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
	m_pBackgroundTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"control-view-main-background.png");
	m_pViewBackground->SetDiffuseTexture(m_pBackgroundTexture);

	m_pViewBackground->SetPosition(point(0.0f, -0.0005f, 0.0f));

Error:
	return r;
}

RESULT UIControlView::Update() {
	RESULT r = R_PASS;	

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);
		
	UIMallet* pLMallet;
	pLMallet = pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);

	UIMallet* pRMallet;
	pRMallet = pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT);
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

		m_pUISurface->UpdateWithMallet(pMallet, fMalletDirty, m_fMouseDown[i], type);

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

RESULT UIControlView::HandleEvent(UserObserverEventType type) {
	RESULT r = R_PASS;

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);
		
	switch (type) {
	case (UserObserverEventType::BACK): {

		if (m_pDreamOS->GetKeyboardApp()->IsVisible()) {

			CR(HandleKeyboardDown());
		}

		else if (IsVisible()) {
			bool fStreaming = false;

			CR(Hide());
			pDreamUserApp->SetHasOpenApp(false);
		}


	} break;

	} 
	
Error:
	return r;
}

texture *UIControlView::GetOverlayTexture(HAND_TYPE type) {
	texture *pTexture = nullptr;

	if (type == HAND_TYPE::HAND_LEFT) {
		pTexture = m_pOverlayLeft;
	}
	else {
		pTexture = m_pOverlayRight;
	}

	return pTexture;
}

RESULT UIControlView::SetViewQuadTexture(std::shared_ptr<texture> pBrowserTexture) {
	m_pViewQuad->SetDiffuseTexture(pBrowserTexture.get());	//Control view texture to be set by Browser
	return R_PASS;
}

RESULT UIControlView::ShowView() {
	RESULT r = R_PASS;
	/* This screws with animations too much, taking it out for now
	if (m_pDreamOS->GetInteractionEngineProxy()->IsAnimating(m_pViewQuad.get())) {
		m_pDreamOS->GetInteractionEngineProxy()->RemoveAnimationObject(m_pViewQuad.get());
	}
	//*/

	auto fnStartCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		
		std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
		CNR(pDreamUserApp, R_SKIPPED);

		pDreamUserApp->SetEventApp(this);

		UIMallet* pLMallet;
		pLMallet = pDreamUserApp->GetMallet(HAND_TYPE::HAND_LEFT);
		CNR(pLMallet, R_SKIPPED);

		UIMallet* pRMallet;
		pRMallet = pDreamUserApp->GetMallet(HAND_TYPE::HAND_RIGHT);
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
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UIControlView::ResetAppComposite() {
	RESULT r = R_PASS;

	point ptAppBasisPosition;
	quaternion qAppBasisOrientation;	

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);

	CR(pDreamUserApp->GetAppBasisPosition(ptAppBasisPosition));
	CR(pDreamUserApp->GetAppBasisOrientation(qAppBasisOrientation));

	SetPosition(ptAppBasisPosition);
	SetOrientation(qAppBasisOrientation);
Error:
	return r;
}

RESULT UIControlView::Show() {
	RESULT r = R_PASS;

	//CR(ResetAppComposite());
	SetVisible(true);
//	m_pViewQuad->SetVisible(true);
	CR(ShowView());

//	m_pViewBackground->SetVisible(true);
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewBackground.get(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_pDreamOS->GetUserApp()->GetAnimationDuration(),
		AnimationCurveType::SIGMOID,
		AnimationFlags::AnimationFlags()
	));

Error:
	return r;
}

RESULT UIControlView::Dismiss() {
	RESULT r = R_PASS;

	if (m_pDreamOS->GetKeyboardApp()->IsVisible()) {
		CR(HideKeyboard());
	}

	CR(Hide());

Error:
	return r;
}

RESULT UIControlView::HideView() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
	//	SetVisible(false);
		m_strURL = "";
		return R_PASS;
	};

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UIControlView::Hide() {
	RESULT r = R_PASS;

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		m_pViewBackground->SetVisible(false);

		return r;
	};

	CR(HideView());
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pViewBackground.get(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_pDreamOS->GetUserApp()->GetAnimationDuration(),
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

bool UIControlView::IsVisible() {
	return m_pViewQuad != nullptr && m_pViewQuad->IsVisible();
}

bool UIControlView::IsAnimating() {
	auto pProxy = m_pDreamOS->GetInteractionEngineProxy();

	bool fViewAnimating = pProxy->IsAnimating(m_pView.get());
	bool fQuadAnimating = pProxy->IsAnimating(m_pViewQuad.get());

	return fViewAnimating || fQuadAnimating;
}

RESULT UIControlView::SetURLText(std::string strURL) {
	RESULT r = R_PASS;

	std::shared_ptr<text> pURLText;

	//CNR(pURLText, R_SKIPPED);
	//pURLText->SetDirty();
	m_strText = strURL;

//Error:
	return r;
}

RESULT UIControlView::SetKeyboardAnimationDuration(float animationDuration) {
	m_keyboardAnimationDuration = animationDuration;
	return R_PASS;
}

RESULT UIControlView::ShowKeyboard() {
	RESULT r = R_PASS;

	//maintain the keyboard handle until the keyboard is hidden
	std::shared_ptr<UIKeyboard> pKeyboardApp = m_pDreamOS->GetKeyboardApp();
	CNM(pKeyboardApp, "keyboard not available");
	CBR(!pKeyboardApp->IsVisible(), R_SKIPPED);

	CR(pKeyboardApp->Show());

Error:
	return r;
}

RESULT UIControlView::HideKeyboard() {
	RESULT r = R_PASS;

	CR(m_pDreamOS->GetKeyboardApp()->Hide());

Error:
	return r;
}

RESULT UIControlView::HandleKeyboardDown() {
	RESULT r = R_PASS;
	
	CR(HideKeyboard());

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT UIControlView::HandleKeyboardUp() {
	RESULT r = R_PASS;

	point ptTypingPosition;
	float textBoxYOffset;
	float verticalAngle;
	// Position the ControlView behind the keyboard with a slight height offset (center should be above keyboard textbox).
	point ptTypingOffset;

	std::shared_ptr<UIKeyboard> pKeyboardApp = m_pDreamOS->GetKeyboardApp();
	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pKeyboardApp, R_SKIPPED);
	CNR(pDreamUserApp, R_SKIPPED);

	CBR(IsVisible(), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);

	float viewHeight;
	viewHeight = m_pViewQuad->GetHeight();

	// currently always fully shown
	textBoxYOffset = viewHeight/2.0f;
	//textBoxYOffset *= 0.8f;

	// 58 degrees is the old typing angle (straight up), move the y offset down to accomodate
	// tilting the screen up
	verticalAngle = (90.0f - pDreamUserApp->GetViewAngle()) * M_PI / 180.0;
	textBoxYOffset *= 1.0f - sin(verticalAngle - TYPING_ANGLE); 

	ptTypingOffset = point(0.0f, 0.0f, -m_pViewBackground->GetHeight() * 0.5f);	// so that it'll appear past the keyboard quad

	ptTypingPosition = ptTypingOffset +point(0.0f, sin(TYPING_ANGLE) * textBoxYOffset, -cos(TYPING_ANGLE) * textBoxYOffset);

	CR(ShowKeyboard());

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
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

point UIControlView::GetLastEvent() {
	return m_pUISurface->GetLastEvent();
}

std::shared_ptr<quad> UIControlView::GetViewQuad() {
	return m_pViewQuad;
}

std::shared_ptr<UISurface> UIControlView::GetViewSurface() {
	return m_pUISurface;
}

float UIControlView::GetBackgroundWidth() {
	return m_pViewBackground->GetWidth();
}