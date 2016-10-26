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
	
	// TODO: Make this programmatic 

	///*

	///*
	//std::shared_ptr<texture> pHeadTexture = MakeTexture(L"..\\Models\\face2\\faceP.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	std::shared_ptr<composite> pHead = AddModel(L"\\Models\\face4\\untitled.obj",
					   nullptr,//pHeadTexture.get(),
					   point(0.0f, 0.0f - 0.35f, 0.0f),
					   0.02f,
					   vector(0.0f, (float)M_PI, 0.0f));

	m_pHeads.push_back(pHead);
	//m_pHeadTextures.push_back(pHeadTexture);
	//*/

	/*
	pHead = AddModel(L"\\Models\\stormtrooper\\stormtrooper.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.003f,
						vector((float)M_PI_2, (float)M_PI, 0.0f));
	pHead->SetVisible(false);
	m_pHeads.push_back(pHead);
	//*/

	//pHead = AddComposite();
	//pHead->AddSphere(0.25f, 20, 20);
	
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