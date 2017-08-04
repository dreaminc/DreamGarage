#include "DreamPeer.h"
#include "DreamOS.h"

DreamPeer::DreamPeer(DreamOS *pDOS, PeerConnection *pPeerConnection) :
	m_pDOS(pDOS),
	m_state(DreamPeer::state::UNINITIALIZED),
	m_pPeerConnection(pPeerConnection)
{
	// empty
}


RESULT DreamPeer::Initialize() {
	RESULT r = R_PASS;

	CR(SetState(state::INITIALIZED)); 

Error:
	return r;
}

RESULT DreamPeer::RegisterDreamPeerObserver(DreamPeerObserver* pDreamPeerObserver) {
	RESULT r = R_PASS;

	CNM((pDreamPeerObserver), "Observer cannot be nullptr");
	CBM((m_pDreamPeerObserver == nullptr), "Can't overwrite dream peer observer");
	m_pDreamPeerObserver = pDreamPeerObserver;

Error:
	return r;
}

DreamPeer::state DreamPeer::GetState() {
	return m_state;
}

RESULT DreamPeer::SetState(DreamPeer::state peerState) {
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

long DreamPeer::GetPeerUserID() {
	return m_peerUserID;
}

PeerConnection* DreamPeer::GetPeerConnection() {
	return m_pPeerConnection;
}

user* DreamPeer::GetUserModel() {
	return m_pUserModel;
}

RESULT DreamPeer::AssignUserModel(user* pUserModel) {
	RESULT r = R_PASS;

	CBN(m_pUserModel);
	m_pUserModel = pUserModel;

Error:
	return r;
}

// TODO: We should create a proper object pool design
RESULT DreamPeer::ReleaseUserModel() {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel = nullptr;

Error:
	return r;
}

RESULT DreamPeer::SetVisible(bool fVisibile) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	CR(m_pUserModel->SetVisible(fVisibile));

Error:
	return r;
}

RESULT DreamPeer::SetPosition(const point& ptPosition) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->GetHead()->SetPosition(ptPosition);

Error:
	return r;
}

RESULT DreamPeer::SetOrientation(const quaternion& qOrientation) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->GetHead()->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamPeer::UpdateMouth(float mouthScale) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->UpdateMouth(mouthScale);

Error:
	return r;
}

RESULT DreamPeer::UpdateHand(const hand::HandState& handState) {
	RESULT r = R_PASS;

	CN(m_pUserModel);
	m_pUserModel->UpdateHand(handState);

Error:
	return r;
}

WebRTCPeerConnectionProxy* DreamPeer::GetWebRTCPeerConnectionProxy() {
	RESULT r = R_PASS;

	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy = nullptr;

	CN(m_pDOS);

	pWebRTCPeerConnectionProxy = m_pDOS->GetWebRTCPeerConnectionProxy(m_pPeerConnection);

	return pWebRTCPeerConnectionProxy;
Error:
	return nullptr;
}

bool DreamPeer::IsPeerReady() {
	return m_peerConnectionState.fReceivedAndRespondedToHandshake;
}

RESULT DreamPeer::OnDataChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fDataChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeer::OnAudioChannel() {
	RESULT r = R_PASS;

	m_peerConnectionState.fAudioChannel = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeer::UpdatePeerHandshakeState() {
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

RESULT DreamPeer::SentHandshakeRequest() {
	RESULT r = R_PASS;

	CB((m_peerConnectionState.fSentHandshakeRequest == false));
	m_peerConnectionState.fSentHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeer::ReceivedHandshakeACK() {
	RESULT r = R_PASS;

	CB((m_peerConnectionState.fSentHandshakeRequest == true));

	m_peerConnectionState.fSentHandshakeRequest = false;
	m_peerConnectionState.fReceivedHandshakeAck = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeer::ReceivedHandshakeRequest() {
	RESULT r = R_PASS;

	m_peerConnectionState.fReceivedHandshakeRequest = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

RESULT DreamPeer::SentHandshakeACK() {
	RESULT r = R_PASS;

	m_peerConnectionState.fReceivedHandshakeRequest = false;
	m_peerConnectionState.fSentHandshakeRequestACK = true;

	CR(UpdatePeerHandshakeState());

Error:
	return r;
}

