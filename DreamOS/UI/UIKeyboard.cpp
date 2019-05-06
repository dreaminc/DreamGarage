#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

#include "UIKey.h"

#include "UI/UIButton.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"
#include "Primitives/hand/hand.h"

#include "Sound/SoundFile.h"

#include "Sandbox/PathManager.h"

UIKeyboard::UIKeyboard(DreamOS *pDreamOS, void *pContext) :
	DreamApp<UIKeyboard>(pDreamOS, pContext)
{
	// empty
}

RESULT UIKeyboard::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<font> pFont;
	std::wstring wstrFont;

	SetAppName("UIKeyboard");
	SetAppDescription("Virtual text entry");

	GetDOS()->AddObjectToUIGraph(GetComposite());
	// Register keyboard events

	auto pSenseKeyboardPublisher = dynamic_cast<Publisher<SenseVirtualKey, SenseKeyboardEvent>*>(this);
	CR(pSenseKeyboardPublisher->RegisterSubscriber(SVK_ALL, GetDOS()->GetInteractionEngineProxy()));

	m_pHeaderContainer = GetComposite()->AddComposite();

	m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	m_pFont->SetLineHeight(m_lineHeight);

	// textures for keycaps and objects
	m_pKeyTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"key-background.png");
	m_pTextBoxTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"text-input-background.png");

	m_pDeleteTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-delete-background.png");
	m_pLettersTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-abc-background.png");
	m_pNumbersTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-123-background.png");
	m_pReturnTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-return-background.png");
	m_pShiftTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-shift-background.png");
	m_pSpaceTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-space-background.png");
	m_pSymbolsTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-symbol-background.png");
	m_pUnshiftTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"Keycaps\\key-unshift-background.png");
	m_pDefaultIconTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, L"website.png");

	m_pDefaultPressSound = std::shared_ptr<SoundFile>(SoundFile::LoadSoundFile(L"sound-keyboard-standard.wav", SoundFile::type::WAVE));
	CN(m_pDefaultPressSound);

	m_pDeletePressSound = std::shared_ptr<SoundFile>(SoundFile::LoadSoundFile(L"sound-keyboard-delete.wav", SoundFile::type::WAVE));
	CN(m_pDeletePressSound);

	m_pReturnPressSound = std::shared_ptr<SoundFile>(SoundFile::LoadSoundFile(L"sound-keyboard-return.wav", SoundFile::type::WAVE));
	CN(m_pReturnPressSound);

	m_pSpacePressSound = std::shared_ptr<SoundFile>(SoundFile::LoadSoundFile(L"sound-keyboard-spacebar.wav", SoundFile::type::WAVE));
	CN(m_pSpacePressSound);

	m_keyPressSounds = {
		{SVK_BACK, m_pDeletePressSound},
		{SVK_RETURN, m_pReturnPressSound},
		{0x20, m_pSpacePressSound} // 0x20 is space bar
	};

	m_keyObjects[0] = nullptr;
	m_keyObjects[1] = nullptr;

	// pre-load possible keyboard layers 
	InitializeLayoutTexture(LayoutType::QWERTY);
	InitializeLayoutTexture(LayoutType::QWERTY_UPPER);
	InitializeLayoutTexture(LayoutType::QWERTY_NUM);
	InitializeLayoutTexture(LayoutType::QWERTY_SYMBOL);

Error:
	return r;
}

RESULT UIKeyboard::InitializeWithParent(DreamUserControlArea *pParent) {
	RESULT r = R_PASS;

	CBR(m_pParentApp == nullptr, R_SKIPPED);
	m_pParentApp = pParent;

	auto pLayout = new UIKeyboardLayout();
	pLayout->CreateQWERTYLayout(); // should be in constructor probably
	float keyDimension = (m_surfaceWidth) / (float)pLayout->GetKeys()[0].size();

	// after being placed, the keys are scaled down by m_keyScale to create gaps in between the quads.
	// need to increase the overall surface width to accommodate for this, and decrease the width of the text box
	float marginError = keyDimension * (1 - m_keyScale);
	m_surfaceWidth *= m_pParentApp->GetBaseWidth();
	m_surfaceWidth += marginError;
	m_surfaceHeight = m_surfaceWidth * 0.4f;


	//TODO this may become deprecated
	m_qSurfaceOrientation = quaternion::MakeQuaternionWithEuler(m_surfaceAngle * (float)(M_PI) / 180.0f, 0.0f, 0.0f);

	m_pSurfaceContainer = GetComposite()->AddComposite();
	//m_pSurfaceContainer->SetOrientation(m_qSurfaceOrientation);

	m_pSurface = m_pSurfaceContainer->AddQuad(m_surfaceHeight, m_surfaceWidth);
	CN(m_pSurface);

	m_pSurface->SetVisible(false);
	CR(m_pSurface->InitializeOBB()); // TODO: using the default BoundingQuad could potentially be better

	// position keyboard composite
	//float kbOffset = -(-m_surfaceHeight + pParent->GetTotalHeight() + marginError) / 2.0f;
	float kbOffset = 0.0f;
	//GetComposite()->SetPosition(pParent->GetCenter() + point(-pParent->GetCenterOffset(), 0.0f, kbOffset));
	GetComposite()->SetPosition(pParent->GetCenter());
	m_ptComposite = GetComposite()->GetPosition();

	{
		//Setup textbox
		float offset = m_surfaceHeight / 2.0f;
		float angle = (m_pParentApp->GetViewAngle()) * (float)(M_PI) / 180.0f;

		float textBoxWidth = m_surfaceWidth - marginError;

		m_pHeaderContainer->RotateXByDeg(90.0f - m_pParentApp->GetViewAngle());

		float quadHeight = m_lineHeight * m_numLines * 1.5f;
		m_pTextBoxBackground = m_pHeaderContainer->AddQuad(textBoxWidth, quadHeight, point(0.0f, -0.001f, 0.0f));
		m_pTextBoxBackground->SetDiffuseTexture(m_pTextBoxTexture.get());

		m_pFont->SetLineHeight(m_lineHeight);
		m_pTextBoxText = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			"",
			textBoxWidth - m_textboxMargin,
			m_lineHeight * m_numLines, 
			text::flags::LEAD_ELLIPSIS | text::flags::RENDER_QUAD));

		m_pHeaderContainer->AddObject(m_pTextBoxText);

		// position near the top of the keyboard
		m_pHeaderContainer->SetPosition(point(0.0f, 0.0f, -m_surfaceHeight/2.0f -m_pTextBoxBackground->GetHeight() / 2.0f));

		// position near the top of the control area
		//m_pHeaderContainer->SetPosition(point(0.0f, 0.0f, -GetComposite()->GetPosition().z() -pParent->GetTotalHeight() / 2.0f));

		float scale = m_pTextBoxBackground->GetHeight() / 2.0f;
		m_pHeaderContainer->SetPosition(m_pHeaderContainer->GetPosition() + point(0.0f, sin(angle)*scale, -cos(angle)*scale));
		

		//Setup title / icon
		float iconWidth = quadHeight * (4.0f / 3.0f);
		point ptTitle = point((-m_surfaceWidth + iconWidth + marginError) / 2.0f, 0.0f, -(quadHeight + m_pParentApp->GetSpacingSize()));
		m_pTitleIcon = m_pHeaderContainer->AddQuad(iconWidth, quadHeight);
		m_pTitleIcon->SetPosition(ptTitle);
		
		m_pTitleIcon->SetDiffuseTexture(m_pDefaultIconTexture.get());

		m_pTitleText = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			"Website",
			m_surfaceWidth,
			m_lineHeight,
			text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));
		m_pTitleText->SetPosition(point(m_pTitleIcon->GetWidth() + m_pParentApp->GetSpacingSize(), 0.0f, m_pTitleIcon->GetPosition().z()));
		m_pHeaderContainer->AddObject(m_pTitleText);


		auto pView = m_pSurfaceContainer->AddUIView(GetDOS());
		m_pKeyboardControls = pView->AddUIView();

		//m_pUIControlBar->RegisterObserver(this);
		m_pKeyboardControls->SetVisible(false, false);
	}

	CR(InitializeQuadsWithLayout(pLayout));
	m_pLayout = pLayout;

	CR(InitializeKeyboardControls());

	m_currentLayout = LayoutType::QWERTY;

	GetComposite()->SetVisible(false, false);
Error:
	return r;
}

RESULT UIKeyboard::InitializeLayoutTexture(LayoutType type) {
	RESULT r = R_PASS;

	std::shared_ptr<UIKeyboardLayout> pLayout = std::make_shared<UIKeyboardLayout>(type);
	bool fUpper = (type == LayoutType::QWERTY_UPPER || type == LayoutType::QWERTY_SYMBOL);
	bool fNum = (type == LayoutType::QWERTY_NUM || type == LayoutType::QWERTY_SYMBOL);

	pLayout->CreateQWERTYLayout(fUpper, fNum);
	pLayout->SetKeyTexture(m_pKeyTexture.get());

	pLayout->AddToSpecialTextures(SVK_SPACE, m_pSpaceTexture.get());
	pLayout->AddToSpecialTextures(SVK_RETURN, m_pReturnTexture.get());
	pLayout->AddToSpecialTextures(SVK_BACK, m_pDeleteTexture.get());
	
	switch (type) {
	case LayoutType::QWERTY: {
		pLayout->AddToSpecialTextures(SVK_SHIFT, m_pShiftTexture.get());
		pLayout->AddToSpecialTextures(SVK_CONTROL, m_pNumbersTexture.get());
	} break;
	case LayoutType::QWERTY_UPPER: {
		pLayout->AddToSpecialTextures(SVK_SHIFT, m_pUnshiftTexture.get());
		pLayout->AddToSpecialTextures(SVK_CONTROL, m_pNumbersTexture.get());
	} break;
	case LayoutType::QWERTY_NUM: {
		pLayout->AddToSpecialTextures(SVK_SHIFT, m_pSymbolsTexture.get());
		pLayout->AddToSpecialTextures(SVK_CONTROL, m_pLettersTexture.get());
	} break;
	case LayoutType::QWERTY_SYMBOL: {
		pLayout->AddToSpecialTextures(SVK_SHIFT, m_pNumbersTexture.get());
		pLayout->AddToSpecialTextures(SVK_CONTROL, m_pLettersTexture.get());
	} break;
	}

	// this calculation seems weird, but has a value similar to the "fudge factor" in AddGlyphQuad
	pLayout->SetRowHeight((1.0f / (float)pLayout->GetKeys()[0].size()));// / m_keyScale);

	m_pFont->SetLineHeight(m_lineHeight);
	auto pText = GetDOS()->MakeText(m_pFont, pLayout.get(), m_keyMargin, text::flags::NONE);
	CN(pText);
	pText->RenderToQuad();

	m_layoutAtlas[type] = pText;

Error:
	return r;
}

RESULT UIKeyboard::InitializeQuadsWithLayout(UIKeyboardLayout* pLayout) {
	RESULT r = R_PASS;
	
	auto& pLayoutKeys = pLayout->GetKeys();
	//not flexible, TODO: take max of rows?
	float keyDimension = m_surfaceWidth / (float)pLayoutKeys[0].size();

	int rowIndex = 0;
	int colIndex = 0;
	for (auto& layoutRow : pLayoutKeys) {
		for (auto& pKey : layoutRow) {

			float uvLeft = pKey->m_left;// / (m_surfaceWidth * 2.0f);
			float uvRight = (pKey->m_left + pKey->m_width);// / (m_surfaceWidth * 2.0f);

			float rowCount = (float)pLayoutKeys.size();
			float uvTop = (rowIndex / rowCount);
			float uvBottom = (((rowIndex + 1) / rowCount));

			float zPos = (m_surfaceHeight / rowCount) * (rowIndex - (rowCount / 2.0f) + 0.5f);

			float xPos = m_surfaceWidth * (pKey->m_left + pKey->m_width / 2.0f) - (m_surfaceWidth / 2.0f);

			point ptOrigin = point(xPos, 0.0f, zPos) + m_pSurface->GetPosition();

			std::shared_ptr<quad> pQuad = m_pSurfaceContainer->AddQuad(
				keyDimension,
				keyDimension,
				ptOrigin,
				uvcoord(uvLeft, uvTop),
				uvcoord(uvRight, uvBottom));
			CN(pQuad);
			pQuad->SetPosition(ptOrigin);

			pQuad->SetDiffuseTexture(m_layoutAtlas[pLayout->GetLayoutType()]->GetFramebuffer()->GetColorTexture());

			// Set up key quad positioning
			//pQuad->ScaleX((m_keyScale * 0.5f*pKey->m_width) / keyDimension);
			//pQuad->ScaleY(m_keyScale);
			pQuad->ScaleX(pKey->m_width * (float)pLayoutKeys[0].size() - (1.0f - m_keyScale));
			pQuad->ScaleZ(pQuad->GetScale().z() * m_keyScale);

			pQuad->SetMaterialAmbient(m_ambientIntensity);

			pKey->m_pQuad = pQuad;
			//pKey->m_pQuad->SetVisible(false);
			colIndex++;
		}
		colIndex = 0;
		rowIndex++;
	}

Error:
	return r;
}

RESULT UIKeyboard::InitializeKeyboardControls() {
	RESULT r = R_PASS;

	// sizing specific to keyboard
	float keyDimension = m_surfaceWidth / (float)m_pLayout->GetKeys()[0].size();
	float marginError = keyDimension * (1 - m_keyScale);
	float buttonWidth = keyDimension;
	float itemSide = keyDimension * m_keyScale;
	float barButtonWidth = 2.0f * itemSide + marginError;

	float left = -m_surfaceWidth / 2.0f + marginError / 2.0f;
	float backTabOffset = left + barButtonWidth/2.0f;
	float tabOffset = backTabOffset + barButtonWidth + marginError;

	float right = m_surfaceWidth / 2.0f - marginError / 2.0f;
	float cancelOffset = right - barButtonWidth/2.0f;
	//float cancelOffset = doneOffset - barButtonWidth - marginError;

	m_pKeyboardControls->RotateXByDeg(-90.0f);
	m_pKeyboardControls->SetPosition(point(0.0f, 0.0f, -(m_surfaceHeight + buttonWidth) / 2.0f));

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	// Initialize Address Bar
	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	auto pTab = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszTab));
	auto pCantTab = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszCantTab));
	auto pBackTab = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszBackTab));
	auto pCantBackTab = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszCantBackTab));
	auto pCancel = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrCancel)[0]));

	//TODO: use when controlbar is deprecated
	auto pDone = std::shared_ptr<texture>(GetDOS()->MakeTexture(texture::type::TEXTURE_2D, k_wszDone));

	CN(pTab);
	CN(pCantTab);
	CN(pBackTab);
	CN(pCantBackTab);
	CN(pDone);

	m_pNextButton = m_pKeyboardControls->AddButton(tabOffset, barButtonWidth, itemSide,
		std::bind(&UIKeyboard::HandleTabPressed, this, std::placeholders::_1, std::placeholders::_2),
		pTab, pCantTab);

	m_pPreviousButton = m_pKeyboardControls->AddButton(backTabOffset, barButtonWidth, itemSide,
		std::bind(&UIKeyboard::HandleBackTabPressed, this, std::placeholders::_1, std::placeholders::_2),
		pBackTab, pCantBackTab);

	/*
	m_pDoneButton = m_pKeyboardControls->AddButton(doneOffset, barButtonWidth, itemSide,
		std::bind(&UIKeyboard::HandleDonePressed, this, std::placeholders::_1, std::placeholders::_2),
		pDone);
		//*/

	m_pCancelButton = m_pKeyboardControls->AddButton(cancelOffset, barButtonWidth, itemSide,
		std::bind(&UIKeyboard::HandleCancelPressed, this, std::placeholders::_1, std::placeholders::_2),
		pCancel);

	CN(m_pNextButton);
	CN(m_pPreviousButton);
	//CN(m_pDoneButton);
	CN(m_pCancelButton);

	m_pNextButton->SetVisible(true);
	m_pPreviousButton->SetVisible(true);
	//m_pDoneButton->SetVisible(true);
	m_pCancelButton->SetVisible(true);

Error:
	return r;
}

RESULT UIKeyboard::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT UIKeyboard::Update(void *pContext) {
	RESULT r = R_PASS;

	RotationMatrix qOffset = RotationMatrix();
	VirtualObj *pObj = nullptr;

	InteractionEngineProxy *pProxy = nullptr;
	DreamOS *pDreamOS = GetDOS();

	UIKey *keyCollisions[2];
	keyCollisions[0] = nullptr;
	keyCollisions[1] = nullptr;
	point ptCollisions[2];
	point ptCollision;

	std::vector<UIKey*> activeKeysToRemove;

	// skip keyboard interaction if not visible
	//CBR(m_keyboardState == UIKeyboard::state::VISIBLE, R_SKIPPED);
	CBR(GetComposite()->IsVisible(), R_SKIPPED);

	CN(pDreamOS);
	pProxy = pDreamOS->GetInteractionEngineProxy();
	CN(pProxy);

	hand* pLHand = GetDOS()->GetHand(HAND_TYPE::HAND_LEFT);
	CNR(pLHand, R_SKIPPED);
	hand* pRHand = GetDOS()->GetHand(HAND_TYPE::HAND_RIGHT);
	CNR(pRHand, R_SKIPPED);

	CBR(m_pSurface, R_SKIPPED);

	// Update Keys if the app is active
	int i = 0;

	//  Note: this predictive collision functionality is duplicated in control view
	for (auto &hand : { pLHand, pRHand })
	{
		point ptBoxOrigin = m_pSurface->GetOrigin(true);
		point ptSphereOrigin = hand->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(m_pSurface->GetOrientation(true))) * (ptSphereOrigin - m_pSurface->GetOrigin(true)));
		ptCollisions[i] = ptSphereOrigin;

		if (ptSphereOrigin.y() >= hand->GetMalletRadius()) hand->CheckAndCleanDirty();
		//else if (ptSphereOrigin.y() < m_keyReleaseThreshold) mallet->SetDirty();
		
		// if the sphere is lower than its own radius, there must be an interaction
		if (ptSphereOrigin.y() < hand->GetMalletRadius() && !hand->IsDirty()) {

			//TODO: CollisionPointToKey returns one key based on the center of the sphere
			// if it accounted for the radius, it would be able to return multiple keys
			auto key = CollisionPointToKey(ptSphereOrigin);
			if (!key) {
				CR(hand->SetDirty());
				continue;
			}
			CR(AddActiveKey(key));
			keyCollisions[i] = key;

			point ptPosition = key->m_pQuad->GetPosition();

			// TODO: edge case where there is a new interaction during the key release animation
			key->m_pQuad->SetPosition(point(ptPosition.x(), ptSphereOrigin.y() - hand->GetMalletRadius(), ptPosition.z()));
		}

		i++;
	}

	// covers edge case where both mallets are hitting the same key
	if (keyCollisions[0] != nullptr && keyCollisions[1] != nullptr && keyCollisions[0] == keyCollisions[1]) {

		auto key = keyCollisions[0];
		point ptPosition = key->m_pQuad->GetPosition();
		float y = std::min(ptCollisions[0].y(), ptCollisions[1].y());
		key->m_pQuad->SetPosition(point(ptPosition.x(), y - pLHand->GetMalletRadius(), ptPosition.z()));
	}


	for (auto key : m_activeKeys) {

		// get collision point and check that key is active
		bool fActive = false;
		ControllerType controllerType;
		hand *pHand = nullptr;
		for (int j = 0; j < 2; j++) {
			auto k = keyCollisions[j];
			if (key == k) {
				ptCollision = ptCollisions[j];
				fActive = true;
				controllerType = (ControllerType)(j);
				pHand = (j == 0) ? pLHand : pRHand;
			}
		}

		if (!fActive) {
			key->m_state = KeyState::KEY_NOT_INTERSECTED;
			activeKeysToRemove.emplace_back(key);
			continue;
		}
		
		switch (key->m_state) {

		case KeyState::KEY_DOWN: {
			if (ptCollision.y() > m_keyTypeThreshold) key->m_state = KeyState::KEY_MAYBE_UP;
			else if (ptCollision.y() < m_keyReleaseThreshold) {
				ReleaseKey(key);
				if (pHand != nullptr)
					pHand->SetDirty();
			}
			//else key->m_state = KeyState::KEY_DOWN;
		} break;

		case KeyState::KEY_MAYBE_DOWN: {
			if (ptCollision.y() < m_keyTypeThreshold) {
				CR(UpdateKeyState((SenseVirtualKey)key->m_letter, 1));
				CR(PressKey(key, controllerType));
				key->m_state = KeyState::KEY_DOWN;
			}
			else key->m_state = KeyState::KEY_UP;
		} break;

		case KeyState::KEY_MAYBE_UP: {
			if (ptCollision.y() > m_keyTypeThreshold) {
				CR(UpdateKeyState((SenseVirtualKey)(key->m_letter), 0));
				key->m_state = KeyState::KEY_UP;
			}
			else key->m_state = KeyState::KEY_DOWN;
		} break;

		case KeyState::KEY_UP: {
			if (ptCollision.y() < m_keyTypeThreshold) key->m_state = KeyState::KEY_MAYBE_DOWN;
			//else key->m_state = KeyState::KEY_UP;
		} break;

		default: break;
		}

	}

	for (auto key : activeKeysToRemove) {
		ReleaseKey(key);
		RemoveActiveKey(key);
		key->m_state = KeyState::KEY_UP;
	}
	activeKeysToRemove.clear();

Error:
	return r;
}

RESULT UIKeyboard::Shutdown(void *pContext) {
	return R_PASS;
}

UIKeyboard* UIKeyboard::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	UIKeyboard *pUIKeyboard = new UIKeyboard(pDreamOS, pContext);
	return pUIKeyboard;
}

RESULT UIKeyboard::Show() {
	RESULT r = R_PASS;


	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		GetComposite()->SetPosition(m_ptComposite - point(0.0f, m_animationOffsetHeight, 0.0f));
		pKeyboard->GetComposite()->SetVisible(true, false);
		//pKeyboard->HideSurface();
		m_pSurfaceContainer->SetVisible(true, false);
		
		m_pHeaderContainer->SetVisible(false);
		//m_pTitleIcon->SetVisible(false);
		//m_pTitleText->SetVisible(false);

	Error:
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;

		std::shared_ptr<DreamUserApp> pDreamUserApp = nullptr;

		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		CR(UpdateKeyState((SenseVirtualKey)(0), 1));	// To refresh textbox
		CR(UpdateKeyState((SenseVirtualKey)(0), 0));

		pDreamUserApp = GetDOS()->GetUserApp();
		CNR(pDreamUserApp, R_SKIPPED);

		hand* pLHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_LEFT);
		CNR(pLHand, R_SKIPPED);
		hand* pRHand = pDreamUserApp->GetHand(HAND_TYPE::HAND_RIGHT);
		CNR(pRHand, R_SKIPPED);

		pLHand->SetDirty();
		pRHand->SetDirty();

	Error:
		return r;
	};

	DimObj *pObj = GetComposite();
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		m_ptComposite,
		pObj->GetOrientation(),// * m_qSurfaceOrientation,
		pObj->GetScale(),
		color(1.0f, 1.0f, 1.0f, 1.0f),
		m_animationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
Error:
	return r;
}

RESULT UIKeyboard::Hide() {

	RESULT r = R_PASS;

	auto fnCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		pKeyboard->GetComposite()->SetVisible(false, false);
		m_pKeyboardControls->SetVisible(false, false);
		// full press of key that clears whole string
		CR(UpdateKeyState((SenseVirtualKey)(0x01), 0));
		CR(UpdateKeyState((SenseVirtualKey)(0x01), 1));

		CR(UpdateKeyboardLayout(LayoutType::QWERTY));

	Error:
		return r;
	};

	DimObj *pObj = GetComposite();

	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition() - point(0.0f, m_animationOffsetHeight, 0.0f),
		pObj->GetOrientation(),
		pObj->GetScale(),
		color(1.0f, 1.0f, 1.0f, 0.0f),
		m_animationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		nullptr,
		fnCallback,
		this
	);

	return r;
}

bool UIKeyboard::IsVisible() {
	return GetComposite()->IsVisible();
}

RESULT UIKeyboard::SetVisible(bool fVisible) {
	return GetComposite()->SetVisible(fVisible, false);
}

RESULT UIKeyboard::PressKey(UIKey *pKey, ControllerType type) {
	RESULT r = R_PASS;

	if (m_keyPressSounds.count(pKey->m_letter) > 0) {
		CR(GetDOS()->PlaySoundFile(m_keyPressSounds[pKey->m_letter]));
	}
	else {
		CR(GetDOS()->PlaySoundFile(m_pDefaultPressSound));
	}

	CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(type, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));

Error:
	return r;
}

RESULT UIKeyboard::ReleaseKey(UIKey *pKey) {	
	RESULT r = R_PASS;
	
	auto pObj = pKey->m_pQuad;
	point ptPosition = pObj->GetPosition();

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj.get(),
		point(ptPosition.x(), 0.00f, ptPosition.z()),
		pObj->GetOrientation(),
		pObj->GetScale(),
		m_keyReleaseDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags()
	));

	//TODO: this should probably be moved, because it is possible to send a 
	// key up event without a key down event.
	// this only needs to happen when ReleaseKey is called when the previous
	// state was KEY_DOWN
	CR(UpdateKeyState((SenseVirtualKey)(pKey->m_letter), 0));

Error:
	return r;
}

// this function assumes the key height is constant
UIKey* UIKeyboard::CollisionPointToKey(point ptCollision) {
	RESULT r = R_PASS;

	auto& keyboardLayout = m_pLayout->GetKeys();

	float rowIndex = (ptCollision.z() + (m_surfaceHeight / 2.0f)) / m_surfaceHeight * keyboardLayout.size();
	CBR(rowIndex >= 0 && rowIndex < keyboardLayout.size(), R_OBJECT_NOT_FOUND);

	float xPos = (ptCollision.x() + (m_surfaceWidth / 2.0f)) / m_surfaceWidth;
	auto& row = keyboardLayout[(int)rowIndex];

	for (int i = (int)row.size() - 1; i >= 0; i--) {
		auto k = row[i];
		float pos = xPos;
		if (k->m_left <= pos) {
			CBR(k->m_left + k->m_width >= pos, R_OBJECT_NOT_FOUND);
			return k.get();
		}
	}

Error:
	return nullptr;
}

RESULT UIKeyboard::UpdateKeyStates() {
	RESULT r = R_PASS;
	CB((GetKeyboardState(m_KeyStates)));
Error:
	return r;
}

RESULT UIKeyboard::UpdateKeyState(SenseVirtualKey key, uint8_t keyState) {
	if (keyState == 1) {
		UpdateTextBox(key);
	}
	return SetKeyState(key, keyState);
}

RESULT UIKeyboard::CheckKeyState(SenseVirtualKey key) {
	return R_NOT_IMPLEMENTED;
}

RESULT UIKeyboard::UpdateKeyboardLayout(LayoutType kbType) {
	RESULT r = R_PASS;

	m_pLayout->UpdateKeysWithLayout(kbType);

	for (auto layoutRow : m_pLayout->GetKeys()) {
		for (auto& pKey : layoutRow) {
			CR(pKey->m_pQuad->SetDiffuseTexture(m_layoutAtlas[kbType]->GetFramebuffer()->GetColorTexture()));
		}
	}

	m_currentLayout = kbType;
	
Error:
	return r;
}

std::string UIKeyboard::GetText() {
	return m_pTextBoxText->GetText();
}

RESULT UIKeyboard::UpdateTextBox(int chkey) {
	RESULT r = R_PASS;

	//TODO: this logic should probably be in UIKeyboardLayout
	if (chkey == SVK_SHIFT) {
		LayoutType newType;
		switch (m_currentLayout) {
		case LayoutType::QWERTY: newType = LayoutType::QWERTY_UPPER; break;
		case LayoutType::QWERTY_UPPER: newType = LayoutType::QWERTY; break;
		case LayoutType::QWERTY_NUM: newType = LayoutType::QWERTY_SYMBOL; break;
		case LayoutType::QWERTY_SYMBOL: newType = LayoutType::QWERTY_NUM; break;
		}
		CR(UpdateKeyboardLayout(newType));
	}

	else if (chkey == SVK_RETURN) {
		std::shared_ptr<DreamUserApp> pDreamUserApp = GetDOS()->GetUserApp();
		CN(pDreamUserApp);
		pDreamUserApp->HandleUserObserverEvent(UserObserverEventType::KB_ENTER);

		m_pTextBoxText->SetText("");
	}

	else if (chkey == 0x01) {
		m_pTextBoxText->SetText("");
	}
	
	// TODO: better way to refresh textbox using text dirty flag
	else if (chkey == 0) {
		auto strCurrentText = m_pTextBoxText->GetText();
		m_pTextBoxText->SetText("");
		m_pTextBoxText->SetText(strCurrentText);
	}

	else if (chkey == SVK_BACK) {
		auto strTextbox = m_pTextBoxText->GetText();
		if (strTextbox.size() > 0) {
			strTextbox.pop_back();
			m_pTextBoxText->SetText(strTextbox);
		}
	}

	else if (chkey == SVK_CONTROL) {
		LayoutType newType;
		switch (m_currentLayout) {
		case LayoutType::QWERTY: newType = LayoutType::QWERTY_NUM; break;
		case LayoutType::QWERTY_UPPER: newType = LayoutType::QWERTY_NUM; break;
		case LayoutType::QWERTY_NUM: newType = LayoutType::QWERTY; break;
		case LayoutType::QWERTY_SYMBOL: newType = LayoutType::QWERTY; break;
		}
		CR(UpdateKeyboardLayout(newType));

	}	

	else {
		std::string strNew = m_pTextBoxText->GetText();
		strNew += chkey;
		m_pTextBoxText->SetText(strNew);	

		if (m_currentLayout == LayoutType::QWERTY_UPPER) {
			CR(UpdateKeyState((SenseVirtualKey)(chkey), 0));
			CR(UpdateKeyboardLayout(LayoutType::QWERTY));
		}
	}

Error:
	return r;
}

RESULT UIKeyboard::PopulateKeyboardTextBox(std::string strText) {
	RESULT r = R_PASS;
	CR(m_pTextBoxText->SetText(strText));
Error:
	return r;
}

RESULT UIKeyboard::UpdateTitleView(texture *pIconTexture, std::string strTitle) {
	RESULT r = R_PASS;
	m_pHeaderContainer->SetVisible(true);
	//m_pTitleIcon->SetVisible(true);
	//m_pTitleText->SetVisible(true);
	
	if (pIconTexture != nullptr) {
		CR(m_pTitleIcon->SetDiffuseTexture(pIconTexture));
	}
	m_pTitleText->SetText(strTitle);

Error:
	return r;
}

RESULT UIKeyboard::ShowTitleView() {
	RESULT r = R_PASS;
	CR(m_pHeaderContainer->SetVisible(true));
	//CR(m_pTitleIcon->SetVisible(true));
	//CR(m_pTitleText->SetVisible(true));
Error:
	return r;
}

RESULT UIKeyboard::ShowBrowserButtons() {
	RESULT r = R_PASS;

	CR(m_pKeyboardControls->SetVisible(true, false));

Error:
	return r;
}

RESULT UIKeyboard::HideBrowserButtons() {
	RESULT r = R_PASS;

	CR(m_pKeyboardControls->SetVisible(false, false));

Error:
	return r;
}

RESULT UIKeyboard::UpdateComposite(float depth) {
	RESULT r = R_PASS;

	point ptHeader = m_pHeaderContainer->GetPosition();
	m_pHeaderContainer->SetPosition(point(ptHeader.x(), ptHeader.y(), depth));

	float offset = m_surfaceHeight / 2.0f;
	float angle = m_surfaceAngle * (float)(M_PI) / 180.0f;

	m_pSurfaceContainer->SetPosition(point(0.0f, -(sin(angle) * offset + (2.0f * m_lineHeight * m_numLines)), depth + (cos(angle) * offset)));

	point ptkbOffset = point(0.0f, -0.07f, 0.0f);

	point ptOrigin;
	quaternion qOrigin;

	std::shared_ptr<DreamUserApp> pDreamUserApp = GetDOS()->GetUserApp();
	CN(pDreamUserApp);
	CR(pDreamUserApp->GetAppBasisPosition(ptOrigin));
	CR(pDreamUserApp->GetAppBasisOrientation(qOrigin));

	GetComposite()->SetPosition(ptOrigin + ptkbOffset);
	GetComposite()->SetOrientation(qOrigin);

	m_ptComposite = GetComposite()->GetPosition();

Error:
	return r;
}

RESULT UIKeyboard::SetPasswordFlag(bool fIsPassword) {
	RESULT r = R_PASS;

	if (fIsPassword) {
		m_pTextBoxText->AddFlags(text::flags::PASSWORD);
	}
	else {
		m_pTextBoxText->RemoveFlags(text::flags::PASSWORD);
	}

	return R_PASS;
}

std::shared_ptr<UIButton> UIKeyboard::GetCancelButton() {
	return m_pCancelButton;
}

RESULT UIKeyboard::UpdateComposite(float depth, point ptOrigin, quaternion qOrigin) {
	RESULT r = R_PASS;

	point ptHeader = m_pHeaderContainer->GetPosition();
	m_pHeaderContainer->SetPosition(point(ptHeader.x(), ptHeader.y(), depth));

	float offset = m_surfaceHeight / 2.0f;
	float angle = m_surfaceAngle * (float)(M_PI) / 180.0f;

	m_pSurfaceContainer->SetPosition(point(0.0f, -(sin(angle) * offset + (2.0f * m_lineHeight * m_numLines)), depth + (cos(angle) * offset)));

	//point ptkbOffset = point(0.0f, -0.07f, 0.0f);
	point ptkbOffset = point(0.0f, 0.0f, 0.0f);

	GetComposite()->SetPosition(ptOrigin + ptkbOffset);
	GetComposite()->SetOrientation(qOrigin);

	m_ptComposite = GetComposite()->GetPosition();

//Error:
	return r;
}

RESULT UIKeyboard::ClearActiveKeys() {
	m_activeKeys.clear();
	return R_PASS;
}

RESULT UIKeyboard::AddActiveKey(UIKey *pKey) {
	RESULT r = R_PASS;

	CBR(!IsActiveKey(pKey), R_PASS);
	m_activeKeys.push_back(pKey);

Error:
	return r;
}

RESULT UIKeyboard::RemoveActiveKey(UIKey *pKey) {
	RESULT r = R_PASS;

	CBR(IsActiveKey(pKey), R_OBJECT_NOT_FOUND);
	m_activeKeys.remove(pKey);

Error:
	return r;
}

bool UIKeyboard::IsActiveKey(UIKey *pKey) {
	
	for (auto it = m_activeKeys.begin(); it != m_activeKeys.end(); it++) {
		if ((*it) == pKey) {
			return true;
		}
	}
	return false;
}


RESULT UIKeyboard::UpdateViewQuad() {
	RESULT r = R_PASS;

	CR(m_pSurface->UpdateParams(GetWidth(), GetHeight(), m_pSurface->GetNormal()));
	
	// Flip UV vertically
	if (r != R_SKIPPED) {
		m_pSurface->TransformUV(
		{ { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f,
			0.0f, -1.0f } }
		);
	}

	CR(m_pSurface->SetDirty());

	CR(m_pSurface->InitializeBoundingQuad(point(0.0f, 0.0f, 0.0f), GetWidth(), GetHeight(), m_pSurface->GetNormal()));

Error:
	return r;
}

float UIKeyboard::GetWidth() {
	return m_surfaceWidth;
}

float UIKeyboard::GetHeight() {
	return m_surfaceHeight;
}

float UIKeyboard::GetAngle() {
	return m_surfaceAngle;
}

RESULT UIKeyboard::SetWidth(float width) {
	RESULT r = R_PASS;
	m_surfaceWidth = width;
	CR(UpdateViewQuad());
Error:
	return r;
}

RESULT UIKeyboard::SetHeight(float height) {
	RESULT r = R_PASS;
	m_surfaceHeight = height;
	CR(UpdateViewQuad());
Error:
	return r;
}

RESULT UIKeyboard::SetSurfaceAngle(float angle) {
	RESULT r = R_PASS;
	m_surfaceAngle = angle;
	quaternion qOrientation = quaternion::MakeQuaternionWithEuler(angle * (float)(M_PI) / 180.0f, 0.0f, 0.0f);
	m_pSurfaceContainer->SetOrientation(qOrientation);
	return r;
}

RESULT UIKeyboard::SetKeyTypeThreshold(float threshold) {
	m_keyTypeThreshold = threshold;
	return R_PASS;
}

RESULT UIKeyboard::SetKeyReleaseThreshold(float threshold) {
	m_keyReleaseThreshold = threshold;
	return R_PASS;
}

RESULT UIKeyboard::HandleDonePressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pKeyboardControls->IsVisible(), R_SKIPPED);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(UpdateKeyState((SenseVirtualKey)(SVK_CLOSE), 0));
	CR(UpdateKeyState((SenseVirtualKey)(SVK_CLOSE), 1));

Error:
	return r;
}

RESULT UIKeyboard::HandleCancelPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	std::string strKeyboardCancel = "UIKeyboard.FormCancel";
	CBR(m_pKeyboardControls->IsVisible(), R_SKIPPED);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(GetDOS()->SendDOSMessage(strKeyboardCancel));

Error:
	return r;
}

RESULT UIKeyboard::HandleTabPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pKeyboardControls->IsVisible(), R_SKIPPED);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(UpdateKeyState((SenseVirtualKey)(SVK_TAB), 0));
	CR(UpdateKeyState((SenseVirtualKey)(SVK_TAB), 1));

Error:
	return r;
}

RESULT UIKeyboard::HandleBackTabPressed(UIButton* pButtonContext, void* pContext) {
	RESULT r = R_PASS;

	CBR(m_pKeyboardControls->IsVisible(), R_SKIPPED);
	CBR(m_pParentApp->CanPressButton(pButtonContext), R_SKIPPED);
	CR(UpdateKeyState((SenseVirtualKey)(SVK_SHIFTTAB), 0));
	CR(UpdateKeyState((SenseVirtualKey)(SVK_SHIFTTAB), 1));

Error:
	return r;
}

RESULT UIKeyboard::UpdateTabNextTexture(bool fCanTabNext) {
	RESULT r = R_PASS;

	CN(m_pNextButton);
	CR(m_pNextButton->SetEnabledFlag(fCanTabNext));

Error:
	return r;
}
RESULT UIKeyboard::UpdateTabPreviousTexture(bool fCanTabPrevious) {
	RESULT r = R_PASS;

	CN(m_pPreviousButton);
	CR(m_pPreviousButton->SetEnabledFlag(fCanTabPrevious));

Error:
	return r;
}
