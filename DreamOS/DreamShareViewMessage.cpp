#include "DreamShareViewMessage.h"

DreamShareViewMessage::DreamShareViewMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamShareViewMessage::type dreamBrowserMessageType, DreamShareViewMessage::type dreamBrowserAckType) :
	DreamAppMessage(senderUserID, receiverUserID, DREAM_SHARE_VIEW_APP_NAME, uidSenderDreamApp, sizeof(DreamShareViewMessage))
{
	m_body.m_msgType = dreamBrowserMessageType;
	m_body.m_ackMsgType = dreamBrowserAckType;
}

DreamShareViewMessage::~DreamShareViewMessage() {
	// empty 
}

RESULT DreamShareViewMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamShareViewMessage type: %I64u", m_body.m_msgType);

	if (m_body.m_ackMsgType != type::INVALID) {
		DEBUG_LINEOUT("ack type: %I64u", m_body.m_ackMsgType);
	}

	CR(DreamAppMessage::PrintMessage());

Error:
	return r;
}

RESULT DreamShareViewMessage::SetAckMessageType(DreamShareViewMessage::type ackMsgType) {
	m_body.m_ackMsgType = ackMsgType;
	return R_PASS;
}

DreamShareViewMessage::type DreamShareViewMessage::GetAckType() {
	return m_body.m_ackMsgType;
}

bool DreamShareViewMessage::IsAckMessage() {
	if (m_body.m_msgType == type::ACK)
		return true;

	return false;
}

DreamShareViewMessage::type DreamShareViewMessage::GetMessageType() {
	return m_body.m_msgType;
}