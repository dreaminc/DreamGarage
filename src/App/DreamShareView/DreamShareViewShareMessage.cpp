#include "DreamShareViewShareMessage.h"

DreamShareViewShareMessage::DreamShareViewShareMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamShareViewShareMessage::type dreamBrowserMessageType, DreamShareViewShareMessage::type dreamBrowserAckType) :
	DreamShareViewMessage(senderUserID, receiverUserID, sizeof(DreamShareViewShareMessage), uidSenderDreamApp, DreamShareViewMessage::type::SHARE)
{
	m_body.m_msgType = dreamBrowserMessageType;
	m_body.m_ackMsgType = dreamBrowserAckType;
}

DreamShareViewShareMessage::~DreamShareViewShareMessage() {
	// empty 
}

RESULT DreamShareViewShareMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamShareViewMessage type: %I64u", m_body.m_msgType);

	if (m_body.m_ackMsgType != type::INVALID) {
		DEBUG_LINEOUT("ack type: %I64u", m_body.m_ackMsgType);
	}

	CR(DreamShareViewMessage::PrintMessage());

Error:
	return r;
}

RESULT DreamShareViewShareMessage::SetAckMessageType(DreamShareViewShareMessage::type ackMsgType) {
	m_body.m_ackMsgType = ackMsgType;
	return R_PASS;
}

DreamShareViewShareMessage::type DreamShareViewShareMessage::GetAckType() {
	return m_body.m_ackMsgType;
}

bool DreamShareViewShareMessage::IsAckMessage() {
	if (m_body.m_msgType == type::ACK)
		return true;

	return false;
}

DreamShareViewShareMessage::type DreamShareViewShareMessage::GetShareMessageType() {
	return m_body.m_msgType;
}