#include "UIControlView.h"

#include "DreamGarage/DreamUIBar.h"
#include "DreamShareView/DreamShareView.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UISurface.h"

#include "Primitives/hand/hand.h"

#include "Sandbox/PathManager.h"

UIControlView::UIControlView(HALImp *pHALImp, DreamOS *pDreamOS) :
	UISurface(pHALImp, pDreamOS)
{
	//empty
}

RESULT UIControlView::Initialize() {
	RESULT r = R_PASS;

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);

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
	
	CN(InitializeSurfaceQuad(width, height));
	CN(InitializeAddressBar(width * m_borderWidth));

//	pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), ELEMENT_COLLIDE_BEGAN, this);

	m_ptVisiblePosition = point(0.0f, 0.0f, 0.0f);
	m_qViewQuadOrientation = quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, 0.0f);

	m_pViewQuad->SetMaterialAmbient(0.75f);
	m_pViewQuad->FlipUVVertical();
	CR(m_pViewQuad->SetVisible(false));

	m_pViewQuad->SetDiffuseTexture(m_pLoadingScreenTexture);

	m_pViewBackground = AddQuad(width * m_borderWidth, width * m_borderHeight);
	CB(m_pViewBackground);
	m_pBackgroundTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, L"control-view-main-background.png");
	m_pViewBackground->SetDiffuseTexture(m_pBackgroundTexture);

	m_pViewBackground->SetPosition(point(0.0f, -0.0005f, 0.0f));

Error:
	return r;
}

RESULT UIControlView::InitializeAddressBar(float width) {
	RESULT r = R_PASS;

	float barPXWidth = LOCK_PX_WIDTH + BACKGROUND_PX_WIDTH;
	float backgroundWidth = (BACKGROUND_PX_WIDTH) / barPXWidth * width;
	float lockWidth = (LOCK_PX_WIDTH) / barPXWidth * width;
	float barHeight = (ADDRESS_PX_HEIGHT) / barPXWidth * width;
	float spacingSize = (30.0f) / barPXWidth * width;
	float lineHeight = (44.0f) / barPXWidth * width;
	float textOffset = (-4.0f) / barPXWidth * width;

	float height = -m_pViewQuad->GetWidth() * m_borderHeight / 2.0f - barHeight/2.0f - spacingSize;

	point ptLock = point(-width / 2.0f + lockWidth / 2.0f, 0.0f, 0.0f);
	point ptBackground = point(-width/2.0f + lockWidth + backgroundWidth/2.0f, 0.0f, 0.0f);

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	// Initialize Address Bar
	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	m_pAddressSecureTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrAddressSecure)[0]);
	m_pAddressInsecureTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrAddressInsecure)[0]);
	m_pAddressBackgroundTexture = m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrAddressBackground)[0]);

	m_pAddressBar = AddUIView();
	CN(m_pAddressBar);
	m_pAddressBar->SetPosition(point(0.0, 0.0, height));
	m_pAddressBar->SetVisible(false, false);

	if (m_pFont == nullptr) {
		m_pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		CN(m_pFont);
		m_pFont->SetLineHeight(0.06f);
	}

	m_pFont->SetLineHeight(lineHeight);
	m_pAddressText = std::shared_ptr<text>(m_pDreamOS->MakeText(
		m_pFont,
		"",
		backgroundWidth,
		lineHeight, 
		text::flags::RENDER_QUAD));
	m_pAddressText->SetPosition(ptBackground + point(textOffset, 0.001f, 0.0f));

	m_pAddressBar->AddObject(m_pAddressText);

	m_pAddressSecurityQuad = m_pAddressBar->AddQuad(lockWidth, barHeight);
	CN(m_pAddressSecurityQuad);
	m_pAddressSecurityQuad->SetDiffuseTexture(m_pAddressInsecureTexture);
	m_pAddressSecurityQuad->SetPosition(ptLock);

	m_pAddressBackgroundQuad = m_pAddressBar->AddQuad(backgroundWidth, barHeight);
	CN(m_pAddressBackgroundQuad);
	m_pAddressBackgroundQuad->SetDiffuseTexture(m_pAddressBackgroundTexture);
	m_pAddressBackgroundQuad->SetPosition(ptBackground);

Error:
	return r;
}

RESULT UIControlView::Update() {
	RESULT r = R_PASS;	

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);
		
	hand* pLHand;
	pLHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT);
	CNR(pLHand, R_SKIPPED);

	hand* pRHand;
	pRHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
	CNR(pRHand, R_SKIPPED);	

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
		hand *pHand;
		HAND_TYPE type;
		if (i == 0) {
			pHand = pLHand;
			type = HAND_TYPE::HAND_LEFT;
		}
		else {
			pHand = pRHand;
			type = HAND_TYPE::HAND_RIGHT;
		}

		bool fMalletDirty = m_fMalletDirty[i].IsDirty();

		UpdateWithHand(pHand, fMalletDirty, m_fMouseDown[i], type);

		if (fMalletDirty) {
			m_fMalletDirty[i].SetDirty();
		} 
		else {
			m_fMalletDirty[i].CheckAndCleanDirty();
		}
	}

	if (m_fUpdateAddressBarText) {
		CR(m_pAddressText->SetText(m_strCurrentURL));
		m_fUpdateAddressBarText = false;
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

RESULT UIControlView::SetViewQuadTexture(texture* pBrowserTexture) {
	m_pViewQuad->SetDiffuseTexture(pBrowserTexture);	//Control view texture to be set by Browser
	return R_PASS;
}

RESULT UIControlView::SetURLText(std::string strURL) {
	RESULT r = R_PASS;

	m_strCurrentURL = strURL;
	m_fUpdateAddressBarText = true;

Error:
	return r;
}

RESULT UIControlView::SetSchemeText(std::string strScheme) {
	m_strCurrentScheme = strScheme;
	return R_PASS;
}

RESULT UIControlView::SetURLSecurity(bool fSecure) {
	RESULT r = R_PASS;

	if (fSecure) {
		CR(m_pAddressSecurityQuad->SetDiffuseTexture(m_pAddressSecureTexture));
	}
	else {
		CR(m_pAddressSecurityQuad->SetDiffuseTexture(m_pAddressInsecureTexture));
	}	

Error:
	return r;
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
		m_pViewBackground->SetVisible(true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		
		std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
		CNR(pDreamUserApp, R_SKIPPED);

		pDreamUserApp->SetEventApp(this);

		hand* pLHand;
		pLHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT);
		CNR(pLHand, R_SKIPPED);

		hand* pRHand;
		pRHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
		CNR(pRHand, R_SKIPPED);

		pLHand->SetDirty();
		pRHand->SetDirty();

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

RESULT UIControlView::Show() {
	RESULT r = R_PASS;

	//CR(ResetAppComposite());
	SetVisible(true, false);
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

RESULT UIControlView::HideView() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		m_pViewBackground->SetVisible(false);
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

	bool fViewAnimating = pProxy->IsAnimating(this);
	bool fQuadAnimating = pProxy->IsAnimating(m_pViewQuad.get());

	return fViewAnimating || fQuadAnimating;
}

RESULT UIControlView::SetKeyboardAnimationDuration(float animationDuration) {
	m_keyboardAnimationDuration = animationDuration;
	return R_PASS;
}

RESULT UIControlView::HandleKeyboardDown() {
	RESULT r = R_PASS;
	
	CR(m_pDreamOS->GetKeyboardApp()->Hide());
	CR(FlipViewDown());

Error:
	return r;
}

RESULT UIControlView::HandleKeyboardUp(ContentType type) {
	RESULT r = R_PASS;

	std::shared_ptr<UIKeyboard> pKeyboardApp = m_pDreamOS->GetKeyboardApp();
	CNR(pKeyboardApp, R_SKIPPED);

	CBR(IsVisible(), R_SKIPPED);
	CBR(!IsAnimating(), R_SKIPPED);

	CR(FlipViewUp());

	CBR(!pKeyboardApp->IsVisible(), R_SKIPPED);
	CR(pKeyboardApp->Show());

	switch (type) {
	case ContentType::FORM: {
		pKeyboardApp->GetCancelButton()->SetVisible(true);
	} break;
	case ContentType::DEFAULT: {
		pKeyboardApp->GetCancelButton()->SetVisible(false);
	} break;
	}

Error:
	return r;
}

RESULT UIControlView::FlipViewDown() {
	RESULT r = R_PASS;

	m_pAddressBar->SetVisible(false, false);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
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

RESULT UIControlView::FlipViewUp() {
	RESULT r = R_PASS;

	// Position the ControlView behind the keyboard with a slight height offset (center should be above keyboard textbox).
	point ptTypingPosition;
	point ptTypingOffset;

	float viewHeight = m_pViewQuad->GetHeight();
	float textBoxYOffset = viewHeight/2.0f;
	float verticalAngle;

	std::shared_ptr<DreamUserApp> pDreamUserApp = m_pDreamOS->GetUserApp();
	CNR(pDreamUserApp, R_SKIPPED);

	m_pAddressBar->SetVisible(true, false);
	// currently always fully shown

	// 58 degrees is the old typing angle (straight up), move the y offset down to accommodate
	// tilting the screen up
	verticalAngle = (90.0f - pDreamUserApp->GetViewAngle()) * M_PI / 180.0;
	textBoxYOffset *= 1.0f - sin(verticalAngle - TYPING_ANGLE); 

	ptTypingOffset = point(0.0f, 0.0f, -m_pViewBackground->GetHeight() * 0.5f);	// so that it'll appear past the keyboard quad

	ptTypingPosition = ptTypingOffset +point(0.0f, sin(TYPING_ANGLE) * textBoxYOffset, -cos(TYPING_ANGLE) * textBoxYOffset);

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
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


float UIControlView::GetBackgroundWidth() {
	return m_pViewBackground->GetWidth();
}