#include "DreamBrowserMessage.h"

DreamBrowserMessage::DreamBrowserMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp) :
	DreamAppMessage(senderUserID, receiverUserID, DREAM_BROWSER_APP_NAME, uidSenderDreamApp)
{
	m_body.m_msgType = type::PING;
}

DreamBrowserMessage::~DreamBrowserMessage() {
	// empty 
}

RESULT DreamBrowserMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamBrowserMessage type: %I64u", m_body.m_msgType);

	CR(DreamAppMessage::PrintMessage());

Error:
	return r;
}