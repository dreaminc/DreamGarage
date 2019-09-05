#include "DreamShareViewPointerMessage.h"

DreamShareViewPointerMessage::DreamShareViewPointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, int seatPosition, char strInitials[2], bool fActuated, bool fInteracting, bool fLeftHand) :
	DreamShareViewMessage(senderUserID, receiverUserID, sizeof(DreamShareViewPointerMessage), uidSenderDreamApp, DreamShareViewMessage::type::POINTER)
{
	m_body.ptPointer = ptPointer;
	m_body.seatPosition = seatPosition;
	m_body.szInitials[0] = strInitials[0];
	m_body.szInitials[1] = strInitials[1];

	// convert bools to enum class
	flags left = flags::NONE;
	flags actuated = flags::NONE;
	flags interacting = flags::NONE;

	if (fLeftHand) {
		left = flags::LEFT;
	}
	if (fActuated) {
		actuated = flags::ACTUATED;
	}
	if (fInteracting) {
		interacting = flags::INTERACTING;
	}

	m_body.fFlags = left | actuated | interacting;

}

DreamShareViewPointerMessage::~DreamShareViewPointerMessage() {
	// empty
}

RESULT DreamShareViewPointerMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("UpdatePointerMessage:");

	m_body.ptPointer.Print("position");

	CR(DreamShareViewMessage::PrintMessage());

Error:
	return r;
}

bool DreamShareViewPointerMessage::IsLeft() {
	return ((m_body.fFlags & flags::LEFT) != flags::NONE);
}

bool DreamShareViewPointerMessage::IsActuated() {
	return ((m_body.fFlags & flags::ACTUATED) != flags::NONE);
}

bool DreamShareViewPointerMessage::IsInteracting() {
	return ((m_body.fFlags & flags::INTERACTING) != flags::NONE);
}
