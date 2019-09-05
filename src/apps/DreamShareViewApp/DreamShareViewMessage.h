#ifndef DREAM_SHARE_VIEW_MESSAGE_H_
#define DREAM_SHARE_VIEW_MESSAGE_H_

#include "core/ehm/EHM.h"

// Dream Share View App
// dos/src/app/DreamShareViewApp/DreamShareViewMessage.h

#include "os/app/DreamAppMessage.h"

#define DREAM_SHARE_VIEW_APP_NAME "DreamShareView"


class DreamShareViewMessage : public DreamAppMessage {

public:
	enum class type : Message::DataType {
		SHARE,
		POINTER,
		INVALID
	};

private:
	__declspec(align(8)) struct MessageBody {
		DreamShareViewMessage::type m_msgType;
	} m_body;

public:
	DreamShareViewMessage(long senderUserID, long receiverUserID, long messageSize, UID uidSenderDreamApp, DreamShareViewMessage::type dreamShareViewMessageType);
	~DreamShareViewMessage();

	DreamShareViewMessage::type GetMessageType();

	virtual RESULT PrintMessage() override;
};

#endif DREAM_SHARE_VIEW_MESSAGE_H_
