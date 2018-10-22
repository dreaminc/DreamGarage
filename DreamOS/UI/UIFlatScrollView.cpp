#include "UIFlatScrollView.h"
#include "Primitives/Framebuffer.h"
#include "DreamOS.h"

#include "DreamUserControlArea/DreamContentSource.h"
#include "DreamGarage/UITabView.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "UI/UIButton.h"

UIFlatScrollView::UIFlatScrollView(HALImp *pHALImp, DreamOS *pDreamOS) :
UIView(pHALImp, pDreamOS)
{
	// empty
	
	pDreamOS->RegisterSubscriber(SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE, this);
}

UIFlatScrollView::~UIFlatScrollView() 
{
	// empty
}

RESULT UIFlatScrollView::Update() {
	RESULT r = R_PASS;

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;
	double tDiff = (msNow - m_frameMs) * (90.0);

	point ptDiff = point(0.0f, 0.0f, (m_velocity * (float)(tDiff) / 2.0f));

	if (HasChildren()) {
		auto pChildren = GetChildren();

		float offset = GetPosition().z() + ptDiff.z();
		int minIndex = (int)(pChildren.size()) - 4;

		// clamp offset to scrolling bounds based on amount of children
		offset = std::min(0.0f, std::max(offset, -(minIndex * m_scrollSnapDistance)));
		float offsetIndex = (offset) / m_scrollSnapDistance;
		ptDiff.z() = offset - GetPosition().z();

		if (offsetIndex <= 0 && offsetIndex >= -(minIndex)) {
			SetPosition(GetPosition() + ptDiff);
		}
		
	}

	if (m_pRenderContext != nullptr && m_pRenderQuad != nullptr) {
		m_pRenderContext->RenderToQuad(m_pRenderQuad.get(), 0.0f, 0.0f);
	}
			
	m_frameMs = msNow;
	return r;
}

std::vector<std::shared_ptr<UIButton>> UIFlatScrollView::GetTabButtons() {
	return m_pTabButtons;
}

texture *UIFlatScrollView::GetCurrentTexture() {
	return m_pCurrentTexture;
}

RESULT UIFlatScrollView::SetScrollFlag(bool fCanScroll, int index) {
	m_fCanScrollFlag[index] = fCanScroll;

	if (!m_fCanScrollFlag[0] && !m_fCanScrollFlag[1]) {
		m_velocity = 0.0f;
	}

	return R_PASS;
}

RESULT UIFlatScrollView::SetBounds(float width, float height) {
	RESULT r = R_PASS;

	CN(m_pRenderContext);
	m_pRenderContext->SetIsAbsolute(true);
	m_pRenderContext->SetAbsoluteBounds(width, height);

Error:
	return r;
}

std::shared_ptr<FlatContext> UIFlatScrollView::GetRenderContext() {
	return m_pRenderContext;
}

RESULT UIFlatScrollView::SetRenderQuad(std::shared_ptr<quad> pRenderQuad, std::shared_ptr<FlatContext> pFlatContext) {
	m_pRenderQuad = pRenderQuad;
	m_pRenderContext = pFlatContext;
	/*
	if (m_pRenderContext == nullptr) {
		m_pRenderContext = m_pDreamOS->MakeComposite()->MakeFlatContext();
		//m_pRenderContext->SetScaleToFit(false);
		//m_pRenderContext->AddObject(this);
		//m_pRenderContext->SetPosition(0.0f, 3.0f, 5.0f);
		m_pRenderContext->SetIsAbsolute(true);
		m_pRenderContext->SetAbsoluteBounds(m_pRenderQuad->GetWidth(), m_pRenderQuad->GetHeight());
	}
	//*/
	//m_pRenderContext->SetIsAbsolute(true);
	//m_pRenderContext->SetAbsoluteBounds(m_pRenderQuad->GetWidth(), m_pRenderQuad->GetHeight());

	return R_PASS;
}

RESULT UIFlatScrollView::SetTabWidth(float tabWidth) {
	m_tabWidth = tabWidth;
	return R_PASS;
}

RESULT UIFlatScrollView::SetTabHeight(float tabHeight) {
	m_tabHeight = tabHeight;
	return R_PASS;
}

RESULT UIFlatScrollView::SetScrollSnapDistance(float snapDistance) {
	m_scrollSnapDistance = snapDistance;
	return R_PASS;
}

RESULT UIFlatScrollView::Snap() {
	RESULT r = R_PASS;
	
	float rawIndex = GetPosition().z() / m_scrollSnapDistance;
	float endZPosition = (float)(std::round(rawIndex)) * m_scrollSnapDistance;
	//float distance = std::abs(endZPosition - GetPosition().z());
	float distance = 0.05f;

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		this,
		point(GetPosition().x(), GetPosition().y(), endZPosition),
		GetOrientation(),
		GetScale(),
		distance,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		nullptr,
		nullptr,
		this
	))

Error:
	return r;
}

RESULT UIFlatScrollView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		if (m_fCanScrollFlag[pEvent->state.type] && IsVisible()) {
			m_velocity = pEvent->state.ptTouchpad.y() * PAD_MOVE_CONSTANT;
			if (m_velocity == 0.0f) // && IsAnimating
			{
				CR(Snap());
			}
		}

	} break;
	}

Error:
	return r;
}
