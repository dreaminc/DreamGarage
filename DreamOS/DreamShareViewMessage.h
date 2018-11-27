#ifndef DREAM_BROWSER_MESSAGE_H_
#define DREAM_BROWSER_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamShareViewMessage.h
// A base message for dream app messages

#include <string>
#include "DreamAppMessage.h"
#include "Primitives/Types/UID.h"

#define DREAM_SHARE_VIEW_APP_NAME "DreamShareView"

class DreamShareViewMessage : public DreamAppMessage {

public:
	enum class type : Message::DataType {
		PING,
		ACK,
		REQUEST_STREAMING_START,
		REPORT_STREAMING_STOP,
		INVALID
	};

private:
	__declspec(align(8)) struct MessageBody {
		DreamShareViewMessage::type m_msgType;
		DreamShareViewMessage::type m_ackMsgType;
	} m_body;

public:
	DreamShareViewMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamShareViewMessage::type dreamBrowserMessageType, DreamShareViewMessage::type dreamBrowserAckType = DreamShareViewMessage::type::INVALID);
	~DreamShareViewMessage();

	RESULT SetAckMessageType(DreamShareViewMessage::type ackMsgType);

	DreamShareViewMessage::type GetAckType();
	bool IsAckMessage();
	DreamShareViewMessage::type GetMessageType();

	virtual RESULT PrintMessage() override;
};

#endif	// ! DREAM_BROWSER_MESSAGE_H_