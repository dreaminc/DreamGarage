#ifndef UPDATE_VCAM_MESSAGE_H_
#define UPDATE_VCAM_MESSAGE_H_

#define DREAM_VCAM_APP_NAME "DreamVCam"

#include "DreamAppMessage.h"
#include "Primitives/point.h"
#include "Primitives/quaternion.h"

class DreamUpdateVCamMessage : public DreamAppMessage {
private:
	__declspec(align(4)) struct MessageBody {
		point ptPosition;
		quaternion qOrientation;
	} m_body;

public:
	DreamUpdateVCamMessage(long senderUserId, long receiverUserID, point ptPosition, quaternion qOrientation, UID uidSenderDreamApp);
	~DreamUpdateVCamMessage();

	virtual RESULT PrintMessage() override;
};

#endif // ! UPDATE_VCAM_MESSAGE_H_