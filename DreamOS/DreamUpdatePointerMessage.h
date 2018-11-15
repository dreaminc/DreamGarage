#ifndef DREAM_POINTER_MESSAGE_H_
#define DREAM_POINTER_MESSAGE_H_

#include "RESULT/EHM.h"

#include <string>
#include "DreamAppMessage.h"
#include "Primitives/Types/UID.h"

#include "Primitives/point.h"
#include "Primitives/color.h"

#define DREAM_SHARE_VIEW_APP_NAME "DreamShareView.Pointer"

class DreamUpdatePointerMessage : public DreamAppMessage {

public:
	__declspec(align(8)) struct MessageBody {
		point ptPointer;
		color cColor;
		bool fVisible;
		bool fLeftHand;
	} m_body;

public:
	DreamUpdatePointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, color cColor, bool fVisible, bool fLeftHand);
	~DreamUpdatePointerMessage();

	RESULT PrintMessage() override;

};

#endif // ! DREAM_POINTER_MESSAGE_H_