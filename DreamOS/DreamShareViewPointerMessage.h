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
	__declspec(align(8)) struct MessageBody {
		point ptPointer;
		char strInitials[2]; // always 2 characters

		bool fVisible;
		bool fLeftHand;
		unsigned int reserved0;
		unsigned int reserved1;

	} m_body;

public:
	DreamShareViewPointerMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, point ptPointer, char strInitials[2], bool fVisible, bool fLeftHand);
	~DreamShareViewPointerMessage();

	RESULT PrintMessage() override;

};

#endif // ! DREAM_POINTER_MESSAGE_H_