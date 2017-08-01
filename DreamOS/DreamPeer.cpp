#include "DreamPeer.h"

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

