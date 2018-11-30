#include "DreamShareViewMessage.h"

DreamShareViewMessage::DreamShareViewMessage(long senderUserID, long receiverUserID, long messageSize, UID uidSenderDreamApp, DreamShareViewMessage::type dreamBrowserMessageType) :
	DreamAppMessage(senderUserID, receiverUserID, DREAM_SHARE_VIEW_APP_NAME, uidSenderDreamApp, messageSize)
{
	m_body.m_msgType = dreamBrowserMessageType;
}

DreamShareViewMessage::~DreamShareViewMessage() {

}

DreamShareViewMessage::type DreamShareViewMessage::GetMessageType() {
	return m_body.m_msgType;
}
