#ifndef DREAM_GARAGE_MESSAGE_H_
#define DREAM_GARAGE_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/DreamGarageMessage.h
// Base Message object

#include <string>
#include "DreamMessage.h"

class DreamGarageMessage : public DreamMessage {
public:
	enum class type : Message::DataType {
		CLIENT = (Message::DataType)(DreamMessage::type::CLIENT),
		UPDATE_HEAD,
		UPDATE_HAND,
		UPDATE_MOUTH,
		AUDIO_DATA,
		UPDATE_CHAT,
		CUSTOM,
		INVALID = MESSAGE_INVALID
	};

private:
	/*
	__declspec(align(4)) struct MessageBody {
		// Empty
	} m_body;
	*/

public:
	DreamGarageMessage() :
		DreamMessage()
	{
		// empty
	}

	DreamGarageMessage(long senderUserID, long receiverUserID, DreamGarageMessage::type msgType, long messageSize) :
		DreamMessage(senderUserID, receiverUserID, (DreamMessage::type)(msgType), messageSize)
	{
		// empty
	}

	~DreamGarageMessage() {
		// empty
	}

	virtual RESULT PrintMessage() override {
		DEBUG_LINEOUT("DreamGarageMessage:");
		DreamMessage::PrintMessage();
		return R_PASS;
	}
};

#endif	// ! DREAM_GARAGE_MESSAGE_H_