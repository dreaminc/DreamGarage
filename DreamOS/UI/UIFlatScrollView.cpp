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

	auto tNow = std::chrono::high_resolution_clock::now().time_since_epoch();
	double msNow = std::chrono::duration_cast<std::chrono::milliseconds>(tNow).count();
	msNow /= 1000.0;
	double tDiff = (msNow - m_frameMs) * (90.0);

	point ptDiff = point(0.0f, 0.0f, -(m_velocity * (float)(tDiff) / 10000000.0f));

	SetPosition(GetPosition() + ptDiff);

	/*
	if (m_pRenderContext == nullptr) {
		m_pRenderContext = MakeFlatContext();
	}
	m_pRenderContext->RenderToTexture();
	m_pCurrentTexture = m_pRenderContext->GetFramebuffer()->GetColorTexture();
	//*/
		
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

RESULT UIFlatScrollView::AddObject(std::shared_ptr<DimObj> pObject) {
	RESULT r = R_PASS;

	CR(m_pRenderContext->AddObject(pObject));

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

			}
			else if (m_velocity != 0.0f) // && IsAnimating
			{

			}
		}

	} break;
	}

	return r;
}
