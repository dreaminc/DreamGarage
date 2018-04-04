#include "UIFlatScrollView.h"
#include "Primitives/Framebuffer.h"
#include "DreamOS.h"

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

	SetVisible(true);
	//*
	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;
	double tDiff = (msNow - m_frameMs) * (90.0);

	point ptDiff = point(0.0f, 0.0f, -(m_velocity * (float)(tDiff) / 10000000.0f));

	SetPosition(GetPosition() + ptDiff);
	//GetRenderContext()->SetPosition(GetPosition() + ptDiff);
	//*/

	//*
	if (m_pRenderContext != nullptr && m_pRenderQuad != nullptr) {
		m_pRenderContext->RenderToQuad(m_pRenderQuad.get(), 0.0f, 0.0f);
		//m_pCurrentTexture = m_pRenderContext->GetFramebuffer()->GetColorTexture();
	}
	//*/
	//SetVisible(false);
		
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

	if (fCanScroll == false) {
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

RESULT UIFlatScrollView::Notify(SenseControllerEvent *pEvent) {
	RESULT r = R_PASS;

	switch (pEvent->type) {
	case SenseControllerEventType::SENSE_CONTROLLER_PAD_MOVE: {
		if (m_fCanScrollFlag[pEvent->state.type] && IsVisible()) {
			m_velocity = pEvent->state.ptTouchpad.y() * PAD_MOVE_CONSTANT;
			if (m_velocity == 0.0f) // && IsAnimating
			{

			}
			else if (m_velocity != 0.0f) // && IsAnimating
			{

			}
		}

	} break;
	}

	return r;
}
