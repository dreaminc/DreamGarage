#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

#include "UIKey.h"

#include "Primitives/font.h"
#include "Primitives/text.h"
#include "Primitives/framebuffer.h"

UIKeyboard::UIKeyboard(DreamOS *pDreamOS, void *pContext) :
	DreamApp<UIKeyboard>(pDreamOS, pContext)
{
	// empty
}

RESULT UIKeyboard::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	std::shared_ptr<font> pFont;
	std::wstring wstrFont;

	// Register keyboard events
	auto pSenseKeyboardPublisher = dynamic_cast<Publisher<SenseVirtualKey, SenseKeyboardEvent>*>(this);
	CR(pSenseKeyboardPublisher->RegisterSubscriber(SVK_ALL, GetDOS()->GetInteractionEngineProxy()));

	//TODO this may become deprecated
	m_surfaceDistance = 0.5f;
	m_ptSurfaceOffset = point(0.0f, -0.25f, -m_surfaceDistance);
	m_qSurfaceOrientation = quaternion::MakeQuaternionWithEuler(30.0f * (float)(M_PI) / 180.0f, 0.0f, 0.0f);

	m_surfaceHeight = 0.25f;
	m_surfaceWidth = 0.5f;
	m_pSurface = GetComposite()->AddQuad(m_surfaceHeight, m_surfaceWidth);
	CN(m_pSurface);

	m_pSurface->SetVisible(false);
	CR(m_pSurface->InitializeOBB()); // TODO: using the default BoundingQuad could potentially be better

	m_pTextBox = GetComposite()->AddQuad(m_surfaceHeight / 4.0f, m_surfaceWidth);
	CN(m_pTextBox);
	m_pTextBox->SetVisible(false);
	m_pTextBox->SetPosition(point(0.0f, 0.0f, -m_surfaceHeight * 0.75f ));
	m_pTextBoxTexture = GetComposite()->MakeTexture(L"Textbox-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	CR(m_pTextBox->SetColorTexture(m_pTextBoxTexture.get()));

	m_pTextBoxContainer = GetComposite()->AddComposite();

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);

	m_keyTypeThreshold = 0.0f; // triggered once the center of the mallet hits the keyboard surface
	m_keyReleaseThreshold = -0.025f;

	m_pFont = std::make_shared<font>(L"Basis_Grotesque_Pro.fnt", GetComposite(), true);
	m_pKeyTexture = GetComposite()->MakeTexture(L"Key-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	m_keyObjects[0] = nullptr;
	m_keyObjects[1] = nullptr;

	// pre-load possible keyboard layers 
	InitializeTexturesWithLayout(LayoutType::QWERTY);
	InitializeTexturesWithLayout(LayoutType::QWERTY_UPPER);
	InitializeTexturesWithLayout(LayoutType::QWERTY_NUM);
	InitializeTexturesWithLayout(LayoutType::QWERTY_SYMBOL);

	auto pLayout = new UIKeyboardLayout();
	pLayout->CreateQWERTYLayout(); // should be in constructor probably
	InitializeQuadsWithLayout(pLayout);
	m_pLayout = pLayout;

	m_currentLayout = LayoutType::QWERTY;

	GetComposite()->SetVisible(false);

Error:
	return r;
}

RESULT UIKeyboard::InitializeTexturesWithLayout(LayoutType type) {
	RESULT r = R_PASS;

	m_pQuadTextures = GetDOS()->AddFlatContext();

	std::shared_ptr<UIKeyboardLayout> pLayout = std::make_shared<UIKeyboardLayout>(type);
	bool fUpper = (type == LayoutType::QWERTY_UPPER || type == LayoutType::QWERTY_SYMBOL);
	bool fNum = (type == LayoutType::QWERTY_NUM || type == LayoutType::QWERTY_SYMBOL);

	pLayout->CreateQWERTYLayout(fUpper, fNum);

	for (auto& layoutRow : pLayout->GetKeys()) {
		for (auto& pKey : layoutRow) {

			// Set up text box key texture
			if (m_pQuadTextures->HasChildren()) 
				m_pQuadTextures->ClearChildren();

			std::string ch = "";
			if (pKey->m_letter >= 0x20) 
				ch = pKey->m_letter;

			std::shared_ptr<text> pText = m_pQuadTextures->AddText(m_pFont, m_pFont->GetTexture().get(), ch, 0.2f, true);
			GetDOS()->RenderToTexture(m_pQuadTextures);

			m_keyCharAtlas[pKey->m_letter] = GetDOS()->MakeTexture(*(m_pQuadTextures->GetFramebuffer()->GetColorTexture()));

			// Set up key quad texture
			if (m_pQuadTextures->HasChildren()) 
				m_pQuadTextures->ClearChildren();

			auto keyBack = m_pQuadTextures->AddQuad(2.0f, 2.0f, point(0.0f, 0.0f, 0.0f));
			keyBack->SetColorTexture(m_pKeyTexture.get());

			pText = m_pQuadTextures->AddText(m_pFont, m_pFont->GetTexture().get(), ch, 0.2f, true);
			GetDOS()->RenderToTexture(m_pQuadTextures);

			auto pTexture = GetDOS()->MakeTexture(*(m_pQuadTextures->GetFramebuffer()->GetColorTexture()));
			m_keyTextureAtlas[pKey->m_letter] = pTexture;
		}
	}

//Error:
	return r;
}
RESULT UIKeyboard::InitializeQuadsWithLayout(UIKeyboardLayout* pLayout) {
	RESULT r = R_PASS;
	
	m_pQuadTextures = GetDOS()->AddFlatContext();

	auto& pLayoutKeys = pLayout->GetKeys();

	//not flexible, TODO: take max of rows?
	float keyDimension = m_surfaceWidth / (float)pLayoutKeys[0].size();

	int rowIndex = 0;
	int colIndex = 0;
	for (auto& layoutRow : pLayoutKeys) {
		for (auto& pKey : layoutRow) {

			std::shared_ptr<quad> pQuad = GetComposite()->AddQuad(keyDimension, keyDimension);

			if (m_keyTextureAtlas[pKey->m_letter])
				pQuad->UpdateColorTexture(m_keyTextureAtlas[pKey->m_letter]);

			// Set up key quad positioning
			pQuad->ScaleX(1.0f / (keyDimension / (0.5f*pKey->m_width)));

			float rowCount = (float)pLayoutKeys.size();
			float zPos = (m_surfaceHeight / rowCount) * (rowIndex - (rowCount / 2.0f) + 0.5f);

			float xPos = m_surfaceWidth * pKey->m_left - (m_surfaceWidth / 2.0f) +(pKey->m_width / 4.0f);

			pQuad->SetPosition(point(xPos, 0.0f, zPos) + m_pSurface->GetPosition());
			pQuad->SetMaterialAmbient(0.75f);

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

	CN(pDOS);
	pProxy = pDOS->GetInteractionEngineProxy();
	CN(pProxy);

	// Update Mallet Positions
	hand *pHand = pDOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
	CN(pHand);
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	pHand = pDOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);
	CN(pHand);

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

	// Update Keys
	int i = 0;
	for (auto &mallet : { m_pLeftMallet, m_pRightMallet })
	{
		point ptBoxOrigin = m_pSurface->GetOrigin(true);
		point ptSphereOrigin = mallet->GetMalletHead()->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(m_pSurface->GetOrientation(true))) * (ptSphereOrigin - m_pSurface->GetOrigin(true)));
		ptCollisions[i] = ptSphereOrigin;

		if (ptSphereOrigin.y() >= mallet->GetRadius()) mallet->CheckAndCleanDirty();
		else if (ptSphereOrigin.y() < m_keyReleaseThreshold) mallet->SetDirty();
		
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
		key->m_pQuad->SetPosition(point(ptPosition.x(), y - m_pLeftMallet->GetRadius(), ptPosition.z()));
	}


	for (auto key : m_activeKeys) {

		// get collision point and check that key is active
		bool fActive = false;
		for (int j = 0; j < 2; j++) {
			auto k = keyCollisions[j];
			if (key == k) {
				ptCollision = ptCollisions[j];
				fActive = true;
			}
		}

		if (!fActive) {
			key->m_state = KeyState::KEY_NOT_INTERSECTED;
			continue;
		}
		
		switch (key->m_state) {

		case KeyState::KEY_DOWN: {
			if (ptCollision.y() > m_keyTypeThreshold) key->m_state = KeyState::KEY_MAYBE_UP;
			else if (ptCollision.y() < m_keyReleaseThreshold) ReleaseKey(key);
			//else key->m_state = KeyState::KEY_DOWN;
		} break;

		case KeyState::KEY_MAYBE_DOWN: {
			if (ptCollision.y() < m_keyTypeThreshold) {
				CR(UpdateKeyState((SenseVirtualKey)key->m_letter, 1));
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

	for (auto key : m_activeKeys) {
		if (key->m_state == KeyState::KEY_NOT_INTERSECTED) {
			ReleaseKey(key);
			RemoveActiveKey(key);
			key->m_state = KeyState::KEY_UP;
		}
	}

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

RESULT UIKeyboard::ShowKeyboard() {

	UpdateCompositeWithCameraLook(m_surfaceDistance, -0.25f);
	SetSurfaceOffset(GetComposite()->GetPosition());

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		GetComposite()->SetPosition(m_ptSurfaceOffset - point(0.0f, 1.0f, 0.0f));
		pKeyboard->GetComposite()->SetVisible(true);
		pKeyboard->HideSurface();
	Error:
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		m_pLeftMallet->Show();
		m_pRightMallet->Show();
	Error:
		return r;
	};

	DimObj *pObj = GetComposite();
	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		m_ptSurfaceOffset,
		pObj->GetOrientation() * m_qSurfaceOrientation,
		pObj->GetScale(),
		0.2f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	);
	return R_PASS;
}

RESULT UIKeyboard::HideKeyboard() {

	auto fnCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		pKeyboard->GetComposite()->SetVisible(false);
		m_pLeftMallet->Hide();
		m_pRightMallet->Hide();
	Error:
		return r;
	};

	DimObj *pObj = GetComposite();
	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		pObj,
		pObj->GetPosition() - point(0.0f, 1.0f, 0.0f),
		pObj->GetOrientation(),
		pObj->GetScale(),
		0.2f,
		AnimationCurveType::EASE_OUT_QUAD,
		AnimationFlags(),
		nullptr,
		fnCallback,
		this
	);
	return R_PASS;
}

RESULT UIKeyboard::HideSurface() {
	return m_pSurface->SetVisible(false);
}

bool UIKeyboard::IsVisible() {
	return GetComposite()->IsVisible();
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
		0.1f,
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

//	auto& keyboardLayout = m_kbLayers[m_currentLayout]->GetKeys();
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
			if (m_keyTextureAtlas.count(pKey->m_letter) > 0 && m_keyTextureAtlas[pKey->m_letter]) {
				CR(pKey->m_pQuad->UpdateColorTexture(m_keyTextureAtlas[pKey->m_letter]));
			}
		}
	}
	m_currentLayout = kbType;
	
Error:
	return r;
}

RESULT UIKeyboard::SetMallets(UIMallet *leftMallet, UIMallet *rightMallet) {
	RESULT r = R_PASS;

	CN(leftMallet);
	m_pLeftMallet = leftMallet;

	CN(rightMallet);
	m_pRightMallet = rightMallet;

Error:
	return R_PASS;
}

RESULT UIKeyboard::UpdateTextBox(int chkey) {
	RESULT r = R_PASS;
		
	if (chkey == SVK_RETURN) {
		CR(m_pTextBoxContainer->ClearChildren());
	}

	else if (chkey == SVK_BACK) {
		CR(m_pTextBoxContainer->RemoveLastChild());
	}

	//TODO: this logic should probably be in UIKeyboardLayout
	else if (chkey == SVK_SHIFT) {
		LayoutType newType;
		switch (m_currentLayout) {
		case LayoutType::QWERTY: newType = LayoutType::QWERTY_UPPER; break;
		case LayoutType::QWERTY_UPPER: newType = LayoutType::QWERTY; break;
		case LayoutType::QWERTY_NUM: newType = LayoutType::QWERTY_SYMBOL; break;
		case LayoutType::QWERTY_SYMBOL: newType = LayoutType::QWERTY_NUM; break;
		}
		CR(UpdateKeyboardLayout(newType));
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
		float keyDimension = m_surfaceWidth / (float)m_pLayout->GetKeys()[0].size();
		if (m_keyCharAtlas.count(chkey) > 0) {
			auto pQuad = m_pTextBoxContainer->AddQuad(keyDimension, keyDimension);
			CN(pQuad);
			CR(pQuad->UpdateColorTexture(m_keyCharAtlas[chkey]));
			// TODO: Hard-coded positioning code here should be temporary
			pQuad->SetPosition(point((keyDimension / 3.0f) * m_pTextBoxContainer->GetChildren().size() - (m_surfaceWidth / 2.0f), 0.001f, -m_surfaceHeight * 0.75f));
		}
	}


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

RESULT UIKeyboard::SetKeyTypeThreshold(float threshold) {
	m_keyTypeThreshold = threshold;
	return R_PASS;
}

RESULT UIKeyboard::SetKeyReleaseThreshold(float threshold) {
	m_keyReleaseThreshold = threshold;
	return R_PASS;
}

RESULT UIKeyboard::SetSurfaceOffset(point ptOffset) {
	m_ptSurfaceOffset = ptOffset;
	return R_PASS;
}
