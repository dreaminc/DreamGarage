#include "user.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp)
{
	m_pHeadTextures.clear();
	m_pHeads.clear();

	Initialize();
}

RESULT user::Initialize() {
	RESULT r = R_PASS;
	
	std::shared_ptr<composite> pHead = AddModel(L"\\Models\\face4\\untitled.obj",
					   nullptr,
					   point(0.0f, 0.0f - 0.35f, 0.0f),
					   0.02f,
					   vector(0.0f, (float)M_PI, 0.0f));

	// for now the mouth is in a hardcoded position attached to the face model
	m_pMouth = AddQuad(0.3, 1.0);

	m_pMouth->RotateXByDeg(270);
	m_pMouth->MoveTo(0, 0.25f - 0.35f, -0.17f);

	m_pMouthTexture = MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	m_pMouth->SetColorTexture(m_pMouthTexture.get());


	pHead->AddChild(m_pMouth);
	m_pMouth->Scale(0.1f);

	m_pHeads.push_back(pHead);
	
	// Hands
	m_pLeftHand = AddHand();
	m_pRightHand = AddHand();

	SetPosition(point(0.0f, 0.0f, 0.0f));

	//Error:
	return r;
}

RESULT user::SwitchHeadModel() {
	RESULT r = R_PASS;
	bool fNext = false;

	CB(m_pHeads.size() > 1);

	for (auto &pHead : m_pHeads) {
		if (pHead->IsVisible()) {
			pHead->SetVisible(false);
			fNext = true;
			continue;
		}
		
		if (fNext) {
			pHead->SetVisible(true);
			fNext = false;
			break;
		}
	}

	if (fNext == true) {
		m_pHeads.front()->SetVisible(true);
	}

Error:
	return r;
}

RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	if (pHandState.handType == hand::HAND_LEFT) {
		m_pLeftHand->SetHandState(pHandState);
	}
	else if (pHandState.handType == hand::HAND_RIGHT) {
		m_pRightHand->SetHandState(pHandState);
	}

//Error:
	return r;
}

RESULT user::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;

	m_pMouth->Scale(0.01f + 0.1f * mouthScale);
//Error:
	return r;
}