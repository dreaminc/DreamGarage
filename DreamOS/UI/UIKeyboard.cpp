#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

UIKeyboard::UIKeyboard(DreamOS *pDreamOS, void *pContext) :
	DreamApp<UIKeyboard>(pDreamOS, pContext)
{
}

RESULT UIKeyboard::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	//TODO this may become deprecated
	m_ptSurface = point(-0.0f, 0.9f, 4.65f);
	GetComposite()->SetOrientation(quaternion::MakeQuaternionWithEuler(30.0f * (float)(M_PI) / 180.0f, 0.0f, 0.0f));

	m_surfaceHeight = 0.25f;
	m_surfaceWidth = 0.5f;
	m_pSurface = GetComposite()->AddQuad(m_surfaceHeight, m_surfaceWidth);
	CN(m_pSurface);

	m_pSurface->SetVisible(false);
	m_pSurface->InitializeOBB();

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);

	m_pFont = std::make_shared<Font>(L"Basis_Grotesque_Pro.fnt", true);
	m_pKeyTexture = GetComposite()->MakeTexture(L"Key-Dark-1024.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	m_keyStates[0] = ActiveObject::state::NOT_INTERSECTED;
	m_keyStates[1] = ActiveObject::state::NOT_INTERSECTED;

	m_keyObjects[0] = nullptr;
	m_keyObjects[1] = nullptr;

	m_typed = "";

	m_pLayout = new UIKeyboardLayout();
	m_pLayout->CreateQWERTYLayout();
	InitializeQuadsWithLayout();

	GetComposite()->SetVisible(false);

Error:
	return r;
}

RESULT UIKeyboard::InitializeQuadsWithLayout() {
	RESULT r = R_PASS;
	
	FlatContext *pQuadTextures = GetDOS()->AddFlatContext();

	auto& layout = m_pLayout->GetKeys();

	//not flexible, TODO: take max of rows?
	float keyDimension = m_surfaceWidth / (float)layout[0].size();

	int rowIndex = 0;
	int colIndex = 0;
	for (auto& row : layout) {
		for (auto& key : row) {
			if (pQuadTextures->HasChildren()) pQuadTextures->ClearChildren();

			auto keyBack = pQuadTextures->AddQuad(2.0f, 2.0f, point(0.0f, 0.0f, 0.0f));
			keyBack->SetColorTexture(m_pKeyTexture.get());

			pQuadTextures->AddText(m_pFont, key->m_letter, 0.2f, true);
			GetDOS()->RenderToTexture(pQuadTextures);

			std::shared_ptr<quad> pQuad = GetComposite()->AddQuad(keyDimension, keyDimension);
			pQuad->SetColorTexture(pQuadTextures->GetFramebuffer()->GetTexture());
			pQuad->ScaleX(1.0f / (keyDimension / (0.5f*key->m_width)));

			float rowCount = (float)layout.size();
			float zPos = (m_surfaceHeight / rowCount) * (rowIndex - (rowCount / 2.0f) + 0.5f);

			float xPos = m_surfaceWidth * key->m_left + (key->m_width / 2.0f) - (m_surfaceWidth / 2.0f);

			pQuad->SetPosition(point(xPos, 0.01f, zPos) + m_pSurface->GetPosition());
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
	for (auto &mallet : { m_pLeftMallet->GetMalletHead(), m_pRightMallet->GetMalletHead() })
	{
		CollisionManifold manifold = m_pSurface->Collide(mallet);

		pObj = manifold.GetObjectA();
		if (pObj == nullptr)
			pObj = manifold.GetObjectB();
		CN(pObj);

		if (manifold.NumContacts() > 0) {

			if (m_keyStates[i] == ActiveObject::state::NOT_INTERSECTED) { 

				// forces mallet head to be above collision surface
				if (manifold.GetNormal().y() > 0.0f) { 
					
					auto key = CollisionPointToKey(manifold);
					CNR(key, R_OBJECT_NOT_FOUND);
					m_typed += key->m_letter;
					m_keyObjects[i] = key->m_pQuad;
					OVERLAY_DEBUG_SET("str key", m_typed.c_str());
				}

				m_keyStates[i] = ActiveObject::state::INTERSECTED;
				CNR(m_keyObjects[i], R_OBJECT_NOT_FOUND);

				point ptPosition = m_keyObjects[i]->GetPosition();
				GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(

					m_keyObjects[i].get(),
					point(ptPosition.x(), -0.025f, ptPosition.z()),
					m_keyObjects[i]->GetOrientation(),
					m_keyObjects[i]->GetScale(),
					0.1f,
					AnimationCurveType::EASE_OUT_QUAD,
					AnimationFlags()
				);
			}
		}
		else {
			m_keyStates[i] = ActiveObject::state::NOT_INTERSECTED;
			if (m_keyObjects[i] != nullptr) {

				point ptPosition = m_keyObjects[i]->GetPosition();
				GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
					m_keyObjects[i].get(),
					point(ptPosition.x(), 0.01f, ptPosition.z()),
					m_keyObjects[i]->GetOrientation(),
					m_keyObjects[i]->GetScale(),
					0.1f,
					AnimationCurveType::EASE_OUT_QUAD,
					AnimationFlags()
				);

				m_keyObjects[i] = nullptr;
			}
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

//TODO:
//The collision information needed for the keyboard is currently deeper
//than what InteractionObjectEvents offer.  
//A future InteractionEngine solution has potential to be cleaner arch.
RESULT UIKeyboard::Notify(InteractionObjectEvent *oEvent) {
	
	switch (oEvent->m_eventType) {
	case ELEMENT_INTERSECT_ENDED: {
//		m_pTyped->SetText(m_pTyped->GetText() + 'a', 1.0f);
	} break;
	}

	return R_PASS;
}

RESULT UIKeyboard::ShowKeyboard() {

	auto fnStartCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		GetComposite()->SetPosition(point(0.0f, -1.0f, 5.0f));
	Error:
		return r;
	};

	auto fnEndCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		pKeyboard->GetComposite()->SetVisible(true);
		pKeyboard->HideSurface();
		m_pLeftMallet->Show();
		m_pRightMallet->Show();
	Error:
		return r;
	};

	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		m_ptSurface,
		GetComposite()->GetOrientation(),
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

//TODO: A good amount of code in the layout and in this function currently assumes
// that the keyboard is a grid
UIKey* UIKeyboard::CollisionPointToKey(CollisionManifold& manifold) {
	RESULT r = R_PASS;

	point pt = manifold.GetContactPoint(0).GetPoint();
	point ptCenter = GetComposite()->GetPosition();
	OVERLAY_DEBUG_SET("pt", pt);

	auto& keyboardLayout = m_pLayout->GetKeys();
	int rowIndex = (pt.z() - ptCenter.z() + (m_surfaceHeight / 2.0f)) / m_surfaceHeight * keyboardLayout.size();
	CBR(rowIndex >= 0 && rowIndex < keyboardLayout.size(), R_OBJECT_NOT_FOUND);

	float xPos = (pt.x() - ptCenter.x() + (m_surfaceWidth / 2.0f)) / m_surfaceWidth;
	OVERLAY_DEBUG_SET("xpo", xPos);
	auto& row = keyboardLayout[rowIndex];

	for (int i = (int)row.size() - 1; i >= 0; i--) {
		auto k = row[i];
		float pos = xPos - (k->m_width / 2.0f);
		if (k->m_left <= pos) {
			CBR(k->m_left + k->m_width >= pos, R_OBJECT_NOT_FOUND);
			return k.get();
		}
	}

Error:
	return nullptr;
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
