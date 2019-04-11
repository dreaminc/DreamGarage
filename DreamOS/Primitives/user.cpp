#include "user.h"
#include "Primitives/quad.h"
#include "Primitives/model/model.h"
#include "Primitives/font.h"

#include "Cloud/HTTP/HTTPController.h"

#include "Core/Utilities.h"
#include "Sandbox/CommandLineManager.h"
#include "DreamOS.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

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

user::~user() {

	RemoveMouth();

	m_pDreamOS->RemoveObjectFromUIGraph(m_pUIObjectComposite.get());
	m_pUIObjectComposite = nullptr;
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

RESULT user::RemoveMouth() {
	RESULT r = R_PASS;

	CNR(m_pDreamOS, R_SKIPPED);
	CNR(m_pMouthComposite, R_SKIPPED);

	m_pDreamOS->RemoveObjectFromUIGraph(m_pMouthComposite.get());
	m_pDreamOS->RemoveObjectFromAuxUIGraph(m_pMouthComposite.get());

	m_pMouthComposite = nullptr;

Error:
	return r;
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

RESULT user::SetDreamOS(DreamOS *pDreamOS) {
	m_pDreamOS = pDreamOS;
	return R_PASS;
}

RESULT user::UpdateAvatarModelWithID(long avatarModelID) {
	RESULT r = R_PASS;

	color modelColor;

#ifndef _DEBUG
	if (m_pHeadModel != nullptr) {
		m_pHead->RemoveChild(m_pHeadModel);
		m_pHeadModel = nullptr;
	}

	m_avatarModelId = avatarModelID;
	CR(LoadHeadModelFromID());


	m_pMouth->GetFirstChild<mesh>()->SetDiffuseTexture(m_mouthStates[0].get());
	m_pDreamOS->AddObjectToUIGraph(m_pMouthComposite.get(), SandboxApp::PipelineType::MAIN);

	// for now the mouth is in a hardcoded position attached to the face model
	m_pLeftHand = AddHand(HAND_TYPE::HAND_LEFT, m_avatarModelId);
	m_pLeftHand->SetVisible(true, false);

	m_pRightHand = AddHand(HAND_TYPE::HAND_RIGHT, m_avatarModelId);
	m_pRightHand->SetVisible(true, false);

#endif

Error:
	return r;
}

RESULT user::OnModelReady(DimObj *pDimObj, void *pContext) {
	RESULT r = R_PASS;

	color modelColor;
	vector vHeadOffset = vector(0.0f, (float)(M_PI), 0.0f);

	model *pObj = dynamic_cast<model*>(pDimObj);
	CN(pObj);

	m_pHeadModel = std::shared_ptr<model>(pObj);
	m_pHead->AddObject(m_pHeadModel);
	m_pHead->SetVisible(true);

	// TODO: broken with async load, not sure why it was important
	//modelColor = m_pHeadModel->GetChildMesh(0)->GetDiffuseColor();
	//m_pHeadModel->SetMaterialSpecularColor(modelColor, true);

	m_pHeadModel->SetMaterialShininess(4.0f, true);

	m_pHeadModel->SetScale(m_headScale);
	m_pHeadModel->SetOrientationOffset(vHeadOffset);

Error:
	return r;
}

RESULT user::LoadHeadModelFromID() {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;
	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	//std::wstring wstrHeadModel = k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + k_wstrAvatarFileType;
	std::wstring wstrHeadModel = L"dreamos:/Assets" + k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + L"/head.fbx";
	std::wstring wstrMouthModel = wstrAssetPath + k_wstrAvatarPath + std::to_wstring(m_avatarModelId) + L"/mouth.fbx";

	m_pHead = AddComposite();
	CN(m_pHead);
	m_pHead->InitializeOBB();

	//m_pHeadModel = AddModel(wstrHeadModel);
	//CN(m_pHeadModel)
	CR(m_pDreamOS->MakeModel(std::bind(&user::OnModelReady, this, std::placeholders::_1, std::placeholders::_2), this, wstrHeadModel));

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
	RESULT r = R_PASS;

	CNR(m_pMouthComposite, R_SKIPPED)

	m_pMouthComposite->SetPosition(ptPosition);

Error:
	return r;
}

RESULT user::SetMouthOrientation(quaternion qOrientation) {
	RESULT r = R_PASS;

	CNR(m_pMouthComposite, R_SKIPPED)

	m_pMouthComposite->SetOrientation(qOrientation);

Error:
	return r;
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

RESULT user::InitializeObject() {
	RESULT r = R_PASS;

	m_pUIObjectComposite = MakeComposite();
	m_pUIObjectComposite->SetPosition(GetPosition(true));
	m_pUIObjectComposite->SetVisible(true, false);
	//m_pDreamOS->AddObjectToUIGraph(m_pUIObjectComposite.get(), SandboxApp::PipelineType::MAIN | SandboxApp::PipelineType::AUX);

	m_pUserLabelComposite = m_pUIObjectComposite->AddComposite();

//	m_pSphere = AddSphere(0.1f,10,10);
//	m_pSphere->SetVisible(false);

	DOSLOG(INFO, "DreamPeerApp object composites created");

Error:
	return r;
}

RESULT user::InitializeUserNameText(std::string strScreenName) {
	RESULT r = R_PASS;

	if (m_pFont == nullptr) {
		m_pFont = m_pDreamOS->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		CN(m_pFont);
		m_pFont->SetLineHeight(0.06f);
	}

	m_pTextUserName = std::shared_ptr<text>(m_pDreamOS->MakeText(
		m_pFont,
		strScreenName,
		0.4,
		LABEL_HEIGHT,
		text::flags::FIT_TO_SIZE | text::flags::RENDER_QUAD));

	CN(m_pTextUserName);

	m_pTextUserName->SetVisible(false);

	m_pTextUserName->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));

	CR(m_pUserLabelComposite->AddObject(m_pTextUserName));
	m_pTextUserName->SetMaterialDiffuseColor(m_hiddenColor);

	DOSLOG(INFO, "DreamPeerApp text object created");

Error:
	return r;
}

RESULT user::InitializeUserNameLabel() {
	RESULT r = R_PASS;

	vector vCameraDirection;
	vCameraDirection = GetPosition(true) - m_pDreamOS->GetCamera()->GetPosition(true);
	vCameraDirection = vector(vCameraDirection.x(), 0.0f, vCameraDirection.z()).Normal();

	float hasPhoto = HasProfilePhoto() ? LABEL_PHOTO_WIDTH : 0;
	// TODO: switch on profile picture
	float totalWidth = m_pTextUserName->GetWidth() + hasPhoto + LABEL_GAP_WIDTH * 2.0f;
	float photoX = -totalWidth/2.0f + LABEL_PHOTO_WIDTH / 2.0f;
	float leftGapX = -totalWidth/2.0f + hasPhoto + LABEL_GAP_WIDTH / 2.0f;
	float textboxX = -totalWidth / 2.0f + hasPhoto + LABEL_GAP_WIDTH + m_pTextUserName->GetWidth() / 2.0f;
	float rightGapX = totalWidth / 2.0f - LABEL_GAP_WIDTH / 2.0f;

	float backgroundDepth = -0.005f;

	PathManager *pPathManager = PathManager::instance();
	std::wstring wstrAssetPath;

	pPathManager->GetValuePath(PATH_ASSET, wstrAssetPath);

	std::wstring wstrSeatPosition = util::StringToWideString(std::to_string(m_seatingPosition));

	CN(m_pUserLabelComposite);
	m_pUserLabelComposite->SetVisible(true);
	m_pUserLabelComposite->SetMaterialDiffuseColor(m_backgroundColor);

	if (HasProfilePhoto()) {

		CR(PendProfilePhotoDownload());
		m_pPhotoQuad = m_pUserLabelComposite->AddQuad(LABEL_PHOTO_WIDTH, LABEL_HEIGHT);
		CN(m_pPhotoQuad);
		m_pPhotoQuad->SetPosition(point(photoX, NAMETAG_HEIGHT, backgroundDepth));
		m_pPhotoQuad->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrPhoto)[0]));
		m_pPhotoQuad->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));
		m_pPhotoQuad->SetMaterialDiffuseColor(m_visibleColor);
	}

	m_pLeftGap = m_pUserLabelComposite->AddQuad(LABEL_GAP_WIDTH, LABEL_HEIGHT);
	CN(m_pLeftGap);
	m_pLeftGap->SetPosition(point(leftGapX, NAMETAG_HEIGHT, backgroundDepth));

	if (HasProfilePhoto()) {
		m_pLeftGap->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrLeft + wstrSeatPosition + L".png")[0]));
	} 
	else {
		m_pLeftGap->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrLeftEmpty + wstrSeatPosition + L".png")[0]));
	}

	m_pLeftGap->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pLeftGap->SetMaterialDiffuseColor(m_backgroundColor);

	m_pNameBackground = m_pUserLabelComposite->AddQuad(m_pTextUserName->GetWidth(), LABEL_HEIGHT);
	CN(m_pNameBackground);

	m_pTextBoxTexture = MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrMiddle)[0]);
	m_pNameBackground->SetPosition(point(textboxX, NAMETAG_HEIGHT, backgroundDepth));
	m_pNameBackground->SetDiffuseTexture(m_pTextBoxTexture.get());
	m_pNameBackground->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pNameBackground->SetMaterialDiffuseColor(m_backgroundColor);

	m_pTextUserName->SetPosition(point(textboxX, NAMETAG_HEIGHT-0.005f, 0.0f), text::VerticalAlignment::MIDDLE, text::HorizontalAlignment::CENTER);

	m_pRightGap = m_pUserLabelComposite->AddQuad(LABEL_GAP_WIDTH, LABEL_HEIGHT);
	CN(m_pRightGap);
	m_pRightGap->SetPosition(point(rightGapX, NAMETAG_HEIGHT, backgroundDepth));
	m_pRightGap->SetDiffuseTexture(m_pDreamOS->MakeTexture(texture::type::TEXTURE_2D, &(wstrAssetPath + k_wstrRight)[0]));
	m_pRightGap->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pRightGap->SetMaterialDiffuseColor(m_backgroundColor);

	if (m_pDreamOS->GetSandboxConfiguration().f3rdPersonCamera) {
		m_pUserLabelComposite->SetVisible(false);
	}
	else {
		m_pUserLabelComposite->SetVisible(true);
	}

	DOSLOG(INFO, "DreamPeerApp username label created");

Error:
	return r;

}

RESULT user::UpdateUserNameLabelPlacement(camera *pCamera) {
	RESULT r = R_PASS;

	point ptSeatPosition = m_pUIObjectComposite->GetPosition(true);
	vector vCameraDirection;

	vCameraDirection = ptSeatPosition - pCamera->GetPosition(true);
	vCameraDirection = vector(vCameraDirection.x(), 0.0f, vCameraDirection.z()).Normal();

	// Making a quaternion with two vectors uses cross product,
	// vector(0,0,1) and vector(0,0,-1) are incompatible with vector(0,0,-1)
	if (vCameraDirection == vector::kVector(1.0f)) {
		SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, (float)M_PI, 0.0f));
	}
	else if (vCameraDirection == vector::kVector(-1.0f)) {
		SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, 0.0f));
	}
	else {
		SetUserLabelOrientation(quaternion(vector::kVector(-1.0f), vCameraDirection));
	}

	auto pHead = GetHead();

	BoundingBox* pOuterBoundingVolume = dynamic_cast<BoundingBox*>(pHead->GetBoundingVolume().get());
	//CN(pOuterBoundingVolume);

	float outerDistance = pOuterBoundingVolume->GetFarthestPointInDirection(vector(0.0f, 1.0f, 0.0f)).y();

	// TODO: test pOuter->GetO * pName->GetO
	quaternion qNameComposite = m_pUIObjectComposite->GetOrientation();
	qNameComposite.Reverse();
	qNameComposite = qNameComposite * pOuterBoundingVolume->GetOrientation(true);

	point ptOrigin = RotationMatrix(qNameComposite) * ScalingMatrix(pOuterBoundingVolume->GetScale(false)) * vector(pOuterBoundingVolume->GetCenter());
	ptOrigin += pHead->GetOrigin();

	m_pUserLabelComposite->SetPosition(point(ptOrigin.x(), outerDistance, ptOrigin.z()));
	m_pUserLabelComposite->SetVisible(true);
	//m_pSphere->SetPosition(point(ptOrigin.x(), outerDistance, ptOrigin.z()));

	UpdateMouthPose();

	if (m_pPendingPhotoTextureBuffer != nullptr) {
		CR(UpdateProfilePhoto());
	}


Error:
	return r;
}

RESULT user::ShowUserNameField() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		m_pUIObjectComposite->SetVisible(true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {

		return R_PASS;
	};	

	m_pUserLabelComposite->SetVisible(true);
	m_pUIObjectComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), m_pDreamOS->GetCamera()->GetLookVectorXZ()));
	m_pUIObjectComposite->SetVisible(true);

	/*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pUserLabelComposite.get(),
		m_backgroundColor,
		m_userNameAnimationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/

Error:
	return r;
}

RESULT user::HideUserNameField() {
	RESULT r = R_PASS;
	
	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		m_pUIObjectComposite->SetVisible(false);
		return R_PASS;
	};

	/*
	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_hiddenColor,
		m_userNameAnimationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	CR(m_pDreamOS->GetInteractionEngineProxy()->PushAnimationItem(
		m_pUserLabelComposite.get(),
		m_hiddenColor,
		m_userNameAnimationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
Error:
	return r;
}

RESULT user::SetUserLabelPosition(point ptPosition) {
	RESULT r = R_PASS;

	m_pUIObjectComposite->SetPosition(ptPosition);
	//m_pSphere->SetVisible(true);

	return r;
}

RESULT user::SetUserLabelOrientation(quaternion qOrientation) {
	RESULT r = R_PASS;

	m_pUIObjectComposite->SetOrientation(qOrientation);

	return r;
}

bool user::HasProfilePhoto() {
	return m_strProfilePhotoURL != "";
}

RESULT user::SetProfilePhoto(std::string strProfilePhoto) {
	m_strProfilePhotoURL = strProfilePhoto;
	return R_PASS;
}

RESULT user::SetScreenName(std::string strScreenName) {
	m_strScreenName = strScreenName;
	return R_PASS;
}

RESULT user::SetInitials(std::string strInitials) {
	m_strInitials = strInitials;
	return R_PASS;
}

std::string user::GetInitials() {
	return m_strInitials;
}

RESULT user::SetSeatingPosition(int seatingPosition) {
	m_seatingPosition = seatingPosition;
	return R_PASS;
}

int user::GetSeatingPosition() {
	return m_seatingPosition;
}

RESULT user::PendProfilePhotoDownload() {
	RESULT r = R_PASS;

	std::string strAuthorizationToken;

	auto pUserControllerProxy = (UserControllerProxy*)m_pDreamOS->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
	auto pHTTPControllerProxy = (HTTPControllerProxy*)m_pDreamOS->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
	//auto strHeaders = HTTPController::ContentHttp();

	CN(pUserControllerProxy);
	CN(pHTTPControllerProxy);

	CR(pHTTPControllerProxy->RequestFile(m_strProfilePhotoURL, std::vector<std::string>(), "", std::bind(&user::OnProfilePhotoDownload, this, std::placeholders::_1, std::placeholders::_2), nullptr));

Error:
	return r;
}

RESULT user::OnProfilePhotoDownload(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext) {
	RESULT r = R_PASS;

	CN(pBufferVector);
	m_pPendingPhotoTextureBuffer = pBufferVector;

Error:
	return r;
}

RESULT user::UpdateProfilePhoto() {
	RESULT r = R_PASS;

	texture *pTexture = nullptr;

	CN(m_pPendingPhotoTextureBuffer);
	uint8_t* pBuffer = &(m_pPendingPhotoTextureBuffer->operator[](0));
	size_t pBuffer_n = m_pPendingPhotoTextureBuffer->size();

	pTexture = m_pDreamOS->MakeTextureFromFileBuffer(texture::type::TEXTURE_2D, pBuffer, pBuffer_n);
	CN(pTexture);

	m_pPhotoQuad->SetDiffuseTexture(pTexture);

Error:
	if (m_pPendingPhotoTextureBuffer != nullptr) {
		m_pPendingPhotoTextureBuffer = nullptr;
	}
	return r;
}

std::shared_ptr<composite> user::GetUserLabelComposite() {
	return m_pUserLabelComposite;
}

std::shared_ptr<composite> user::GetUserObjectComposite() {
	return m_pUIObjectComposite;
}

bool user::IsUserNameVisible() {
	if (m_pTextUserName != nullptr && m_pNameBackground != nullptr) {
		return m_pTextUserName->IsVisible() && m_pNameBackground->IsVisible();
	}
	else {
		return false;
	}
}

RESULT user::Update() {
	RESULT r = R_PASS;

	if (m_pUIObjectComposite != nullptr && m_pNameBackground == nullptr && m_pTextUserName != nullptr && !m_pTextUserName->IsDirty() && m_seatingPosition != -1) {
		CR(InitializeUserNameLabel());
	}

	if (m_pUIObjectComposite != nullptr && m_pTextUserName == nullptr && m_strScreenName != "") {
		CR(InitializeUserNameText(m_strScreenName));
	}

	{
		auto pHead = GetHead();

		BoundingBox* pOuterBoundingVolume = dynamic_cast<BoundingBox*>(pHead->GetBoundingVolume().get());
		CN(pOuterBoundingVolume);

		float outerDistance = pOuterBoundingVolume->GetFarthestPointInDirection(vector(0.0f, 1.0f, 0.0f)).y();

		// TODO: test pOuter->GetO * pName->GetO
		quaternion qNameComposite = m_pUIObjectComposite->GetOrientation();
		qNameComposite.Reverse();
		qNameComposite = qNameComposite * pOuterBoundingVolume->GetOrientation(true);

		point ptOrigin = RotationMatrix(qNameComposite) * ScalingMatrix(pOuterBoundingVolume->GetScale(false)) * vector(pOuterBoundingVolume->GetCenter());
		ptOrigin += pHead->GetOrigin();

		m_pUserLabelComposite->SetPosition(point(ptOrigin.x(), outerDistance, ptOrigin.z()));
		m_pUserLabelComposite->SetVisible(true);
		//m_pSphere->SetPosition(point(ptOrigin.x(), outerDistance, ptOrigin.z()));

		UpdateMouthPose();

//		UpdateUserNameLabelPlacement();

		if (m_pPendingPhotoTextureBuffer != nullptr) {
			CR(UpdateProfilePhoto());
		}
	}

Error:
	return r;
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

	pHand->Update();

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