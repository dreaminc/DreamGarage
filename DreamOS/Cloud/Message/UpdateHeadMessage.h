#ifndef UPDATE_HEAD_MESSAGE_H_
#define UPDATE_HEAD_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Message/Message.h
// Base Message object

#include <string>
#include "Message.h"

#include "Primitives/point.h"
#include "Primitives/vector.h"
#include "Primitives/quaternion.h"

class UpdateHeadMessage : public Message {

private:
	__declspec(align(4)) struct MessageBody {
		point ptPosition;
		quaternion qOrientation;
		vector vVelocity;
		quaternion qAngularVelocity;
	} m_body;

public:
	UpdateHeadMessage(long senderUserID, long receiverUserID, point ptPosition, quaternion qOrientation) :
		Message(senderUserID, receiverUserID, MessageType::MESSAGE_UPDATE_HEAD, sizeof(UpdateHeadMessage))
	{
		m_body.ptPosition = ptPosition;
		m_body.qOrientation = qOrientation;
		m_body.vVelocity = vector();
		m_body.qAngularVelocity = quaternion();
	}

	UpdateHeadMessage(long senderUserID, long receiverUserID, 
					  point ptPosition, quaternion qOrientation, 
					  vector vVelocity, quaternion qAngularVelocity
	) :
		Message(senderUserID, receiverUserID, MessageType::MESSAGE_UPDATE_HEAD, sizeof(UpdateHeadMessage))
	{
		m_body.ptPosition = ptPosition;
		m_body.qOrientation = qOrientation;
		m_body.vVelocity = vVelocity;
		m_body.qAngularVelocity = qAngularVelocity;
	}

	~UpdateHeadMessage() {
		// empty
	}

	RESULT PrintMessage() override {
		DEBUG_LINEOUT("UpdateHeadMessage:");
		
		m_body.ptPosition.Print("position");
		m_body.qOrientation.Print("orientation");
		m_body.vVelocity.Print("velocity");
		m_body.qAngularVelocity.Print("angular momentum");

		return R_PASS;
	}
};

#endif	// ! UPDATE_HEAD_MESSAGE_H_