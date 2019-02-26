#include "DreamShareViewPointerMessage.h"

DreamShareViewPointerMessage::DreamShareViewPointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, char strInitials[2], bool fVisible, bool fLeftHand) :
	DreamShareViewMessage(senderUserID, receiverUserID, sizeof(DreamShareViewPointerMessage), uidSenderDreamApp, DreamShareViewMessage::type::POINTER)
{
	m_body.ptPointer = ptPointer;
	m_body.szInitials[0] = strInitials[0];
	m_body.szInitials[1] = strInitials[1];
	m_body.fVisible = fVisible;
	m_body.fLeftHand = fLeftHand;
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
