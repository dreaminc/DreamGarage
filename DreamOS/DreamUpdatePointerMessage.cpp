#include "DreamUpdatePointerMessage.h"

DreamUpdatePointerMessage::DreamUpdatePointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, color cColor, bool fVisible, bool fLeftHand) :
	DreamShareViewMessage(senderUserID, receiverUserID, sizeof(DreamUpdatePointerMessage), uidSenderDreamApp, DreamShareViewMessage::type::POINTER)
{
	m_body.ptPointer = ptPointer;
	m_body.cColor = cColor;
	m_body.fVisible = fVisible;
	m_body.fLeftHand = fLeftHand;
}

DreamUpdatePointerMessage::~DreamUpdatePointerMessage() {
	// empty
}

RESULT DreamUpdatePointerMessage::PrintMessage() {
	DEBUG_LINEOUT("UpdatePointerMessage:");
	Message::PrintMessage();

	m_body.ptPointer.Print("position");

	return R_PASS;
}
