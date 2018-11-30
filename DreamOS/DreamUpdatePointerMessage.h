#ifndef DREAM_POINTER_MESSAGE_H_
#define DREAM_POINTER_MESSAGE_H_

#include "RESULT/EHM.h"

#include <string>
#include "DreamShareViewMessage.h"
#include "Primitives/Types/UID.h"

#include "Primitives/point.h"
#include "Primitives/color.h"

class DreamUpdatePointerMessage : public DreamShareViewMessage {

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