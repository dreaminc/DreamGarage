#include "DreamBrowserMessage.h"

DreamBrowserMessage::DreamBrowserMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamBrowserMessage::type dreamBrowserMessageType, DreamBrowserMessage::type dreamBrowserAckType) :
	DreamAppMessage(senderUserID, receiverUserID, DREAM_BROWSER_APP_NAME, uidSenderDreamApp, sizeof(DreamBrowserMessage))
{
	m_body.m_msgType = dreamBrowserMessageType;
	m_body.m_ackMsgType = dreamBrowserAckType;
}

DreamBrowserMessage::~DreamBrowserMessage() {
	// empty 
}

RESULT DreamBrowserMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamBrowserMessage type: %I64u", m_body.m_msgType);

	if (m_body.m_ackMsgType != type::INVALID) {
		DEBUG_LINEOUT("ack type: %I64u", m_body.m_ackMsgType);
	}

	CR(DreamAppMessage::PrintMessage());

Error:
	return r;
}

RESULT DreamBrowserMessage::SetAckMessageType(DreamBrowserMessage::type ackMsgType) {
	m_body.m_ackMsgType = ackMsgType;
	return R_PASS;
}

DreamBrowserMessage::type DreamBrowserMessage::GetAckType() {
	return m_body.m_ackMsgType;
}

bool DreamBrowserMessage::IsAckMessage() {
	if (m_body.m_msgType == type::ACK)
		return true;

	return false;
}

DreamBrowserMessage::type DreamBrowserMessage::GetMessageType() {
	return m_body.m_msgType;
}