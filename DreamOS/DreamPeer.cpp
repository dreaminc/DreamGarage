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