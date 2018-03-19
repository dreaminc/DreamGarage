#include "UIFlatScrollView.h"
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
		
	return r;
}

std::vector<std::shared_ptr<UIButton>> UIFlatScrollView::GetTabButtons() {
	return m_pTabButtons;
}

RESULT UIFlatScrollView::SetScrollFlag(bool fCanScroll, int index) {
	m_fCanScrollFlag[index] = fCanScroll;

	if (fCanScroll == false) {
		m_velocity = 0.0f;
	}

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
