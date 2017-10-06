#include "DreamControlView.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "DreamConsole/DreamConsole.h"

#include "UI/UIMallet.h"
#include "UI/UIView.h"

#include <functional>
#include <stack>

DreamControlView::DreamControlView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlView>(pDreamOS, pContext)
{
	//empty
}

RESULT DreamControlView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();

	m_vNormal = vector::jVector().RotateByQuaternion(quaternion::MakeQuaternionWithEuler(-(float)M_PI / 3.0f, 0.0f, 0.0f));
	
	m_pView = GetComposite()->AddUIView(pDreamOS);
	CN(m_pView);
	m_pScrollView = m_pView->AddUIScrollView();
	CN(m_pScrollView);
	m_pViewQuad = m_pView->AddQuad(.48f, .27f, 1, 1, nullptr, m_vNormal);
	CN(m_pViewQuad);
	m_pViewQuad->SetMaterialAmbient(0.75f);
	CR(m_pViewQuad->SetVisible(false));

	m_viewState = State::HIDDEN;

	m_ptHiddenPosition = point(0.0f, -0.25f, 5.0f);
	m_ptVisiblePosition = point(0.0f, -0.25f, 4.0f);

	m_hiddenScale = 0.2f;
	m_visibleScale = 2.0f;

	m_hideThreshold = 0.20f;
	m_showThreshold = -0.35f;

	m_pLeftMallet = new UIMallet(GetDOS());
	CN(m_pLeftMallet);

	m_pRightMallet = new UIMallet(GetDOS());
	CN(m_pRightMallet);
	m_pRightMallet->Show();
	m_pLeftMallet->Show();

	pDreamOS->AddInteractionObject(m_pLeftMallet->GetMalletHead());
	pDreamOS->AddInteractionObject(m_pRightMallet->GetMalletHead());
	//pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), INTERACTION_EVENT_SELECT, this);

	for (int i = 0; i < InteractionEventType::INTERACTION_EVENT_INVALID; i++) {
		CR(pDreamOS->AddAndRegisterInteractionObject(m_pViewQuad.get(), (InteractionEventType)(i), this));
	}

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;
	DreamOS *pDreamOS = GetDOS();
	vector vLook = GetDOS()->GetCamera()->GetLookVector();

	RotationMatrix qOffset = RotationMatrix();
	hand *pHand = pDreamOS->GetHand(hand::HAND_TYPE::HAND_LEFT);
	CNR(pHand, R_OBJECT_NOT_FOUND);
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pLeftMallet)
		m_pLeftMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pLeftMallet->GetHeadOffset()));

	pHand = pDreamOS->GetHand(hand::HAND_TYPE::HAND_RIGHT);
	CNR(pHand, R_OBJECT_NOT_FOUND);

	qOffset = RotationMatrix();
	qOffset.SetQuaternionRotationMatrix(pHand->GetOrientation());

	if (m_pRightMallet)
		m_pRightMallet->GetMalletHead()->MoveTo(pHand->GetPosition() + point(qOffset * m_pRightMallet->GetHeadOffset()));

	switch (m_viewState) {

	case State::VISIBLE: {
		if (vLook.y() > m_hideThreshold)
			Hide();
	} break;
		
	case State::HIDDEN: {
		if (vLook.y() < m_showThreshold)
			Show();
	} break;
	
	}
	
Error:
	return r;
}

RESULT DreamControlView::Notify(InteractionObjectEvent *pInteractionEvent) {
	RESULT r = R_PASS;
	m_pViewQuad->FlipUVVertical();
	switch (pInteractionEvent->m_eventType) {
	case (InteractionEventType::INTERACTION_EVENT_SELECT_DOWN): {
		m_pViewQuad->FlipUVVertical();
		CR(GetDOS()->GetHMD()->GetSenseController()->SubmitHapticImpulse(CONTROLLER_TYPE(0), SenseController::HapticCurveType::SINE, 1.0f, 20.0f, 1));
	} break;
	}
Error:
	return r;
}

RESULT DreamControlView::Shutdown(void *pContext) {
	return R_PASS;
}

DreamControlView *DreamControlView::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamControlView *pDreamControlView = new DreamControlView(pDreamOS, pContext);
	return pDreamControlView;
}

RESULT DreamControlView::Show() {
	RESULT r = R_PASS;
	UpdateCompositeWithCameraLook(0.6f, -0.20f);	//depth, height
	m_ptVisiblePosition = GetComposite()->GetPosition();
	m_ptHiddenPosition = GetComposite()->GetPosition() - point(0.0f, 1.0f, 0.0f);

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
		GetComposite(),
		m_ptVisiblePosition,
		GetComposite()->GetOrientation(),
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

RESULT DreamControlView::Hide() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		SetViewState(State::HIDE);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		GetViewQuad()->SetVisible(false);
		SetViewState(State::HIDDEN);
		return R_PASS;
	};

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		GetComposite(),
		m_ptHiddenPosition,
		GetComposite()->GetOrientation(),
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

RESULT DreamControlView::SetSharedViewContext() {
	RESULT r = R_PASS;

	texture *tempTexture = GetDOS()->MakeTexture(L"1920x1080.jpg", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	CR(m_pViewQuad->SetDiffuseTexture(tempTexture));

	float width = 1920.0f;
	float height = 1080.0f;
	float scale = 1.0f / 10000.0f;

	CR(m_pViewQuad->UpdateParams(width * scale, height * scale, m_vNormal));

	//m_pViewQuad->FlipUVVertical();

Error:
	return r;
}

/* old stuffs
RESULT DreamControlView::SetSharedViewContext(std::shared_ptr<DreamBrowser> pContext) {
	RESULT r = R_PASS;

	m_pSharedViewContext = pContext;
	CNR(pContext, R_OBJECT_NOT_FOUND);
	CR(m_pViewQuad->SetDiffuseTexture(m_pSharedViewContext->GetScreenTexture().get()));

	float width = m_pSharedViewContext->GetWidth();
	float height = m_pSharedViewContext->GetHeight();
	float scale = 1.0f / 6.0f;

	CR(m_pViewQuad->UpdateParams(width * scale, height * scale, m_vNormal));
	
	m_pViewQuad->FlipUVVertical();

Error:
	return r;
}
*/

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

RESULT DreamControlView::SetViewState(State state) {
	m_viewState = state;
	return R_PASS;
}
