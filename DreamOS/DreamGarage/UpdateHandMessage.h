#ifndef UPDATE_HAND_MESSAGE_H_
#define UPDATE_HAND_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Message/Message.h
// Base Message object

#include <string>
#include "DreamGarageMessage.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"
#include "Primitives/hand.h"

// TODO: We might want to switch this out with other messages

class UpdateHandMessage : public DreamGarageMessage {
private:
	__declspec(align(4)) struct MessageBody {
		hand::HandState handState;
	} m_body;

public:
	UpdateHandMessage(long senderUserID, long receiverUserID, hand::HandState handState) :
		DreamGarageMessage(senderUserID, receiverUserID, DreamGarageMessage::type::UPDATE_HAND, sizeof(UpdateHandMessage))
	{
		m_body.handState = handState;
	}

	~UpdateHandMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("UpdateHandMessage:");
		m_body.handState.PrintState();
		return R_PASS;
	}

	hand::HandState GetHandState() {
		return m_body.handState;
	}
};

#endif	// ! UPDATE_HAND_MESSAGE_H_