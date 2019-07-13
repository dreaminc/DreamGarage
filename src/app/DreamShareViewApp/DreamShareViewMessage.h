#ifndef DREAM_SHARE_VIEW_MESSAGE_H_
#define DREAM_SHARE_VIEW_MESSAGE_H_

#define DREAM_SHARE_VIEW_APP_NAME "DreamShareView"

#include "DreamAppMessage.h"

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
