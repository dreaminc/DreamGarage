#include "UIKeyboard.h"
#include "DreamOS.h"
#include "Primitives/composite.h"

#include "PhysicsEngine/CollisionManifold.h"
#include "InteractionEngine/AnimationItem.h"

UIKey::UIKey() {}

UIKey::UIKey(float left, float width, std::string& letter) 
{
	m_left = left;
	m_width = width;
	m_letter = letter;
}

UIMallet::UIMallet(DreamOS *pDreamOS) 
{
	m_pHead = pDreamOS->AddSphere(0.02f, 10.0f, 10.0f);
	pDreamOS->AddInteractionObject(m_pHead);
	m_pHead->SetVisible(false);
}

UIKeyboard::UIKeyboard(DreamOS *pDreamOS, void *pContext) :
	DreamApp<UIKeyboard>(pDreamOS, pContext)
{
}

RESULT UIKeyboard::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	std::vector<UIKey*> row;
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

	// TODO: going to need something much more advanced than this soon
	// TODO: special characters (backspace) (shift) TODO: multi-character ('.com')
	//
	//m_QWERTY = { "qwertyuiop","asdfghjkl","zxcvbnm" };
	m_QWERTY.clear();
	float left = 0.0f;
	for (auto c : "qwertyuiop") {
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_QWERTY.emplace_back(row);

	row.clear();
	left = 0.05f;
	for (auto c : "asdfghjkl") {
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_QWERTY.emplace_back(row);

	row.clear();
	left = 0.15f;
	for (auto c : "zxcvbnm") {
		std::string ch = std::string(1, c); 
		auto k = new UIKey(left, 0.1f, ch);
		row.emplace_back(k);
		left += 0.1f;
	}
	row.pop_back();
	m_QWERTY.emplace_back(row);

	row.clear();
	left = 0.25f;
	{
		std::string ch = " ";
		auto k = new UIKey(left, 0.35f, ch);
		row.emplace_back(k);
	}
	m_QWERTY.emplace_back(row);

	InitializeQuadsWithLayout(m_QWERTY);
	GetComposite()->SetVisible(false);

Error:
	return r;
}

RESULT UIKeyboard::InitializeQuadsWithLayout(std::vector<std::vector<UIKey*>> layout) {
	RESULT r = R_PASS;
	
	FlatContext *pQuadTextures = GetDOS()->AddFlatContext();

	//not flexible
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
			pQuad->ScaleX(1.0f / (0.1f / key->m_width));

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

	point ptOffset = point(0.0f, 0.0f, -0.2f);
	RotationMatrix qOffset = RotationMatrix();
	VirtualObj *pObj = nullptr;

	InteractionEngineProxy *pProxy = nullptr;
	DreamOS *pDOS = GetDOS();

	CN(pDOS);
	pProxy = pDOS->GetInteractionEngineProxy();
	CN(pProxy);

	hand *pHand = pDOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
	CN(pHand);
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->m_pHead->MoveTo(pHand->GetPosition() + point(qOffset * ptOffset));

	pHand = pDOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);
	CN(pHand);

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->m_pHead->MoveTo(pHand->GetPosition() + point(qOffset * ptOffset));

	int i = 0;
	for (auto &mallet : { m_pLeftMallet->m_pHead, m_pRightMallet->m_pHead })
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

	GetComposite()->SetPosition(point(0.0f, -1.0f, 5.0f));
	auto fnCallback = [&](void *pContext) {
		RESULT r = R_PASS;
		UIKeyboard *pKeyboard = reinterpret_cast<UIKeyboard*>(pContext);
		CN(pKeyboard);
		pKeyboard->GetComposite()->SetVisible(true);
		pKeyboard->HideSurface();
		pKeyboard->ShowMallets();
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
		nullptr,
		fnCallback,
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
		pKeyboard->HideMallets();
	Error:
		return r;
	};

	GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		point(0.0f, -1.0f, 5.0f),
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

RESULT UIKeyboard::HideMallets() {
	RESULT r = R_PASS;
	CR(m_pLeftMallet->m_pHead->SetVisible(false));
	CR(m_pRightMallet->m_pHead->SetVisible(false));
Error:
	return r;
}

RESULT UIKeyboard::ShowMallets() {
	RESULT r = R_PASS;
	CR(m_pLeftMallet->m_pHead->SetVisible(true));
	CR(m_pRightMallet->m_pHead->SetVisible(true));
Error:
	return r;
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

	auto& keyboardLayout = m_QWERTY;
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
			return k;
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
