#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/BoundingQuad.h"
#include "Primitives/BoundingSphere.h"

UIKeyboard::UIKeyboard(DreamOS *pDreamOS, void *pContext) :
	DreamApp<UIKeyboard>(pDreamOS, pContext)
{
}

RESULT UIKeyboard::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

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
	m_pSurface->InitializeOBB();

	m_pTextBox = GetComposite()->AddQuad(m_surfaceHeight / 4.0f, m_surfaceWidth);
	CN(m_pTextBox);
	m_pTextBox->SetVisible(false);
	m_pTextBox->SetPosition(point(0.0f, 0.00f, -m_surfaceHeight * 0.75f ));
	m_pTextBoxTexture = GetComposite()->MakeTexture(L"Textbox-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_pTextBox->SetColorTexture(m_pTextBoxTexture.get());

	m_pTextBoxContainer = GetComposite()->AddComposite();

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);

	m_keyTypeThreshold = -0.015f;

	m_pFont = std::make_shared<Font>(L"Basis_Grotesque_Pro.fnt", true);
	m_pKeyTexture = GetComposite()->MakeTexture(L"Key-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	m_keyStates[0] = ActiveObject::state::NOT_INTERSECTED;
	m_keyStates[1] = ActiveObject::state::NOT_INTERSECTED;

	m_keyObjects[0] = nullptr;
	m_keyObjects[1] = nullptr;

	m_pLayout = new UIKeyboardLayout();
	m_pLayout->CreateQWERTYLayout();
	InitializeQuadsWithLayout();

	GetComposite()->SetVisible(false);

Error:
	return r;
}

RESULT UIKeyboard::InitializeQuadsWithLayout() {
	RESULT r = R_PASS;
	
	m_pQuadTextures = GetDOS()->AddFlatContext();

	auto& layout = m_pLayout->GetKeys();

	//not flexible, TODO: take max of rows?
	float keyDimension = m_surfaceWidth / (float)layout[0].size();

	int rowIndex = 0;
	int colIndex = 0;
	for (auto& row : layout) {
		for (auto& key : row) {

			// Set up text box key texture
			if (m_pQuadTextures->HasChildren()) m_pQuadTextures->ClearChildren();
			std::string ch = "";
			if (key->m_letter > 0x20) ch = key->m_letter;

			m_pQuadTextures->AddText(m_pFont, ch, 0.2f, true);
			GetDOS()->RenderToTexture(m_pQuadTextures);

			m_keyTextureLookup[ch] = m_pQuadTextures->GetFramebuffer()->GetTexture();

			// Set up key quad texture
			if (m_pQuadTextures->HasChildren()) m_pQuadTextures->ClearChildren();

			auto keyBack = m_pQuadTextures->AddQuad(2.0f, 2.0f, point(0.0f, 0.0f, 0.0f));
			keyBack->SetColorTexture(m_pKeyTexture.get());

			m_pQuadTextures->AddText(m_pFont, ch, 0.2f, true);
			GetDOS()->RenderToTexture(m_pQuadTextures);

			std::shared_ptr<quad> pQuad = GetComposite()->AddQuad(keyDimension, keyDimension);
			pQuad->SetColorTexture(m_pQuadTextures->GetFramebuffer()->GetTexture());

			// Set up key quad positioning
			pQuad->ScaleX(1.0f / (keyDimension / (0.5f*key->m_width)));

			float rowCount = (float)layout.size();
			float zPos = (m_surfaceHeight / rowCount) * (rowIndex - (rowCount / 2.0f) + 0.5f);

			float xPos = m_surfaceWidth * key->m_left - (m_surfaceWidth / 2.0f) +(key->m_width / 4.0f);

			pQuad->SetPosition(point(xPos, 0.0f, zPos) + m_pSurface->GetPosition());
			pQuad->SetMaterialAmbient(0.75f);

			key->m_pQuad = pQuad;
			colIndex++;
		}
		colIndex = 0;
		rowIndex++;
	}


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

	// update quads if layout has changed
	if (m_pLayout->CheckAndCleanDirty()) {
		InitializeQuadsWithLayout();
	}

	CN(pDOS);
	pProxy = pDOS->GetInteractionEngineProxy();
	CN(pProxy);

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

	int i = 0;
	for (auto &mallet : { m_pLeftMallet, m_pRightMallet })
	{
		auto head = mallet->GetMalletHead();
		point ptBoxOrigin = m_pSurface->GetOrigin(true);
		point ptSphereOrigin = head->GetOrigin(true);
		ptSphereOrigin = (point)(inverse(RotationMatrix(m_pSurface->GetOrientation(true))) * (ptSphereOrigin - m_pSurface->GetOrigin(true)));
		OVERLAY_DEBUG_SET("pt", ptSphereOrigin);

		if (ptSphereOrigin.y() >= 0.02f) mallet->CheckAndCleanDirty();

		if (ptSphereOrigin.y() < 0.02f && !mallet->IsDirty()) {

			auto key = CollisionPointToKey(ptSphereOrigin);
			if (!key) {
				if (m_keyObjects[i]) ReleaseKey(i);
				continue;
			}

			point ptPosition = key->m_pQuad->GetPosition();
			key->m_pQuad->SetPosition(point(ptPosition.x(), ptSphereOrigin.y() - 0.02f, ptPosition.z()));

			//stops typing the same character a bunch of times
			if (m_keyStates[i] == ActiveObject::state::NOT_INTERSECTED) {
				m_keyObjects[i] = key;
				m_keyStates[i] = ActiveObject::state::INTERSECTED;
			}
			// allows for rolling over keys without typing them
			else if (m_keyObjects[i] && key != m_keyObjects[i]) {
				CR(ReleaseKey(i));
				m_keyObjects[i] = key;
			}
			if (ptSphereOrigin.y() < m_keyTypeThreshold && !key->m_fTyped) {
				CR(UpdateKeyState((SenseVirtualKey)key->m_letter, 1));
				key->m_fTyped = true;
			}
	
			//TODO: may want this to be a different distance than the 'typed' threshold
			// this comes with some edge cases however
			if (ptSphereOrigin.y() < m_keyTypeThreshold) {
				CR(ReleaseKey(i));
				mallet->SetDirty();
			}
		}
		else {
			if (m_keyObjects[i]) ReleaseKey(i);
		}

		i++;
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

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		GetComposite()->SetPosition(point(0.0f, -1.0f, 5.0f));
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

	UpdateAppComposite();
	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		m_ptSurfaceOffset,
		GetComposite()->GetOrientation() * m_qSurfaceOrientation,
		GetComposite()->GetScale(),
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

	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		point(0.0f, 0.0f, 5.0f),
		GetComposite()->GetOrientation(),
		GetComposite()->GetScale(),
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

RESULT UIKeyboard::ReleaseKey(int index) {	
	RESULT r = R_PASS;
	
	auto pKey = m_keyObjects[index];
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

	pKey->m_fTyped = false;

	m_keyObjects[index] = nullptr;
	m_keyStates[index] = ActiveObject::state::NOT_INTERSECTED;

	CR(UpdateKeyState((SenseVirtualKey)(pKey->m_letter), 0));

Error:
	return r;
}
// this function assumes the key height is constant
UIKey* UIKeyboard::CollisionPointToKey(point ptCollision) {
	RESULT r = R_PASS;
/*
	point pt = manifold.GetContactPoint(0).GetPoint();
	point ptCenter = GetComposite()->GetPosition();

	auto mat = inverse(RotationMatrix(GetComposite()->GetOrientation()));
	pt = mat*pt;
	ptCenter = mat*ptCenter;
//*/
	//OVERLAY_DEBUG_SET("max", (float)manifold.MaxPenetrationDepth());
	//OVERLAY_DEBUG_SET("min", (float)manifold.MinPenetrationDepth());

	auto& keyboardLayout = m_pLayout->GetKeys();
//	int rowIndex = (pt.z() - ptCenter.z() + (m_surfaceHeight / 2.0f)) / m_surfaceHeight * keyboardLayout.size();
	int rowIndex = (ptCollision.z() + (m_surfaceHeight / 2.0f)) / m_surfaceHeight * keyboardLayout.size();
	CBR(rowIndex >= 0 && rowIndex < keyboardLayout.size(), R_OBJECT_NOT_FOUND);

//	float xPos = (pt.x() - ptCenter.x() + (m_surfaceWidth / 2.0f)) / m_surfaceWidth;
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

//TODO: This isn't ideal, especially because this code is similar to code in 
// UIModule and UIBar.  This code should be refactored upon creation of 
// a 'user' object that manages the positions of applications appropriately.
//
RESULT UIKeyboard::UpdateAppComposite() {
	RESULT r = R_PASS;

	auto pComposite = GetComposite();
	quaternion q = pComposite->GetCamera()->GetOrientation();
	float headRotationYDeg = q.ProjectedYRotationDeg();
	float radY = headRotationYDeg * M_PI / 180.0f;
	quaternion q2 = quaternion::MakeQuaternionWithEuler(0.0f, radY, 0.0f);

	m_ptSurfaceOffset = pComposite->GetCamera()->GetPosition();
	point cam = m_surfaceDistance * point(-sin(radY), -0.5f, -cos(radY));
	OVERLAY_DEBUG_SET("cam", cam);
	m_ptSurfaceOffset = m_ptSurfaceOffset + cam;
	pComposite->SetPosition(m_ptSurfaceOffset);
	pComposite->SetOrientation(q2);

//Error:
	return r;
}

RESULT UIKeyboard::UpdateTextBox(int chkey) {
	RESULT r = R_PASS;
		
	if (chkey == SVK_RETURN) {
		CR(m_pTextBoxContainer->ClearChildren());
	}

	else if (chkey == SVK_BACK) {
		CR(m_pTextBoxContainer->RemoveLastChild());
	}

	else {
		float keyDimension = m_surfaceWidth / (float)m_pLayout->GetKeys()[0].size();
		std::string ch = "";
		ch = chkey;
		if (m_keyTextureLookup.count(ch) > 0) {
			auto pQuad = m_pTextBoxContainer->AddQuad(keyDimension, keyDimension);
			CN(pQuad);
			CR(pQuad->SetColorTexture(m_keyTextureLookup[ch]));
			// TODO: Hardcoded positioning code here should be temporary
			pQuad->SetPosition(point((keyDimension / 3.0f) * m_pTextBoxContainer->GetChildren().size() - (m_surfaceWidth / 2.0f), 0.001f, -m_surfaceHeight * 0.75f));
		}
	}


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
