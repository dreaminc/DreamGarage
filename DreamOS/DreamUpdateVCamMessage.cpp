#include "DreamUpdateVCamMessage.h"

DreamUpdateVCamMessage::DreamUpdateVCamMessage(long senderUserId, long receiverUserID, point ptPosition, quaternion qOrientation, UID uidSenderDreamApp) :
	DreamAppMessage(senderUserId, receiverUserID, DREAM_VCAM_APP_NAME, uidSenderDreamApp, sizeof(DreamUpdateVCamMessage::MessageBody))
{
	m_body.ptPosition = ptPosition;
	m_body.qOrientation = qOrientation;
}

DreamUpdateVCamMessage::~DreamUpdateVCamMessage() {
	// empty
}

RESULT DreamUpdateVCamMessage::PrintMessage() {
	RESULT r = R_PASS;

	CR(DreamAppMessage::PrintMessage());

Error:
	return r;
}
