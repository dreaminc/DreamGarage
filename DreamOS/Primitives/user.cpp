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
	CN(m_pMouthComposite);

	m_pMouthComposite->SetScale(0.013f);
	m_pMouthComposite->SetPosition(point(0.0f, -0.35f, HEAD_POS));
	m_pMouthComposite->SetOrientationOffset(vHeadOffset);
	m_pMouthComposite->SetMaterialShininess(2.0f, true);

	//m_pHead = AddModel(util::StringToWideString(strHeadPath));
	
	m_pMouth = m_pMouthComposite->AddModel(util::StringToWideString(k_strMouthPath));
	CN(m_pMouth);
	
	// TODO: should be a part of an avatar folder once there are multiple mouths, 
	// could also help inform a loop with different naming
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_men/mouth_man_04.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_men/mouth_man_03.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_men/mouth_man_02.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_men/mouth_man_01.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_women/mouth_women_04.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_women/mouth_women_03.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_women/mouth_women_02.png"));
	m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, L"mouth_women/mouth_women_01.png"));

	for (int i = 0; i < 8; i++) {
		CN(m_mouthStates[i]);
	}

//	m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_pMouthTexture.get());

#else
	//m_pHead = AddComposite();
	//m_pHead->AddVolume(0.2f);

	m_pHead = AddModel(L"\\cube.obj");
	//m_pHead->SetVertexColor(COLOR_BLUE);
	//m_pHead->SetMaterialDiffuseColor(COLOR_BLUE, true);

#endif



	SetPosition(point(0.0f, 0.0f, 0.0f));

Error:
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
		m_pDreamOS->AddObjectToUIGraph(m_pMouthComposite.get());
	}

	return R_PASS;
}

RESULT user::UpdateAvatarModelWithID(long avatarModelID) {
	RESULT r = R_PASS;

	vector vHeadOffset = vector(0.0f, (float)(M_PI), 0.0f);

	CBM(m_pHead == nullptr, "avatar model already set");
	m_avatarModelId = avatarModelID;
	CR(LoadHeadModelFromID());

	m_pHead->SetScale(0.013f);
	m_pHead->SetOrientationOffset(vHeadOffset);
	m_pHead->SetMaterialShininess(2.0f, true);

	//*
	// women model ids
	if (IsFemaleModel()) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[4].get());
	}
	// men mouths
	else {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[0].get());
	}
	//*/

#ifndef _DEBUG
	// for now the mouth is in a hardcoded position attached to the face model

	m_pLeftHand = AddHand(HAND_TYPE::HAND_LEFT, m_avatarModelId);
//	m_pLeftHand->OnLostTrack();
	m_pLeftHand->SetVisible(true);
	
	m_pRightHand = AddHand(HAND_TYPE::HAND_RIGHT, m_avatarModelId);
//	m_pRightHand->OnLostTrack();
	m_pRightHand->SetVisible(true);

#endif

Error:
	return r;
}

bool user::IsFemaleModel() {
	return m_avatarModelId == 1 || m_avatarModelId == 4;
}

RESULT user::LoadHeadModelFromID() {
	RESULT r = R_PASS;

	//CB(m_avatarModelId != AVATAR_INVALID);

	switch (m_avatarModelId) {

		/*
		case AVATAR_TYPE::WOMAN: {
			m_pHead = AddModel(L"\\Avatar_Woman\\avatar_1.FBX");
		} break;

		case AVATAR_TYPE::BRUCE: {
			m_pHead = AddModel(L"\\Avatar_Bruce\\avatar_2.FBX");
		} break;
		//*/

	case 1: m_pHead = AddModel(L"\\Avatars\\avatar_1.FBX"); break;
	case 2: m_pHead = AddModel(L"\\Avatars\\avatar_2.FBX"); break;
	case 3: m_pHead = AddModel(L"\\Avatars\\avatar_3.FBX"); break;
	case 4: m_pHead = AddModel(L"\\Avatars\\avatar_4.FBX"); break;
	}


Error:
	return r;
}

RESULT user::SetMouthPosition(point ptPosition) {
	m_pMouthComposite->SetPosition(ptPosition);
	return R_PASS;
}

RESULT user::SetMouthOrientation(quaternion qOrientation) {
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
	
	CNR(m_pMouth, R_SKIPPED);

	// Simple IIR filter
	{
		// controls how fast the mouth scale responds to sustained volume
		float newAmount = 0.2f;
		float newMouthScale = mouthScale * 8.0f + 0.01f;

		m_mouthScale = (1.0f - newAmount) * (m_mouthScale) + (newAmount) * (newMouthScale);
	}

	float numBins = (float)(m_numMouthStates);
	int rangedValue = (int)(m_mouthScale * numBins);

	if (rangedValue > 3) {
		rangedValue = 3;
	}

	if (rangedValue < 0) {
		rangedValue = 0;
	}

	if (!IsFemaleModel()) {
		rangedValue += 4;
	}

	if (m_pMouth != nullptr) {
		m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[rangedValue].get());
	}

Error:
	return r;
}