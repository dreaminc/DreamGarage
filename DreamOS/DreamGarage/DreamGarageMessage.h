#ifndef DREAM_GARAGE_MESSAGE_H_
#define DREAM_GARAGE_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamGarageMessage.h
// Base Message object

#include <string>
#include "Cloud/Message/Message.h"

class DreamGarageMessage : public Message {
public:
	enum class type {
		UPDATE_HEAD,
		UPDATE_HAND,
		AUDIO_DATA,
		UPDATE_CHAT,
		CUSTOM,
		INVALID
	};

private:
	/*
	__declspec(align(4)) struct MessageBody {
		// Empty
	} m_body;
	*/

public:
	DreamGarageMessage() :
		Message()
	{
		// empty
	}

	DreamGarageMessage(long senderUserID, long receiverUserID, DreamGarageMessage::type msgType, long messageSize) :
		Message(senderUserID, receiverUserID, (Message::MessageType)(msgType), messageSize)
	{
		// empty
	}

	~DreamGarageMessage() {
		// empty
	}

	virtual RESULT PrintMessage() override {
		DEBUG_LINEOUT("DreamGarageMessage:");
		Message::PrintMessage();
		return R_PASS;
	}
};

#endif	// ! DREAM_GARAGE_MESSAGE_H_