#include "user.h"
#include "Primitives/quad.h"
#include "Primitives/model/model.h"

#include "Core/Utilities.h"
#include "Sandbox/CommandLineManager.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp)
{
	m_pHeadTextures.clear();

	Initialize();
}

RESULT user::Initialize() {
	RESULT r = R_PASS;

	InitializeOBB();

	std::string strHeadPath = "default";

#ifndef PRODUCTION_BUILD
	CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	strHeadPath = pCommandLineManager->GetParameterValue("head.path");
#endif

#ifndef _DEBUG
	//SetScale(0.018f);

	vector vHeadOffset;
	if (strHeadPath == "default") {
		strHeadPath = k_strDefaultHeadPath;
		vHeadOffset = vector(0.0f, (float)M_PI, 0.0f);
	}
	else {
		vHeadOffset = vector(-(float)(M_PI_2), (float)(M_PI), 0.0f);
	}

	m_pHead = AddModel(util::StringToWideString(strHeadPath));
	m_pHead->SetOrientationOffset(vHeadOffset);
	m_pHead->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pHead->SetScale(0.028f);
	//m_pHead->SetScale(0.018f);

#else
	//m_pHead = AddComposite();
	//m_pHead->AddVolume(0.2f);

	m_pHead = AddModel(L"\\cube.obj");
	//m_pHead->SetVertexColor(COLOR_BLUE);
	//m_pHead->SetMaterialDiffuseColor(COLOR_BLUE, true);

#endif


#ifndef _DEBUG
	// for now the mouth is in a hardcoded position attached to the face model

	m_pMouth = m_pHead->AddQuad(0.3, 1.0);
	m_pMouth->MoveTo(0.0f, 12.0f, 8.35f);
	
	m_pMouth->RotateXByDeg(90);
	m_pMouth->RotateZByDeg(90);
	
	m_pMouthTexture = MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	
	m_pMouth->SetMaterialTexture(MaterialTexture::Ambient, m_pMouthTexture.get());
	m_pMouth->SetMaterialTexture(MaterialTexture::Diffuse, m_pMouthTexture.get());
	
	m_pMouth->Scale(0.1f);

	m_pLeftHand = AddHand(HAND_TYPE::HAND_LEFT);
	m_pLeftHand->OnLostTrack();
	
	m_pRightHand = AddHand(HAND_TYPE::HAND_RIGHT);
	m_pRightHand->OnLostTrack();
#endif

	SetPosition(point(0.0f, 0.0f, 0.0f));

	//Error:
	return r;
}

std::shared_ptr<composite> user::GetHead() {
	return m_pHead;
}

std::shared_ptr<hand> user::GetHand(HAND_TYPE type) {
	if (type == HAND_TYPE::HAND_LEFT) {
		return m_pLeftHand;
	}
	else if (type == HAND_TYPE::HAND_RIGHT) {
		return m_pRightHand;
	}
	return nullptr;
}

RESULT user::Activate(user::CONTROLLER_TYPE type) {

	SetVisible(true);
	m_pLeftHand->SetVisible(false);
	m_pRightHand->SetVisible(false);

	return R_PASS;
}

//TODO: why doesn't this use hand::SetHandState(pHandState)
RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point ptHand = pHandState.ptPalm;
	ptHand = ptHand - GetPosition(true) + point(0.0f, -0.35f, HEAD_POS);

	quaternion qHandOrientation = pHandState.qOrientation;

	std::shared_ptr<hand> pHand = nullptr;

	switch (pHandState.handType) {
		case HAND_TYPE::HAND_LEFT: {
			pHand = m_pLeftHand;
		} break;

		case HAND_TYPE::HAND_RIGHT: {
			pHand = m_pRightHand;
		} break;
	}

	CN(pHand);

	pHand->SetPosition(ptHand);
	pHand->SetOrientation(qHandOrientation);
	pHand->SetTracked(pHandState.fTracked);
	pHand->SetVisible(pHand->IsTracked());

Error:
	return r;
}

RESULT user::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;
	
	CN(m_pMouth);

	m_pMouth->Scale(0.01f + 8.0f * mouthScale);

Error:
	return r;
}