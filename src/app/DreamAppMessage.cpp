#include "DreamAppMessage.h"

DreamAppMessage::DreamAppMessage(long senderUserID, long receiverUserID, std::string strSenderDreamAppName, UID uidSenderDreamApp, long messageSize) :
	DreamMessage(senderUserID, receiverUserID, DreamMessage::type::APP, messageSize)
{
	m_dreamAppMessageHeader.strSenderDreamAppName = strSenderDreamAppName;
	m_dreamAppMessageHeader.uidSenderDreamApp;
}

DreamAppMessage::~DreamAppMessage() {
	// empty
}

RESULT DreamAppMessage::PrintMessage() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DreamAppMessage sender:%s UID: %I64u",
		m_dreamAppMessageHeader.strSenderDreamAppName.c_str(), m_dreamAppMessageHeader.uidSenderDreamApp.GetID());

	CR(DreamMessage::PrintMessage());

Error:
	return R_PASS;
}

std::string DreamAppMessage::GetDreamAppName() {
	return m_dreamAppMessageHeader.strSenderDreamAppName;
}

UID DreamAppMessage::GetDreamAppUID() {
	return m_dreamAppMessageHeader.uidSenderDreamApp;
}

