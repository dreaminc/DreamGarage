#include "user.h"
#include "Primitives/quad.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp)
{
	m_pHeadTextures.clear();
	m_pHeads.clear();

	Initialize();
}

RESULT user::Initialize() {
	RESULT r = R_PASS;

#ifndef _DEBUG
	m_pHead = AddModel(L"\\Models\\face4\\untitled.obj",
					   nullptr,
					   point(0.0f, 0.0f - 0.35f, HEAD_POS),
					   0.018f,
					   vector(0.0f, (float)M_PI, 0.0f));
#else
	m_pHead = AddComposite();
	m_pHead->AddVolume(0.2f);
#endif

	m_pHeads.push_back(m_pHead);

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

#ifndef _DEBUG
	// for now the mouth is in a hardcoded position attached to the face model
	m_pMouth = m_pHead->AddQuad(0.3, 1.0);
	m_pMouth->MoveTo(0.0f, -0.135f, 0.1f);

	m_pMouth->RotateXByDeg(270);
	m_pMouth->RotateZByDeg(90);

	m_pMouthTexture = MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	
	m_pMouth->SetMaterialTexture(MaterialTexture::Ambient, m_pMouthTexture.get());
	m_pMouth->SetMaterialTexture(MaterialTexture::Diffuse, m_pMouthTexture.get());

	m_pMouth->Scale(0.1f);
#endif

	// Hands
	m_pLeapLeftHand = AddHand();
	m_pLeapLeftHand->OnLostTrack();

	m_pLeapRightHand = AddHand();
	m_pLeapRightHand->OnLostTrack();

	SetPosition(point(0.0f, 0.0f, 0.0f));

	//Error:
	return r;
}

std::shared_ptr<composite> user::GetHead() {
	return m_pHeads[0];
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

RESULT user::Activate(user::CONTROLLER_TYPE type) {

	SetVisible(true);
	m_pLeapLeftHand->SetVisible(false);
	m_pLeapRightHand->SetVisible(false);

	return R_PASS;
}

RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	if (pHandState.handType == hand::HAND_LEFT) {
		m_pLeapLeftHand->SetHandState(pHandState);
		m_pLeapLeftHand->SetFrameOfReferenceObject(m_pHead, pHandState);
	}
	else if (pHandState.handType == hand::HAND_RIGHT) {
		m_pLeapRightHand->SetHandState(pHandState);
		m_pLeapRightHand->SetFrameOfReferenceObject(m_pHead, pHandState);
	}

//Error:
	return r;
}

RESULT user::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;
	
	if(m_pMouth)
		m_pMouth->Scale(0.01f + 0.1f * mouthScale);

//Error:
	return r;
}