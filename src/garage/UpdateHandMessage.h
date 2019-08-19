#ifndef UPDATE_HAND_MESSAGE_H_
#define UPDATE_HAND_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream Garage
// dos/src/garage/UpdateHandMessage.h

// TODO: Does this belong here?

#include <string>

#include "DreamGarageMessage.h"

class hand;

#include "core/hand/HandState.h"

// TODO: We might want to switch this out with other messages

class UpdateHandMessage : public DreamGarageMessage {
private:
	__declspec(align(4)) struct MessageBody {
		HandState handState;
	} m_body;

public:
	UpdateHandMessage(long senderUserID, long receiverUserID, HandState handState) :
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

	HandState GetHandState() {
		return m_body.handState;
	}
};

#endif	// ! UPDATE_HAND_MESSAGE_H_