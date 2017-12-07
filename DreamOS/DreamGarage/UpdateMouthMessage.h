#ifndef UPDATE_MOUTH_MESSAGE_H_
#define UPDATE_MOUTH_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamGarage/UpdateMouthMessage.h
// A useful user update mouth message

#include <string>
#include "DreamGarageMessage.h"

// TODO: We might want to switch this out with other messages

class UpdateMouthMessage : public DreamGarageMessage {
private:
	__declspec(align(4)) struct MessageBody {
		float mouthSize;
	} m_body;

public:
	UpdateMouthMessage(long senderUserID, long receiverUserID, float mouthSize) :
		DreamGarageMessage(senderUserID, receiverUserID, DreamGarageMessage::type::UPDATE_MOUTH, sizeof(UpdateMouthMessage))
	{
		m_body.mouthSize = mouthSize;
	}

	~UpdateMouthMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("UpdateMouthMessage: mouth size: %f", m_body.mouthSize);
		return R_PASS;
	}

	float GetMouthSize() {
		return m_body.mouthSize;
	}
};

#endif	// ! UPDATE_MOUTH_MESSAGE_H_