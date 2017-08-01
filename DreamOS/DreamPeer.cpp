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

	CR(r);

	// Do stuff

Error:
	return r;
}

DreamPeer::state DreamPeer::GetState() {
	return m_state;
}

long DreamPeer::GetPeerUserID() {
	return m_peerUserID;
}

bool DreamPeer::IsPeerReady() {
	return m_peerConnectionState.fReceivedAndRespondedToHandshake;
}

RESULT DreamPeer::UpdatePeerHandshakeState() {
	if (m_peerConnectionState.fReceivedHandshakeAck && m_peerConnectionState.fSentHandshakeRequestACK) {
		m_peerConnectionState.fReceivedAndRespondedToHandshake = true;
	}
	else {
		m_peerConnectionState.fReceivedAndRespondedToHandshake = false;
	}

	return R_PASS;
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

