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

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;
	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	vector vHeadOffset;
	if (strHeadPath == "default") {
		vHeadOffset = vector(0.0f, (float)M_PI, 0.0f);
	}
	else {
		vHeadOffset = vector(-(float)(M_PI_2), (float)(M_PI), 0.0f);
	}
	m_pMouthComposite = MakeComposite();
	CN(m_pMouthComposite);

	m_pMouthComposite->SetScale(m_headScale);
	m_pMouthComposite->SetOrientationOffset(vHeadOffset);
	m_pMouthComposite->SetMaterialShininess(2.0f, true);

	//m_pHead = AddModel(util::StringToWideString(strHeadPath));
	

#else
	m_pHead = AddModel(L"\\cube.obj");
#endif

Error:
	return r;
}

std::shared_ptr<composite> user::GetHead() {
	return m_pHead;
}

std::shared_ptr<model> user::GetMouth() {
	return m_pMouth;
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

	color modelColor;
	vector vHeadOffset = vector(0.0f, (float)(M_PI), 0.0f);

#ifndef _DEBUG
	CBM(m_pHead == nullptr, "avatar model already set");
	m_avatarModelId = avatarModelID;
	CR(LoadHeadModelFromID());

	m_pHead->SetScale(m_headScale);
	m_pHead->SetOrientationOffset(vHeadOffset);

	modelColor = m_pHead->GetChildMesh(0)->GetDiffuseColor();
	m_pHead->SetMaterialSpecularColor(modelColor, true);
	m_pHead->SetMaterialShininess(4.0f, true);

	m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[0].get());

	// for now the mouth is in a hardcoded position attached to the face model
	m_pLeftHand = AddHand(HAND_TYPE::HAND_LEFT, m_avatarModelId);
	m_pLeftHand->SetVisible(true);

	m_pRightHand = AddHand(HAND_TYPE::HAND_RIGHT, m_avatarModelId);
	m_pRightHand->SetVisible(true);

#endif

Error:
	return r;
}

RESULT user::LoadHeadModelFromID() {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;
	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	//std::wstring wstrHeadModel = k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + k_wstrAvatarFileType;
	std::wstring wstrHeadModel = wstrAssetPath + k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + L"/head.fbx";
	std::wstring wstrMouthModel = wstrAssetPath + k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + L"/mouth.fbx";

	m_pHead = AddModel(wstrHeadModel);
	CN(m_pHead);

	m_pMouth = m_pMouthComposite->AddModel(wstrMouthModel);
	CN(m_pMouth);
	
	// loop iteration is a quirk of how the files are named
	// with avatar specific textures this will probably be done in LoadHeadModelFromID()
	for (int i = 1; i <= m_numMouthStates; i++) {
		std::wstring wstrMouth = wstrAssetPath + k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + L"/mouth-" + std::to_wstring(i) + k_wstrMouthFileType;
		m_mouthStates.push_back(MakeTexture(texture::type::TEXTURE_2D, &wstrMouth[0]));
	}

	for (int i = 0; i < 4; i++) {
		CN(m_mouthStates[i]);
	}

Error:
	return r;
}

RESULT user::SetMouthPosition(point ptPosition) {
	if (m_pMouthComposite != nullptr)
		m_pMouthComposite->SetPosition(ptPosition);

	return R_PASS;
}

RESULT user::SetMouthOrientation(quaternion qOrientation) {
	if(m_pMouthComposite != nullptr)
		m_pMouthComposite->SetOrientation(qOrientation);

	return R_PASS;
}

RESULT user::UpdateMouthPose() {
	RESULT r = R_PASS;

	CNR(m_pMouth, R_SKIPPED)
	m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[m_currentMouthPose].get());

Error:
	return R_PASS;
}

long user::GetAvatarModelId() {
	return m_avatarModelId;
}

int user::GetCurrentMouthPose() {
	return m_currentMouthPose;
}

//TODO: why doesn't this use hand::SetHandState(pHandState)
RESULT user::UpdateHand(const hand::HandState& pHandState) {
	RESULT r = R_PASS;

	point ptHand = pHandState.ptPalm;
	ptHand = ptHand - GetPosition(true);

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
		float newMouthScale = mouthScale*1.5f;

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

	m_currentMouthPose = rangedValue;

Error:
	return r;
}