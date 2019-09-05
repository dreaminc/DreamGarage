#ifndef UPDATE_HEAD_MESSAGE_H_
#define UPDATE_HEAD_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream Garage
// dos/src/garage/UpdateHeadMessage.h

// TODO: Should this be here?

#include <string>

#include "DreamGarageMessage.h"

#include "core/primitives/point.h"
#include "core/primitives/vector.h"
#include "core/primitives/quaternion.h"

class UpdateHeadMessage : public DreamGarageMessage {

private:
	__declspec(align(4)) struct MessageBody {
		point ptPosition;
		quaternion qOrientation;
		vector vVelocity;
		quaternion qAngularVelocity;
	} m_body;

public:
	UpdateHeadMessage(long senderUserID, long receiverUserID, point ptPosition, quaternion qOrientation) :
		DreamGarageMessage(senderUserID, receiverUserID, DreamGarageMessage::type::UPDATE_HEAD, sizeof(UpdateHeadMessage))
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
		DreamGarageMessage(senderUserID, receiverUserID, DreamGarageMessage::type::UPDATE_HEAD, sizeof(UpdateHeadMessage))
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
		Message::PrintMessage();

		m_body.ptPosition.Print("position");
		m_body.qOrientation.Print("orientation");
		m_body.vVelocity.Print("velocity");
		m_body.qAngularVelocity.Print("angular momentum");

		return R_PASS;
	}

public:
	point GetPosition() { return m_body.ptPosition; }
	quaternion GetOrientation() { return m_body.qOrientation; }
	vector GetVelocity() { return m_body.vVelocity; }
	quaternion GetAngularVelocity() { return m_body.qAngularVelocity; }
};

#endif	// ! UPDATE_HEAD_MESSAGE_H_