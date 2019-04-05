#ifndef DREAM_POINTER_MESSAGE_H_
#define DREAM_POINTER_MESSAGE_H_

#include "RESULT/EHM.h"

#include <string>
#include "DreamShareViewMessage.h"
#include "Primitives/Types/UID.h"

#include "Primitives/point.h"
#include "Primitives/color.h"

class DreamShareViewPointerMessage : public DreamShareViewMessage {

public:
	__declspec(align(4)) struct MessageBody {
		point ptPointer;
		int seatPosition;
		char szInitials[2]; // always 2 characters

		bool fActuated;
		bool fInteracting;
		bool fLeftHand;

	} m_body;

public:
	DreamShareViewPointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, int seatPosition, char strInitials[2], bool fActuated, bool fInteracting, bool fLeftHand);
	~DreamShareViewPointerMessage();

	RESULT PrintMessage() override;

};

#endif // ! DREAM_POINTER_MESSAGE_H_