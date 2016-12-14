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

	// for now the mouth is in a hardcoded position attached to the face model
	m_pMouth = AddQuad(0.3, 1.0);

	m_pMouth->RotateXByDeg(270);
	m_pMouth->MoveTo(0, 0.25f - 0.35f, -0.17f);

	m_pMouthTexture = MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	
	m_pMouth->SetMaterialTexture(MaterialTexture::Ambient, m_pMouthTexture.get());
	m_pMouth->SetMaterialTexture(MaterialTexture::Diffuse, m_pMouthTexture.get());

	m_pMouth->Scale(0.1f);
	pHead->AddChild(m_pMouth);

	m_pHeads.push_back(pHead);
	
	// Hands
	m_pLeapLeftHand = AddHand();
	m_pLeapRightHand = AddHand();

	m_pLeapLeftHand->SetVisible(false);
	m_pLeapRightHand->SetVisible(false);

	pHead->AddChild(m_pLeapLeftHand);
	pHead->AddChild(m_pLeapRightHand);

	m_pViveLeftModel = AddModel(L"\\Models\\face4\\LeftHand.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.015f,
						vector((float)(M_PI_2), (float)(-M_PI_2), 0.0f));
	
	m_pViveRightModel = AddModel(L"\\Models\\face4\\RightHand.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.015f,
						vector((float)(M_PI_2), (float)(M_PI_2), 0.0f));

	m_pViveLeftModel->SetVisible(false);
	m_pViveRightModel->SetVisible(false);

/*
	m_pLeapLeftModel = AddModel(L"\\Models\\face4\\LeftHand.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.015f,
						vector((float)(M_PI_2), (float)(-M_PI_2), 0.0f));
	
	m_pLeapRightModel = AddModel(L"\\Models\\face4\\RightHand.obj",
						nullptr,
						point(0.0f, 0.0f, 0.0f),
						0.015f,
						vector((float)(M_PI_2), (float)(M_PI_2), 0.0f));
	pHead->AddChild(m_pLeapLeftModel);
	pHead->AddChild(m_pLeapRightModel);


	m_pLeapLeftModel->SetVisible(false);
	m_pLeapRightModel->SetVisible(false);
//*/	

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
/*
std::shared_ptr<composite> user::GetHandModel(hand::HAND_TYPE type) {
	if (type == hand::HAND_LEFT) {
		return m_pLeapLeftModel;
	}
	else if (type == hand::HAND_RIGHT) {
		return m_pLeapRightModel;
	}
	return nullptr;
}
//*/

std::shared_ptr<composite> user::GetViveModel(hand::HAND_TYPE type) {
	if (type == hand::HAND_LEFT) {
		return m_pViveLeftModel;
	}
	else if (type == hand::HAND_RIGHT) {
		return m_pViveRightModel;
	}
	return nullptr;
}

RESULT user::Activate(user::CONTROLLER_TYPE type) {

	SetVisible(true);

	bool setController = (type == user::CONTROLLER_TYPE::LEAP_SKELETON);
	m_pLeapLeftHand->SetVisible(setController);
	m_pLeapRightHand->SetVisible(setController);

//	setController = (type == user::CONTROLLER_TYPE::LEAP_MODEL);
//	m_pLeapLeftModel->SetVisible(setController);
//	m_pLeapRightModel->SetVisible(setController);

	setController = (type == user::CONTROLLER_TYPE::VIVE_MODEL);
	m_pViveLeftModel->SetVisible(setController);
	m_pViveRightModel->SetVisible(setController);

	return R_PASS;
}

RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;
	point setHandConstant = point(0.0f, 0.0f, -0.25f);
	point ptModel = pHandState.ptPalm;
	ptModel += setHandConstant;

	quaternion baseRight = quaternion();
	baseRight *= quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, (float)M_PI_2);
	
	bool fHand = pHandState.fSkeleton && pHandState.fTracked;
	bool fModel = !pHandState.fSkeleton && pHandState.fTracked;

	if (pHandState.handType == hand::HAND_LEFT && pHandState.fOriented) {
		m_pLeapLeftHand->SetHandState(pHandState);
/*
		if (fHand != m_pLeapLeftHand->IsVisible()) {
			m_pLeapLeftHand->SetVisible(fHand);
		}
		if (fModel != m_pLeapLeftModel->IsVisible()) {
			m_pLeapLeftModel->SetVisible(fModel);
		}
		m_pLeapLeftModel->SetPosition(ptModel);

		baseRight.Reverse();
		m_pLeapLeftModel->SetOrientation(pHandState.qOrientation * baseRight);
//*/
	}
	else if (pHandState.handType == hand::HAND_RIGHT && pHandState.fOriented) {
		m_pLeapRightHand->SetHandState(pHandState);
/*
		if (fHand != m_pLeapRightHand->IsVisible()) {
			m_pLeapRightHand->SetVisible(fHand);
		}
		m_pLeapRightHand->SetHandState(pHandState);
		
		if (fModel != m_pLeapRightModel->IsVisible()) {
			m_pLeapRightModel->SetVisible(pHandState.fTracked && !pHandState.fSkeleton);
		}
		m_pLeapRightModel->SetPosition(ptModel);

		m_pLeapRightModel->SetOrientation(pHandState.qOrientation * baseRight);
//*/

	}
	else if (pHandState.handType == hand::HAND_LEFT && !pHandState.fOriented) {
		if (!m_pViveLeftModel->IsVisible())
			m_pViveLeftModel->SetVisible();
		m_pViveLeftModel->SetPosition(ptModel);
		m_pViveLeftModel->SetOrientation(pHandState.qOrientation);
	}
	else if (pHandState.handType == hand::HAND_RIGHT && !pHandState.fOriented) {
		if (!m_pViveRightModel->IsVisible())
			m_pViveRightModel->SetVisible();
		m_pViveRightModel->SetPosition(ptModel);
		m_pViveRightModel->SetOrientation(pHandState.qOrientation);
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