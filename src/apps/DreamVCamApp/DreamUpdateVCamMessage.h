#ifndef UPDATE_VCAM_MESSAGE_H_
#define UPDATE_VCAM_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream VCam
// dos/src/apps/DreamVCamApp/DreamUpdateVCamMessage.h

#include "os/app/DreamAppMessage.h"

#include "core/primitives/point.h"
#include "core/primitives/quaternion.h"

//#define DREAM_VCAM_APP_NAME "DreamVCam"
#define DREAM_VCAM_APP_NAME "hello"

class DreamUpdateVCamMessage : public DreamAppMessage {
public:
	__declspec(align(8)) struct MessageBody {
		point ptPosition;
		quaternion qOrientation;
	} m_body;

public:
	DreamUpdateVCamMessage(long senderUserId, long receiverUserID, point ptPosition, quaternion qOrientation, UID uidSenderDreamApp);
	~DreamUpdateVCamMessage();

	virtual RESULT PrintMessage() override;
};

#endif // ! UPDATE_VCAM_MESSAGE_H_