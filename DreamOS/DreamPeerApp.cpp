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
#include "Primitives/camera.h"

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

	if (m_pUserModel != nullptr) {
		m_pUserModel->RemoveMouth();

		GetDOS()->RemoveObjectFromUIGraph(m_pUserModel->GetUserObjectComposite().get());
	}

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

	if (m_pUserModel != nullptr) {
		m_pUserModel->Update();
		m_pUserModel->UpdateUserNameLabelPlacement(GetDOS()->GetCamera());

		// 
		m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT)->GetPhantomModel()->SetVisible(false);
		m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT)->GetPhantomModel()->SetVisible(false);
	}

Error:
	return r;
}

std::shared_ptr<composite> DreamPeerApp::GetUserLabelComposite() {
	return m_pUserModel->GetUserLabelComposite();
}

RESULT DreamPeerApp::SetUserLabelPosition(point ptPosition) {
	RESULT r = R_PASS;

	m_pUserModel->SetUserLabelPosition(ptPosition);

	return r;
}

RESULT DreamPeerApp::SetUserLabelOrientation(quaternion qOrientation) {
	RESULT r = R_PASS;

	m_pUserModel->SetUserLabelOrientation(qOrientation);

	return r;
}

RESULT DreamPeerApp::UpdateLabelOrientation(camera *pCamera) {
	RESULT r = R_PASS;

	CNR(m_pUserModel, R_SKIPPED);
	CR(m_pUserModel->UpdateUserNameLabelPlacement(pCamera));

Error:
	return r;
}

RESULT DreamPeerApp::HideUserNameField() {
	RESULT r = R_PASS;
	
	CNR(m_pUserModel, R_SKIPPED);
	CR(m_pUserModel->HideUserNameField());

Error:
	return r;
}

RESULT DreamPeerApp::ShowUserNameField() {
	RESULT r = R_PASS;

	CNR(m_pUserModel, R_SKIPPED);
	CR(m_pUserModel->ShowUserNameField());

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

	m_pUserModel->InitializeObject();
	GetDOS()->AddObjectToUIGraph(m_pUserModel->GetUserObjectComposite().get(), SandboxApp::PipelineType::MAIN | SandboxApp::PipelineType::AUX);

	m_pUserModel->SetScreenName(m_strScreenName);
	m_pUserModel->SetProfilePhoto(m_strProfilePhotoURL);

	//m_pUserLabelComposite->SetVisible(true);
	m_pUserModel->GetUserLabelComposite()->SetVisible(true);
	//CR(ShowUserNameField());

	GetDOS()->AddObjectToUIGraph(m_pUserModel->GetMouth().get(), SandboxApp::PipelineType::AUX);
	GetDOS()->AddObject(m_pUserModel->GetHand(HAND_TYPE::HAND_LEFT)->GetPhantomModel().get(), SandboxApp::PipelineType::AUX);
	GetDOS()->AddObject(m_pUserModel->GetHand(HAND_TYPE::HAND_RIGHT)->GetPhantomModel().get(), SandboxApp::PipelineType::AUX);

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
	
	if (m_pUserModel->GetUserLabelComposite() != nullptr) {
		m_pUserModel->GetUserLabelComposite()->SetVisible(fVisible, false);
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
	return m_pUserModel != nullptr && m_pUserModel->IsUserNameVisible();
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

