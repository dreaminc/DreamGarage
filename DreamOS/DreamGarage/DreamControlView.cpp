#include "DreamControlView.h"
#include "DreamOS.h"
#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"
#include "DreamConsole/DreamConsole.h"

DreamControlView::DreamControlView(DreamOS *pDreamOS, void *pContext) :
	DreamApp<DreamControlView>(pDreamOS, pContext)
{
}

RESULT DreamControlView::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	m_vNormal = vector::jVector().RotateByQuaternion(quaternion::MakeQuaternionWithEuler(-(float)M_PI / 3.0f, 0.0f, 0.0f));

	m_pViewQuad = GetComposite()->AddQuad(1.0f, 1.0f, 1, 1, nullptr, m_vNormal);
	CN(m_pViewQuad);
	m_pViewQuad->SetPosition(m_ptHiddenPosition);
	CR(m_pViewQuad->SetVisible(false));

	m_viewState = State::HIDDEN;

	m_ptHiddenPosition = point(0.0f, -0.25f, 5.0f);
	m_ptVisiblePosition = point(0.0f, -0.25f, 4.0f);

	m_hiddenScale = 0.2f;
	m_visibleScale = 2.0f;

	m_hideThreshold = -0.15f;
	m_showThreshold = -0.5f;

Error:
	return r;
}

RESULT DreamControlView::OnAppDidFinishInitializing(void *pContext) {
	return R_PASS;
}

RESULT DreamControlView::Update(void *pContext) {
	RESULT r = R_PASS;

	quaternion qHeadRotation = GetDOS()->GetCameraOrientation();

	RotationMatrix matOffset = RotationMatrix();
	matOffset.SetQuaternionRotationMatrix(qHeadRotation);
	point ptOffset = point(0.0f, 0.0f, -1.0f);
	ptOffset = matOffset * ptOffset;

	switch (m_viewState) {

	case State::VISIBLE: {
		if (ptOffset.y() > m_hideThreshold)
			Hide();
	} break;
		
	case State::HIDDEN: {
		if (ptOffset.y() < m_showThreshold)
			Show();
	} break;
	
	}

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
		m_pViewQuad.get(),
		m_ptVisiblePosition,
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
		m_pViewQuad.get(),
		m_ptHiddenPosition,
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

RESULT DreamControlView::SetSharedViewContext(std::shared_ptr<DreamBrowser> pContext) {
	RESULT r = R_PASS;

	m_pSharedViewContext = pContext;
	CNR(pContext, R_OBJECT_NOT_FOUND);
	CR(m_pViewQuad->UpdateColorTexture(m_pSharedViewContext->GetScreenTexture().get()));
	float width = m_pSharedViewContext->GetWidth();
	float height = m_pSharedViewContext->GetHeight();
	float scale = 1.0f / 6.0f;
	CR(m_pViewQuad->UpdateParams(width * scale, height * scale, m_vNormal));
	
	m_pViewQuad->FlipUVVertical();

Error:
	return r;
}

std::shared_ptr<quad> DreamControlView::GetViewQuad() {
	return m_pViewQuad;
}

RESULT DreamControlView::SetViewState(State state) {
	m_viewState = state;
	return R_PASS;
}
