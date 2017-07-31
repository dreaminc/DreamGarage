#ifndef PEER_STAY_ALIVE_MESSAGE_H_
#define PEER_STAY_ALIVE_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/PeerAckMessage.h
// Peer ACK Message

#include <string>
#include "DreamMessage.h"

class PeerAckMessage : public DreamMessage {

public:
	enum class type : Message::DataType {
		PEER_HANDSHAKE,
		PEER_STAY_ALIVE,
		INVALID
	};

private:
	__declspec(align(8)) struct MessageBody {
		PeerAckMessage::type m_ackType;
	} m_body;

public:
	PeerAckMessage(long senderUserID, long receiverUserID, PeerAckMessage::type ackType ) :
		DreamMessage(senderUserID, receiverUserID, DreamMessage::type::PEER_ACK, sizeof(PeerAckMessage))
	{
		// empty
	}

	~PeerAckMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("PeerAckMessage:");
		DreamMessage::PrintMessage();
		return R_PASS;
	}

	PeerAckMessage::type GetACKType() {
		return m_ackType;
	}

private:
	PeerAckMessage::type m_ackType = PeerAckMessage::type::INVALID;
};

#endif	// ! PEER_STAY_ALIVE_MESSAGE_H_