#include "DreamPeerApp.h"
#include "DreamOS.h"

#include "InteractionEngine/AnimationCurve.h"
#include "InteractionEngine/AnimationItem.h"

#include "Primitives/font.h"

#include "Cloud/HTTP/HTTPController.h"
#include "Cloud/Environment/PeerConnection.h"
#include "UI/UIView.h"

#include "Sound/SpatialSoundObject.h"
#include "DreamGarage/AudioDataMessage.h"

#include "Core/Utilities.h"

DreamPeerApp::DreamPeerApp(DreamOS *pDOS, void *pContext) :
	DreamApp<DreamPeerApp>(pDOS, pContext),
	m_pDOS(pDOS),
	m_state(DreamPeerApp::state::UNINITIALIZED)
	//m_pPeerConnection(pPeerConnection)
{
	// empty
}

DreamPeerApp* DreamPeerApp::SelfConstruct(DreamOS *pDreamOS, void *pContext) {
	DreamPeerApp *pDreamApp = new DreamPeerApp(pDreamOS, pContext);
	return pDreamApp;
}

RESULT DreamPeerApp::InitializeApp(void *pContext) {
	RESULT r = R_PASS;

	CR(SetState(state::INITIALIZED));

	SetAppName("DreamPeerApp");
	SetAppDescription("A Dream User App");


	m_pUIObjectComposite = GetComposite()->MakeComposite();
	m_pUIObjectComposite->SetPosition(GetComposite()->GetPosition(true));
	GetDOS()->AddObjectToUIGraph(m_pUIObjectComposite.get(), (SandboxApp::PipelineType::AUX | SandboxApp::PipelineType::MAIN));

	m_pUserLabelComposite = m_pUIObjectComposite->MakeComposite();
	m_pUserLabelComposite = m_pUIObjectComposite->AddComposite();

	DOSLOG(INFO, "DreamPeerApp object composites created");

Error:
	return r;
}

RESULT DreamPeerApp::OnAppDidFinishInitializing(void *pContext) {
	RESULT r = R_PASS;

	CR(r);

Error:
	return r;
}

RESULT DreamPeerApp::Shutdown(void *pContext) {
	RESULT r = R_PASS;

	m_pUserModel->RemoveMouth();
	m_pUserModel = nullptr;

	GetDOS()->RemoveObjectFromUIGraph(m_pUIObjectComposite.get());
	m_pUIObjectComposite = nullptr;


Error:
	return r;
}

RESULT DreamPeerApp::Update(void *pContext) {
	RESULT r = R_PASS;

	// If pending user mode - add to composite here
	if (m_fPendingAssignedUserModel) {

		CN(m_pUserModel);
		CR(GetComposite()->AddObject(m_pUserModel));
		m_fPendingAssignedUserModel = false;
	}

	if (m_pSpatialSoundObject == nullptr) {
		m_pSpatialSoundObject = GetDOS()->AddSpatialSoundObject(point(), vector(), vector());
		CN(m_pSpatialSoundObject);
	}

	if (m_pNameBackground == nullptr && m_pTextUserName != nullptr && !m_pTextUserName->IsDirty()) {
		CR(InitializeUserNameLabel());
	}

	if (m_pTextUserName == nullptr && m_strScreenName != "") {
		CR(InitializeUserNameText());
	}
	
	
	// update user label position
	if (m_pUserModel != nullptr) {

		auto pHead = m_pUserModel->GetHead();

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

		m_pUserModel->UpdateMouthPose();
	}

	if (m_pPendingPhotoTextureBuffer != nullptr) {
		CR(UpdateProfilePhoto());
	}
	
Error:
	return r;
}

RESULT DreamPeerApp::InitializeUserNameText() {
	RESULT r = R_PASS;

	if (m_pFont == nullptr) {
		m_pFont = GetDOS()->MakeFont(L"Basis_Grotesque_Pro.fnt", true);
		CN(m_pFont);
		m_pFont->SetLineHeight(0.06f);
	}

	m_pTextUserName = std::shared_ptr<text>(GetDOS()->MakeText(
		m_pFont,
		m_strScreenName,
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

RESULT DreamPeerApp::InitializeUserNameLabel() {
	RESULT r = R_PASS;

	vector vCameraDirection;


	vCameraDirection = GetComposite()->GetPosition(true) - GetDOS()->GetCamera()->GetPosition(true);
	vCameraDirection = vector(vCameraDirection.x(), 0.0f, vCameraDirection.z()).Normal();

	float hasPhoto = HasProfilePhoto() ? LABEL_PHOTO_WIDTH : 0;
	// TODO: switch on profile picture
	float totalWidth = m_pTextUserName->GetWidth() + hasPhoto + LABEL_GAP_WIDTH * 2.0f;
	float photoX = -totalWidth/2.0f + LABEL_PHOTO_WIDTH / 2.0f;
	float leftGapX = -totalWidth/2.0f + hasPhoto + LABEL_GAP_WIDTH / 2.0f;
	float textboxX = -totalWidth / 2.0f + hasPhoto + LABEL_GAP_WIDTH + m_pTextUserName->GetWidth() / 2.0f;
	float rightGapX = totalWidth / 2.0f - LABEL_GAP_WIDTH / 2.0f;

	float backgroundDepth = -0.005f;

	CN(m_pUserLabelComposite);
	m_pUserLabelComposite->SetVisible(true);
	m_pUserLabelComposite->SetMaterialDiffuseColor(m_backgroundColor);

	if (HasProfilePhoto()) {

		CR(PendProfilePhotoDownload());
		m_pPhotoQuad = m_pUserLabelComposite->AddQuad(LABEL_PHOTO_WIDTH, LABEL_HEIGHT);
		CN(m_pPhotoQuad);
		m_pPhotoQuad->SetPosition(point(photoX, NAMETAG_HEIGHT, backgroundDepth));
		m_pPhotoQuad->SetDiffuseTexture(m_pDOS->MakeTexture(texture::type::TEXTURE_2D, &k_wstrPhoto[0]));
		m_pPhotoQuad->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));
		m_pPhotoQuad->SetMaterialDiffuseColor(m_visibleColor);
	}

	m_pLeftGap = m_pUserLabelComposite->AddQuad(LABEL_GAP_WIDTH, LABEL_HEIGHT);
	CN(m_pLeftGap);
	m_pLeftGap->SetPosition(point(leftGapX, NAMETAG_HEIGHT, backgroundDepth));

	if (HasProfilePhoto()) {
		m_pLeftGap->SetDiffuseTexture(m_pDOS->MakeTexture(texture::type::TEXTURE_2D, &k_wstrLeft[0]));
	} 
	else {
		m_pLeftGap->SetDiffuseTexture(m_pDOS->MakeTexture(texture::type::TEXTURE_2D, &k_wstrLeftEmpty[0]));
	}

	m_pLeftGap->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pLeftGap->SetMaterialDiffuseColor(m_backgroundColor);

	m_pNameBackground = m_pUserLabelComposite->AddQuad(m_pTextUserName->GetWidth(), LABEL_HEIGHT);
	CN(m_pNameBackground);

	m_pTextBoxTexture = GetComposite()->MakeTexture(texture::type::TEXTURE_2D, &k_wstrMiddle[0]);
	m_pNameBackground->SetPosition(point(textboxX, NAMETAG_HEIGHT, backgroundDepth));
	m_pNameBackground->SetDiffuseTexture(m_pTextBoxTexture.get());
	m_pNameBackground->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pNameBackground->SetMaterialDiffuseColor(m_backgroundColor);

	m_pTextUserName->SetPosition(point(textboxX, NAMETAG_HEIGHT-0.005f, 0.0f), text::VerticalAlignment::MIDDLE, text::HorizontalAlignment::CENTER);

	m_pRightGap = m_pUserLabelComposite->AddQuad(LABEL_GAP_WIDTH, LABEL_HEIGHT);
	CN(m_pRightGap);
	m_pRightGap->SetPosition(point(rightGapX, NAMETAG_HEIGHT, backgroundDepth));
	m_pRightGap->SetDiffuseTexture(m_pDOS->MakeTexture(texture::type::TEXTURE_2D, &k_wstrRight[0]));
	m_pRightGap->SetOrientation(quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));	
	m_pRightGap->SetMaterialDiffuseColor(m_backgroundColor);

	if (GetDOS()->GetSandboxConfiguration().f3rdPersonCamera) {
		m_pUserLabelComposite->SetVisible(false);
	}
	else {
		m_pUserLabelComposite->SetVisible(true);
	}

	DOSLOG(INFO, "DreamPeerApp username label created");

Error:
	return r;
}

RESULT DreamPeerApp::SetUsernameAnimationDuration(float animationDuration) {
	m_userNameAnimationDuration = animationDuration;
	return R_PASS;
}

std::shared_ptr<composite> DreamPeerApp::GetUserLabelComposite() {
	return m_pUIObjectComposite;
}

RESULT DreamPeerApp::SetUserLabelPosition(point ptPosition) {
	RESULT r = R_PASS;

	m_pUIObjectComposite->SetPosition(ptPosition);

	return r;
}

RESULT DreamPeerApp::SetUserLabelOrientation(quaternion qOrientation) {
	RESULT r = R_PASS;

	m_pUIObjectComposite->SetOrientation(qOrientation);

	return r;
}

bool DreamPeerApp::HasProfilePhoto() {
	return m_strProfilePhotoURL != "";
}

RESULT DreamPeerApp::HideUserNameField() {
	RESULT r = R_PASS;
	
	auto fnStartCallback = [&](void *pContext) {
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {
		m_pUIObjectComposite->SetVisible(false);
		return R_PASS;
	};
	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_hiddenColor,
		0.5,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
	///*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_hiddenColor,
		m_userNameAnimationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
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

RESULT DreamPeerApp::ClearAssets() {
	RESULT r = R_PASS;

	return r;
}

RESULT DreamPeerApp::ShowUserNameField() {
	RESULT r = R_PASS;

	auto fnStartCallback = [&](void *pContext) {
		m_pUIObjectComposite->SetVisible(true);
		return R_PASS;
	};

	auto fnEndCallback = [&](void *pContext) {

		return R_PASS;
	};	

	m_pUserLabelComposite->SetVisible(true);
	m_pUIObjectComposite->SetOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), GetDOS()->GetCamera()->GetLookVectorXZ()));
	//* quaternion::MakeQuaternionWithEuler(vector((90 * (float)M_PI) / 180, 0.0f, 0.0f)));
	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pNameComposite.get(),
		m_visibleColor,
		0.5,
		AnimationCurveType::LINEAR,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));
	//*/
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pUserLabelComposite.get(),
		m_backgroundColor,
		m_userNameAnimationDuration,
		AnimationCurveType::SIGMOID,
		AnimationFlags(),
		fnStartCallback,
		fnEndCallback,
		this
	));

	/*
	CR(GetDOS()->GetInteractionEngineProxy()->PushAnimationItem(
		m_pTextUserName.get(),
		m_visibleColor,
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

RESULT DreamPeerApp::RegisterDreamPeerObserver(DreamPeerAppObserver* pDreamPeerObserver) {
	RESULT r = R_PASS;

	CNM((pDreamPeerObserver), "Observer cannot be nullptr");
	CBM((m_pDreamPeerObserver == nullptr), "Can't overwrite dream peer observer");
	m_pDreamPeerObserver = pDreamPeerObserver;

Error:
	return r;
}

DreamPeerApp::state DreamPeerApp::GetState() {
	return m_state;
}

bool DreamPeerApp::IsHandshakeRequestHung() {
	if (m_peerConnectionState.fSentHandshakeRequest == true && m_peerConnectionState.fReceivedHandshakeAck == false) {
		return true;
	}

	return false;
}

bool DreamPeerApp::IsHandshakeRequestAckHung() {
	if (m_peerConnectionState.fReceivedHandshakeAck == true && m_peerConnectionState.fSentHandshakeRequestACK == false) {
		return true;
	}

	return false;
}

RESULT DreamPeerApp::SetState(DreamPeerApp::state peerState) {
	RESULT r = R_PASS;

	// TODO: Check for incorrect state changes?

	if (peerState != m_state) {
		
		m_state = peerState;

		if (m_pDreamPeerObserver != nullptr) {
			CR(m_pDreamPeerObserver->OnDreamPeerStateChange(this));
		}
	}

Error:
	return r;
}

long DreamPeerApp::GetPeerUserID() {
	return m_peerUserID;
}

PeerConnection* DreamPeerApp::GetPeerConnection() {
	return m_pPeerConnection;
}

RESULT DreamPeerApp::SetPeerConnection(PeerConnection *pPeerConnection) {
	RESULT r = R_PASS;

	CN(pPeerConnection);

	m_pPeerConnection = pPeerConnection;
	m_peerUserID = m_pPeerConnection->GetPeerUserID();	
	
	UserController* pUserController = dynamic_cast<UserController*>(GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

	// TODO: async version?
	pUserController->GetPeerProfile(m_peerUserID);

	m_strScreenName = pUserController->GetPeerScreenName(m_peerUserID);
	m_avatarModelId = pUserController->GetPeerAvatarModelID(m_peerUserID);
	m_strProfilePhotoURL = pUserController->GetPeerProfilePhotoURL(m_peerUserID);

	DOSLOG(INFO, "SetPeerConnection: %s %d", m_strScreenName, m_avatarModelId);

Error:
	return r;
}

RESULT DreamPeerApp::PendProfilePhotoDownload() {
	RESULT r = R_PASS;

	std::string strAuthorizationToken;

	auto pUserControllerProxy = (UserControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::USER);
	auto pHTTPControllerProxy = (HTTPControllerProxy*)GetDOS()->GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE::HTTP);
	//auto strHeaders = HTTPController::ContentHttp();

	CN(pUserControllerProxy);
	CN(pHTTPControllerProxy);

	CR(pHTTPControllerProxy->RequestFile(m_strProfilePhotoURL, std::vector<std::string>(), "", std::bind(&DreamPeerApp::OnProfilePhotoDownload, this, std::placeholders::_1, std::placeholders::_2), nullptr));

Error:
	return r;
}

RESULT DreamPeerApp::OnProfilePhotoDownload(std::shared_ptr<std::vector<uint8_t>> pBufferVector, void* pContext) {
	RESULT r = R_PASS;

	CN(pBufferVector);
	m_pPendingPhotoTextureBuffer = pBufferVector;

Error:
	return r;
}

RESULT DreamPeerApp::UpdateProfilePhoto() {
	RESULT r = R_PASS;

	texture *pTexture = nullptr;

	CN(m_pPendingPhotoTextureBuffer);
	uint8_t* pBuffer = &(m_pPendingPhotoTextureBuffer->operator[](0));
	size_t pBuffer_n = m_pPendingPhotoTextureBuffer->size();

	pTexture = GetDOS()->MakeTextureFromFileBuffer(texture::type::TEXTURE_2D, pBuffer, pBuffer_n);
	CN(pTexture);

	m_pPhotoQuad->SetDiffuseTexture(pTexture);

Error:
	if (m_pPendingPhotoTextureBuffer != nullptr) {
		m_pPendingPhotoTextureBuffer = nullptr;
	}
	return r;
}

std::shared_ptr<user> DreamPeerApp::GetUserModel() {
	return m_pUserModel;
}

RESULT DreamPeerApp::AssignUserModel(user* pUserModel) {
	RESULT r = R_PASS;

	CN(pUserModel);
	m_pUserModel = std::shared_ptr<user>(pUserModel);
	m_pUserModel->SetVisible(m_fVisible);
	m_pUserModel->SetDreamOS(GetDOS());

	m_fPendingAssignedUserModel = true;
	CR(m_pUserModel->UpdateAvatarModelWithID(m_avatarModelId));

	//m_pUserLabelComposite->SetVisible(true);
	m_pUIObjectComposite->SetVisible(true);
	//CR(ShowUserNameField());

Error:
	return r;
}

// TODO: We should create a proper object pool design
RESULT DreamPeerApp::ReleaseUserModel() {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel = nullptr;

Error:
	return r;
}

RESULT DreamPeerApp::SetVisible(bool fVisible) {
	RESULT r = R_PASS;
	std::shared_ptr<hand> pHand = nullptr;

	m_fVisible = fVisible;

	CN(m_pUserModel);
	CR(m_pUserModel->SetVisible(fVisible));

	if (m_pUserModel != nullptr && m_pUserModel->GetMouth() != nullptr) {
		CR(m_pUserModel->GetMouth()->SetVisible(fVisible));
	}
	
	if (m_pUIObjectComposite != nullptr) {
		m_pUIObjectComposite->SetVisible(fVisible, false);
	}

	pHand = m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT);
	CN(pHand);
	pHand->SetVisible(fVisible, false);

	pHand = m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT);
	CN(pHand);
	pHand->SetVisible(fVisible, false);

Error:
	return r;
}

bool DreamPeerApp::IsVisible() {
	return m_fVisible;
}

bool DreamPeerApp::IsUserNameVisible() {
	if (m_pTextUserName != nullptr && m_pNameBackground != nullptr) {
		return m_pTextUserName->IsVisible() && m_pNameBackground->IsVisible();
	}
	else {
		return false;
	}
}

RESULT DreamPeerApp::SetPosition(const point& ptPosition) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	//m_pUserModel->SetPosition(ptPosition);
	GetComposite()->SetPosition(ptPosition);
	m_pUserModel->SetMouthPosition(ptPosition);
	SetUserLabelPosition(ptPosition);

	if (m_pSpatialSoundObject != nullptr) {
		m_pSpatialSoundObject->SetPosition(ptPosition);
	}

Error:
	return r;
}

RESULT DreamPeerApp::SetOrientation(const quaternion& qOrientation) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	//m_pUserModel->SetOrientation(qOrientation);
	m_pUserModel->GetHead()->SetOrientation(qOrientation);
	m_pUserModel->SetMouthOrientation(qOrientation);

	if (m_pSpatialSoundObject != nullptr) {
		m_pSpatialSoundObject->SetOrientation(qOrientation);
	}

Error:
	return r;
}

RESULT DreamPeerApp::RotateByDeg(float degX, float degY, float degZ) {
	RESULT r = R_PASS;

	CNR(m_pUserModel, R_SKIPPED);
	//m_pUserModel->RotateByDeg(degX, degY, degZ);
	m_pUserModel->GetHead()->RotateByDeg(degX, degY, degZ);
	//GetComposite()->RotateByDeg(degX, degY, degZ);

Error:
	return r;
}

RESULT DreamPeerApp::HandleUserAudioDataMessage(AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	/*
	// Audio is now being played by the flat mono channels up in DreamGarage
	int16_t *pAudioDataBuffer = (int16_t*)(pAudioDataMessage->GetAudioMessageBuffer());
	CN(pAudioDataBuffer);

	size_t numFrames = pAudioDataMessage->GetNumFrames();

	// TODO: Handle channels?
	size_t channels = pAudioDataMessage->GetNumChannels();

	// Play
	if (m_pSpatialSoundObject != nullptr) {

		// Not sure if we need to allocate new memory here or not

		int16_t *pInt16Soundbuffer = new int16_t[numFrames];
		memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * numFrames);

		if (pInt16Soundbuffer != nullptr) {
			CR(m_pSpatialSoundObject->PushMonoAudioBuffer((int)numFrames, pInt16Soundbuffer));
		}
	}
	*/

	// Mouth Position
	//*
	{
		auto pAudioBuffer = pAudioDataMessage->GetAudioMessageBuffer();
		CN(pAudioBuffer);

		size_t numSamples = pAudioDataMessage->GetNumChannels() * pAudioDataMessage->GetNumFrames();
		long averageAccumulator = 0.0f;

		for (int i = 0; i < numSamples; ++i) {
			int16_t value = *(static_cast<const int16_t*>(pAudioBuffer) + i);
			//float scaledValue = (float)(value) / (std::numeric_limits<int16_t>::max());

			averageAccumulator += std::abs(value);
		}

		float mouthScale = ((float)((float)averageAccumulator / (float)numSamples)) / ((float)(std::numeric_limits<int16_t>::max()));
		mouthScale *= 10.0f;

		util::Clamp<float>(mouthScale, 0.0f, 1.0f);
		UpdateMouth(mouthScale);
	}
	//*/

Error:
	return r;
}

RESULT DreamPeerApp::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;

	CNR(m_pUserModel, R_SKIPPED);

	m_pUserModel->UpdateMouth(mouthScale);

Error:
	return r;
}

RESULT DreamPeerApp::UpdateHand(const hand::HandState& handState) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->UpdateHand(handState);

Error:
	return r;
}

WebRTCPeerConnectionProxy* DreamPeerApp::GetWebRTCPeerConnectionProxy() {
	RESULT r = R_PASS;

	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy = nullptr;

	CN(m_pDOS);

	pWebRTCPeerConnectionProxy = m_pDOS->GetWebRTCPeerConnectionProxy(m_pPeerConnection);

	return pWebRTCPeerConnectionProxy;
Error:
	return nullptr;
}

bool DreamPeerApp::IsPeerReady() {
	return m_peerConnectionState.fReceivedAndRespondedToHandshake;
}

RESULT DreamPeerApp::OnDataChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fDataChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

bool DreamPeerApp::IsDataChannel() {
	return m_peerConnectionState.fDataChannel;
}

RESULT DreamPeerApp::OnAudioChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fAudioChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::UpdatePeerHandshakeState() {
	RESULT r = R_PASS;

	if (m_peerConnectionState.fReceivedHandshakeAck && 
		m_peerConnectionState.fSentHandshakeRequestACK &&
		m_peerConnectionState.fAudioChannel && 
		m_peerConnectionState.fDataChannel
		) 
	{
		m_peerConnectionState.fReceivedAndRespondedToHandshake = true;
		CR(SetState(state::ESTABLISHED));
	}
	else {
		if (m_peerConnectionState.fReceivedHandshakeAck ||
			m_peerConnectionState.fSentHandshakeRequestACK)
		{
			CR(SetState(state::PENDING));
		}

		m_peerConnectionState.fReceivedAndRespondedToHandshake = false;
	}

Error:
	return r;
}

RESULT DreamPeerApp::SentHandshakeRequest() {
	RESULT r = R_PASS;

//	CB((m_peerConnectionState.fSentHandshakeRequest == false));
	m_peerConnectionState.fSentHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::ReceivedHandshakeACK() {
	RESULT r = R_PASS;

//	CB((m_peerConnectionState.fSentHandshakeRequest == true));

//	m_peerConnectionState.fSentHandshakeRequest = false;
	m_peerConnectionState.fReceivedHandshakeAck = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::ReceivedHandshakeRequest() {
	RESULT r = R_PASS;

	m_peerConnectionState.fReceivedHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeerApp::SentHandshakeACK() {
	RESULT r = R_PASS;

	//m_peerConnectionState.fReceivedHandshakeRequest = false;
	m_peerConnectionState.fSentHandshakeRequestACK = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

