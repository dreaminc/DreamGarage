#ifndef PEER_STAY_ALIVE_MESSAGE_H_
#define PEER_STAY_ALIVE_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/PeerHandshakeMessage.h
// Peer Handshake MEssage

#include <string>
#include "DreamMessage.h"

class PeerStayAliveMessage : public DreamMessage {

private:
	//__declspec(align(4)) struct MessageBody {
	//	// empty
	//} m_body;

public:
	PeerStayAliveMessage(long senderUserID, long receiverUserID) :
		DreamMessage(senderUserID, receiverUserID, DreamMessage::type::PEER_STAYALIVE, sizeof(PeerStayAliveMessage))
	{
		// empty
	}

	PeerStayAliveMessage(long senderUserID, long receiverUserID) :
		DreamMessage(senderUserID, receiverUserID, DreamMessage::type::PEER_STAYALIVE, sizeof(PeerStayAliveMessage))
	{
		// empty
	}

	~PeerStayAliveMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("PeerStayAliveMessage:");
		DreamMessage::PrintMessage();
		return R_PASS;
	}
};

#endif	// ! PEER_STAY_ALIVE_MESSAGE_H_