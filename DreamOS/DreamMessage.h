#ifndef DREAM_MESSAGE_H_
#define DREAM_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamGarageMessage.h
// Base Message object

#include <string>
#include "Cloud/Message/Message.h"

class DreamMessage : public Message {
public:
	enum class type : Message::DataType {
		OS = (Message::DataType)(Message::type::OS),
		PEER_HANDSHAKE,
		PEER_STAYALIVE,
		PEER_ACK,
		CLIENT	= DreamMessage::type::OS << MESSAGE_SPACE_BIT_SIZE,		// TODO: This may be deprecated as everything becomes an APP
		APP = DreamMessage::type::CLIENT << MESSAGE_SPACE_BIT_SIZE,
		INVALID = MESSAGE_INVALID
	};

private:
	/*
	__declspec(align(4)) struct MessageBody {
	// Empty
	} m_body;
	*/

public:
	DreamMessage() :
		Message()
	{
		// empty
	}

	DreamMessage(long senderUserID, long receiverUserID, DreamMessage::type msgType, long messageSize) :
		Message(senderUserID, receiverUserID, (Message::type)(msgType), messageSize)
	{
		// empty
	}

	~DreamMessage() {
		// empty
	}

	virtual RESULT PrintMessage() override {
		DEBUG_LINEOUT("DreamGarageMessage:");
		Message::PrintMessage();
		return R_PASS;
	}
};

#endif	// ! DREAM_MESSAGE_H_