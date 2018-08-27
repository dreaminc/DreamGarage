#include "user.h"
#include "Primitives/quad.h"
#include "Primitives/model/model.h"

#include "Core/Utilities.h"
#include "Sandbox/CommandLineManager.h"
#include "DreamOS.h"

user::user(HALImp* pHALImp) :
	composite(pHALImp)
{
	m_pHeadTextures.clear();

	Initialize();
}

user::user(HALImp* pHALImp, DreamOS *pDreamOS) :
	composite(pHALImp),
	m_pDreamOS(pDreamOS)
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
	m_pMouthComposite = MakeComposite();

	m_pMouthComposite->SetScale(0.028f);
	m_pMouthComposite->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pMouthComposite->SetOrientationOffset(vHeadOffset);
	m_pMouthComposite->SetMaterialShininess(2.0f, true);

	m_pHead = AddModel(util::StringToWideString(strHeadPath));
	
	m_pHead->SetScale(0.028f);
	//m_pHead->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pHead->SetOrientationOffset(vHeadOffset);
	m_pHead->SetMaterialShininess(2.0f, true);

	/*
	m_pHead->SetMaterialShininess(2.0f, true);
	m_pHead->SetOrientationOffset(vHeadOffset);
	m_pHead->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pHead->SetScale(0.028f);
	//*/

	m_pMouth = m_pMouthComposite->AddModel(util::StringToWideString(k_strMouthPath));
	/*
	m_pMouth->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pMouth->SetOrientationOffset(vHeadOffset);
	m_pMouth->SetMaterialShininess(2.0f, true);
	m_pMouth->SetScale(0.028f); 
	//*/
	
	float scale = 1.0f / 0.028f;
	m_pMouthQuad = m_pMouthComposite->AddQuad(0.25f, 0.25f,1,1,nullptr,vector::jVector());
	m_pMouthQuad->SetPosition(point(0.0f, 0.0f, 0.25f));
	m_pMouthQuad->SetScale(scale);
	//m_pMouthQuad->RotateXByDeg(-90.0f)
	/*
	m_pMouthQuad->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pMouthQuad->SetOrientationOffset(vHeadOffset);
	m_pMouthQuad->SetMaterialShininess(2.0f, true);
	//*/
	
	m_pMouthQuad->RotateXByDeg(90.0f);
	
	m_mouthStates.push_back(MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));

	m_pMouthTexture = MakeTexture(L"mouth.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pMouthTexture1 = MakeTexture(L"mouth_01.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pMouthTexture2 = MakeTexture(L"mouth_02.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);
	m_pMouthTexture3 = MakeTexture(L"mouth_03.png", texture::TEXTURE_TYPE::TEXTURE_DIFFUSE);

	m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture.get());
	m_pMouthQuad->SetDiffuseTexture(m_pMouthTexture.get());
	
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

RESULT user::SetDreamOS(DreamOS *pDreamOS) {
	m_pDreamOS = pDreamOS;

	if (m_pDreamOS != nullptr) {
		//m_pDreamOS->AddObjectToUIGraph(m_pMouth.get());
		//m_pDreamOS->AddObjectToUIGraph(m_pMouthQuad.get());
		m_pDreamOS->AddObjectToUIGraph(m_pMouthComposite.get());
	}

	return R_PASS;
}

RESULT user::SetMouthPosition(point ptPosition) {
	//m_pMouthQuad->SetPosition(ptPosition);
	//m_pMouth->SetPosition(ptPosition);
	m_pMouthComposite->SetPosition(ptPosition);
	return R_PASS;
}

RESULT user::SetMouthOrientation(quaternion qOrientation) {
	//m_pMouthQuad->SetOrientation(quaternion::MakeQuaternionWithEuler(90.0f * (float)M_PI / 180.0f, 0.0f, 0.0f) * qOrientation);
	//m_pMouthQuad->SetOrientation(qOrientation);
	//m_pMouth->SetOrientation(qOrientation);
	m_pMouthComposite->SetOrientation(qOrientation);
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

//	m_pMouth->Scale(0.01f + 8.0f * mouthScale);
	/*
	if (mouthScale != 0.0f) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture1.get());
	}
	//*/
	//*
	if (mouthScale < 0.25f) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture.get());
	}
	else if (mouthScale < 0.5f) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture3.get());
	}
	else if (mouthScale < 0.75f) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture2.get());
	}
	else if (mouthScale < 1.0f) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture1.get());
	}
	//*/

Error:
	return r;
}