#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

#include "UIKey.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

RESULT UIKeyboardHandle::Show() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(ShowKeyboard());
Error:
	return r;
}

RESULT UIKeyboardHandle::Hide() {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(HideKeyboard());
Error:
	return r;
}

RESULT UIKeyboardHandle::UpdateComposite(float height, float depth) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(UpdateKeyboardComposite(height, depth));
Error:
	return r;
}

bool UIKeyboardHandle::IsVisible() {
	RESULT r = R_PASS;
	CB(GetAppState());
	return IsKeyboardVisible();
Error:
	//TODO: could be a problem because nullptr is equal to false
	return nullptr;
}

RESULT UIKeyboardHandle::UpdateTitleView(texture *pIconTexture, std::string strTitle) {
	RESULT r = R_PASS;
	CB(GetAppState());
	CR(UpdateKeyboardTitleView(pIconTexture,strTitle));
Error:
	return r;
}

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

	auto userUIDs = GetDOS()->GetAppUID("DreamUserApp");
	CB(userUIDs.size() == 1);
	m_userAppUID = userUIDs[0];

	auto pSenseKeyboardPublisher = dynamic_cast<Publisher<SenseVirtualKey, SenseKeyboardEvent>*>(this);
	CR(pSenseKeyboardPublisher->RegisterSubscriber(SVK_ALL, GetDOS()->GetInteractionEngineProxy()));

	//TODO this may become deprecated
	m_qSurfaceOrientation = quaternion::MakeQuaternionWithEuler(m_surfaceAngle * (float)(M_PI) / 180.0f, 0.0f, 0.0f);

	m_pSurfaceContainer = GetComposite()->AddComposite();
	m_pSurfaceContainer->SetOrientation(m_qSurfaceOrientation);

	m_pSurface = m_pSurfaceContainer->AddQuad(m_surfaceHeight, m_surfaceWidth);
	CN(m_pSurface);

	m_pSurface->SetVisible(false);
	CR(m_pSurface->InitializeOBB()); // TODO: using the default BoundingQuad could potentially be better

	m_pHeaderContainer = GetComposite()->AddComposite();

	m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
	m_pFont->SetLineHeight(m_lineHeight);

	// textures for keycaps and objects
	m_pKeyTexture = GetComposite()->MakeTexture(L"key-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pTextBoxTexture = GetComposite()->MakeTexture(L"text-input-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

	m_pDeleteTexture = GetComposite()->MakeTexture(L"Keycaps\\key-delete-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pLettersTexture = GetComposite()->MakeTexture(L"Keycaps\\key-abc-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pNumbersTexture = GetComposite()->MakeTexture(L"Keycaps\\key-123-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pReturnTexture = GetComposite()->MakeTexture(L"Keycaps\\key-return-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pShiftTexture = GetComposite()->MakeTexture(L"Keycaps\\key-shift-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pSpaceTexture = GetComposite()->MakeTexture(L"Keycaps\\key-space-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pSymbolsTexture = GetComposite()->MakeTexture(L"Keycaps\\key-symbol-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pUnshiftTexture = GetComposite()->MakeTexture(L"Keycaps\\key-unshift-background.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

	{
		//Setup textbox
		float offset = m_surfaceHeight / 2.0f;
		float angle = m_surfaceAngle * (float)(M_PI) / 180.0f;

		m_pHeaderContainer->RotateXByDeg(90.0f);

		m_pTextBoxBackground = m_pHeaderContainer->AddQuad(m_surfaceWidth, m_lineHeight * m_numLines * 1.5f, point(0.0f, -0.001f, 0.0f));
		m_pTextBoxBackground->SetDiffuseTexture(m_pTextBoxTexture.get());

		m_pTextBoxText = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			"",
			m_surfaceWidth - 0.02f,
			m_lineHeight * m_numLines, 
			text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));

		m_pHeaderContainer->AddObject(m_pTextBoxText);

		//Setup title / icon
		m_pTitleIcon = m_pHeaderContainer->AddQuad(0.068, 0.068 * (3.0f / 4.0f));
		m_pTitleIcon->SetPosition(point(-m_surfaceWidth / 2.0f + 0.034f, 0.0f, -2.5f * m_lineHeight * m_numLines));

		m_pFont->SetLineHeight(0.050f);
		m_pTitleText = std::shared_ptr<text>(GetDOS()->MakeText(
			m_pFont,
			"",
			m_surfaceWidth - 0.02f,
			0.050,
			text::flags::TRAIL_ELLIPSIS | text::flags::WRAP | text::flags::RENDER_QUAD));
		m_pTitleText->SetPosition(point(m_surfaceWidth / 6.0f, 0.0f, -2.5f * m_lineHeight * m_numLines));
		m_pHeaderContainer->AddObject(m_pTitleText);

	}

	m_keyObjects[0] = nullptr;
	m_keyObjects[1] = nullptr;

	// pre-load possible keyboard layers 
	InitializeLayoutTexture(LayoutType::QWERTY);
	InitializeLayoutTexture(LayoutType::QWERTY_UPPER);
	InitializeLayoutTexture(LayoutType::QWERTY_NUM);
	InitializeLayoutTexture(LayoutType::QWERTY_SYMBOL);

	auto pLayout = new UIKeyboardLayout();
	pLayout->CreateQWERTYLayout(); // should be in constructor probably
	InitializeQuadsWithLayout(pLayout);
	m_pLayout = pLayout;

	m_currentLayout = LayoutType::QWERTY;

	GetComposite()->SetVisible(false);

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
	pLayout->SetRowHeight((1.0f / (float)pLayout->GetKeys()[0].size()) / m_keyScale);

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

			float uvTop = (rowIndex * m_surfaceHeight);
			float uvBottom = (((rowIndex + 1) * m_surfaceHeight));

			float rowCount = (float)pLayoutKeys.size();
			float zPos = (m_surfaceHeight / rowCount) * (rowIndex - (rowCount / 2.0f) + 0.5f);

			float xPos = m_surfaceWidth * (pKey->m_left + pKey->m_width / 2.0f) - (m_surfaceWidth / 2.0f);

			point ptOrigin = point(xPos, 0.0f, zPos) + m_pSurface->GetPosition();

			std::shared_ptr<quad> pQuad = m_pSurfaceContainer->AddQuad(
				keyDimension,
				keyDimension,
				ptOrigin,
				uvcoord(uvLeft, uvTop),
				uvcoord(uvRight, uvBottom));

			pQuad->SetDiffuseTexture(m_layoutAtlas[pLayout->GetLayoutType()]->GetFramebuffer()->GetColorTexture());

			// Set up key quad positioning
			pQuad->ScaleX((m_keyScale * 0.5f*pKey->m_width) / keyDimension);
			pQuad->ScaleY(m_keyScale);

			pQuad->SetMaterialAmbient(m_ambientIntensity);

			pKey->m_pQuad = pQuad;
			pKey->m_pQuad->SetVisible(false);
			colIndex++;
		}
		colIndex = 0;
		rowIndex++;
	}
	pLayout->SetVisible(false);

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
	DreamOS *pDOS = GetDOS();

	UIKey *keyCollisions[2];
	keyCollisions[0] = nullptr;
	keyCollisions[1] = nullptr;
	point ptCollisions[2];
	point ptCollision;

	std::vector<UIKey*> activeKeysToRemove;

	// skip keyboard interaction if not visible
	CBR((IsVisible()), R_SKIPPED);
	CBR(m_pUserHandle != nullptr, R_SKIPPED);
	//CBR(m_pUserHandle != nullptr && m_pUserHandle->GetAppState(), R_SKIPPED);

	CN(pDOS);
	pProxy = pDOS->GetInteractionEngineProxy();
	CN(pProxy);

	// Update Keys if the app is active
	int i = 0;
	UIMallet* pLMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT);
	CNR(pLMallet, R_SKIPPED);
	UIMallet* pRMallet = m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT);
	CNR(pRMallet, R_SKIPPED);

	for (auto &mallet : { pLMallet, pRMallet })
	{
		point ptBoxOrigin = m_pSurface->GetOrigin(true);
		point ptSphereOrigin = mallet->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(m_pSurface->GetOrientation(true))) * (ptSphereOrigin - m_pSurface->GetOrigin(true)));
		ptCollisions[i] = ptSphereOrigin;

		if (ptSphereOrigin.y() >= mallet->GetRadius()) mallet->CheckAndCleanDirty();
		//else if (ptSphereOrigin.y() < m_keyReleaseThreshold) mallet->SetDirty();
		
		// if the sphere is lower than its own radius, there must be an interaction
		if (ptSphereOrigin.y() < mallet->GetRadius() && !mallet->IsDirty()) {

			//TODO: CollisionPointToKey returns one key based on the center of the sphere
			// if it accounted for the radius, it would be able to return multiple keys
			auto key = CollisionPointToKey(ptSphereOrigin);
			if (!key) continue;
			CR(AddActiveKey(key));
			keyCollisions[i] = key;

			point ptPosition = key->m_pQuad->GetPosition();

			// TODO: edge case where there is a new interaction during the key release animation
			key->m_pQuad->SetPosition(point(ptPosition.x(), ptSphereOrigin.y() - mallet->GetRadius(), ptPosition.z()));
		}

		i++;
	}

	// covers edge case where both mallets are hitting the same key
	if (keyCollisions[0] != nullptr && keyCollisions[1] != nullptr && keyCollisions[0] == keyCollisions[1]) {

		auto key = keyCollisions[0];
		point ptPosition = key->m_pQuad->GetPosition();
		float y = std::min(ptCollisions[0].y(), ptCollisions[1].y());
		key->m_pQuad->SetPosition(point(ptPosition.x(), y - pLMallet->GetRadius(), ptPosition.z()));
	}


	for (auto key : m_activeKeys) {

		// get collision point and check that key is active
		bool fActive = false;
		ControllerType controllerType;
		UIMallet *pMallet = nullptr;
		for (int j = 0; j < 2; j++) {
			auto k = keyCollisions[j];
			if (key == k) {
				ptCollision = ptCollisions[j];
				fActive = true;
				controllerType = (ControllerType)(j);
				pMallet = (j == 0) ? pLMallet : pRMallet;
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
				if (pMallet != nullptr)
					pMallet->SetDirty();
			}
			//else key->m_state = KeyState::KEY_DOWN;
		} break;

		case KeyState::KEY_MAYBE_DOWN: {
			if (ptCollision.y() < m_keyTypeThreshold) {
				CR(UpdateKeyState((SenseVirtualKey)key->m_letter, 1));
				CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(controllerType, SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
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

DreamAppHandle* UIKeyboard::GetAppHandle() {
	return (UIKeyboardHandle*)(this);
}

UIKeyboard* UIKeyboard::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	UIKeyboard *pUIKeyboard = new UIKeyboard(pDreamOS, pContext);
	return pUIKeyboard;
}

RESULT UIKeyboard::ShowKeyboard() {

	//Capture user app
	m_pUserHandle = dynamic_cast<DreamUserHandle*>(GetDOS()->CaptureApp(m_userAppUID, this));

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		GetComposite()->SetPosition(m_ptComposite - point(0.0f, m_animationOffsetHeight, 0.0f));
		pKeyboard->GetComposite()->SetVisible(true);
		pKeyboard->HideSurface();

	Error:
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		CN(m_pUserHandle);
//		CB(m_pUserHandle->GetAppState());
		//CR(m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT)->Show());
		//CR(m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT)->Show());
	Error:
		return r;
	};

	DimObj *pObj = GetComposite();
	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		m_ptComposite,
		pObj->GetOrientation(),// * m_qSurfaceOrientation,
		pObj->GetScale(),
		m_animationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	);
	return R_PASS;
}

RESULT UIKeyboard::HideKeyboard() {

	RESULT r = R_PASS;

	auto fnCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		pKeyboard->GetComposite()->SetVisible(false);
		// full press of key that clears whole string
		CR(UpdateKeyState((SenseVirtualKey)(0x01), 0));
		CR(UpdateKeyState((SenseVirtualKey)(0x01), 1));

		CR(UpdateKeyboardLayout(LayoutType::QWERTY));


	Error:
		return r;
	};

	DimObj *pObj = GetComposite();

	//CN(m_pUserHandle);
	//CR(m_pUserHandle->RequestMallet(HAND_TYPE::HAND_LEFT)->Hide());
	//CR(m_pUserHandle->RequestMallet(HAND_TYPE::HAND_RIGHT)->Hide());

	CR(GetDOS()->ReleaseApp(m_pUserHandle, m_userAppUID, this));

	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition() - point(0.0f, m_animationOffsetHeight, 0.0f),
		pObj->GetOrientation(),
		pObj->GetScale(),
		m_animationDuration,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		nullptr,
		fnCallback,
		this
	);

Error:
	return r;
}

RESULT UIKeyboard::HideSurface() {
	return m_pSurface->SetVisible(false);
}

bool UIKeyboard::IsVisible() {
	return GetComposite()->IsVisible();
}

bool UIKeyboard::IsKeyboardVisible() {
	return IsVisible();
}

RESULT UIKeyboard::UpdateKeyboardComposite(float height, float depth) {
	return UpdateComposite(height, depth);
}

RESULT UIKeyboard::SetVisible(bool fVisible) {
	return GetComposite()->SetVisible(fVisible);
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

	int rowIndex = (ptCollision.z() + (m_surfaceHeight / 2.0f)) / m_surfaceHeight * keyboardLayout.size();
	CBR(rowIndex >= 0 && rowIndex < keyboardLayout.size(), R_OBJECT_NOT_FOUND);

	float xPos = (ptCollision.x() + (m_surfaceWidth / 2.0f)) / m_surfaceWidth;
	auto& row = keyboardLayout[rowIndex];

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
		m_pTextBoxText->SetText("");
		//TODO: it is possible that when the menu button is pressed again, 
		// the user is at the root menu by coincidence.  may need to notify
		// DreamUIBar in some way in the future

		//HideKeyboard();
	}

	else if (chkey == 0x01) {
		m_pTextBoxText->SetText("");
	}

	else if (chkey == SVK_BACK) {
		auto strTextbox = m_pTextBoxText->GetText();
		if (strTextbox.size() > 0)
			strTextbox.pop_back();
			m_pTextBoxText->SetText(strTextbox);
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
			CR(UpdateKeyboardLayout(LayoutType::QWERTY));
		}
	}


Error:
	return r;
}

RESULT UIKeyboard::UpdateKeyboardTitleView(texture *pIconTexture, std::string strTitle) {
	RESULT r = R_PASS;

	if (pIconTexture != nullptr) {
		CR(m_pTitleIcon->SetDiffuseTexture(pIconTexture));
	}
	m_pTitleText->SetText(strTitle);

Error:
	return r;
}

RESULT UIKeyboard::UpdateComposite(float height, float depth) {
	RESULT r = R_PASS;

	point ptHeader = m_pHeaderContainer->GetPosition();
	m_pHeaderContainer->SetPosition(point(ptHeader.x(), ptHeader.y(), depth));
	float offset = m_surfaceHeight / 2.0f;
	float angle = m_surfaceAngle * (float)(M_PI) / 180.0f;
	//float angle = SURFACE_ANGLE * (float)(M_PI) / 180.0f;
	m_pSurfaceContainer->SetPosition(point(0.0f, -(sin(angle) * offset + (2.0f * m_lineHeight * m_numLines)), depth + (cos(angle) * offset)));

	CR(UpdateCompositeWithHands(height));
	m_ptComposite = GetComposite()->GetPosition();

Error:
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