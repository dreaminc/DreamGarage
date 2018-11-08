#include "HMD.h"

RESULT HMD::AttachHand(hand *pHand, HAND_TYPE type) {
	hand::HandState state;
	state.handType = type;
	state.ptPalm = point(0.0f, 0.0f, 0.0f);
	if (type == HAND_TYPE::HAND_LEFT) {
		m_pLeftHand = pHand;
		//m_pLeftHand->SetHandState(state);
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		m_pRightHand = pHand;
		//m_pRightHand->SetHandState(state);
	}
	else {
		return R_FAIL;
	}
	return R_PASS;
}

hand *HMD::GetHand(HAND_TYPE type) {
	if (type == HAND_TYPE::HAND_LEFT) {
		return m_pLeftHand;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		return m_pRightHand;
	}
	else {
		return nullptr;
	}
}

SenseController* HMD::GetSenseController() {
	return m_pSenseController;
}
