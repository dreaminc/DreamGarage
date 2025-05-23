#ifndef PEER_HANDSHAKE_MESSAGE_H_
#define PEER_HANDSHAKE_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream Peer App
// dos/src/app/DreamPeerApp/PeerHandshakeMessage.h

// Peer Handshake MEssage

#include <string>

#include "os/DreamMessage.h"

class PeerHandshakeMessage : public DreamMessage {

private:
	//__declspec(align(4)) struct MessageBody {
	//	// empty
	//} m_body;

public:
	PeerHandshakeMessage(long senderUserID, long receiverUserID) :
		DreamMessage(senderUserID, receiverUserID, DreamMessage::type::PEER_HANDSHAKE, sizeof(PeerHandshakeMessage))
	{
		// empty
	}

	~PeerHandshakeMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("PeerHandshakeMessage:");
		DreamMessage::PrintMessage();
		return R_PASS;
	}
};

#endif	// ! PEER_HANDSHAKE_MESSAGE_H_