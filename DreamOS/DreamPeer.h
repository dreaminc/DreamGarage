#ifndef DREAM_PEER_H_
#define DREAM_PEER_H_

#include "RESULT/EHM.h"

// DREAM PEER
// DreamOS/DreamPeer.h
// Dream Peer holds context for a given dream peer
// and keeps track of state

#include "Primitives/DObject.h"

class User;
class PeerConnection;
class composite;
class DreamOS;

class DreamPeer : public DObject {
public:
	enum class state : uint16_t {
		UNINITIALIZED,
		INITIALIZED,
		PENDING,
		ESTABLISHED,
		DISCONNECTED,
		INVALID
	};

	struct PeerConnectionState {
		unsigned fPendingHandshakeAck : 1;
		unsigned fReceivedAndRespondedToHandshake : 1;

		// Hang detection
		uint32_t rxStayAliveCounter;
		uint32_t txStayAliveAckCounter;
	};

public:
	DreamPeer::DreamPeer(DreamOS *pDOS, PeerConnection *pPeerConnection);

	RESULT Initialize();

	DreamPeer::state GetState();
	long GetPeerUserID();

private:
	long m_peerUserID = -1;
	DreamOS *m_pDOS = nullptr;
	PeerConnection *m_pPeerConnection = nullptr;
	DreamPeer::state m_state = DreamPeer::state::UNINITIALIZED;

private:
	PeerConnectionState m_peerConnectionState = {0};
};

#endif // ~ DREAM_PEER_H_