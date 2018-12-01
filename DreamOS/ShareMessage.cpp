#include "ShareMessage.h"

ShareMessage::ShareMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, ShareMessage::type dreamBrowserMessageType, ShareMessage::type dreamBrowserAckType) :
	DreamShareViewMessage(senderUserID, receiverUserID, sizeof(ShareMessage), uidSenderDreamApp, DreamShareViewMessage::type::SHARE)
{
	m_body.m_msgType = dreamBrowserMessageType;
	m_body.m_ackMsgType = dreamBrowserAckType;
}

ShareMessage::~ShareMessage() {
	// empty 
}

RESULT ShareMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamShareViewMessage type: %I64u", m_body.m_msgType);

	if (m_body.m_ackMsgType != type::INVALID) {
		DEBUG_LINEOUT("ack type: %I64u", m_body.m_ackMsgType);
	}

	CR(DreamAppMessage::PrintMessage());

Error:
	return r;
}

RESULT ShareMessage::SetAckMessageType(ShareMessage::type ackMsgType) {
	m_body.m_ackMsgType = ackMsgType;
	return R_PASS;
}

ShareMessage::type ShareMessage::GetAckType() {
	return m_body.m_ackMsgType;
}

bool ShareMessage::IsAckMessage() {
	if (m_body.m_msgType == type::ACK)
		return true;

	return false;
}

ShareMessage::type ShareMessage::GetShareMessageType() {
	return m_body.m_msgType;
}