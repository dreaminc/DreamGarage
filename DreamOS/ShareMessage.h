#ifndef DREAM_BROWSER_MESSAGE_H_
#define DREAM_BROWSER_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamShareViewMessage.h
// A base message for dream app messages

#include <string>
#include "DreamShareViewMessage.h"

#include "Primitives/Types/UID.h"

class ShareMessage : public DreamShareViewMessage {

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
		ShareMessage::type m_msgType;
		ShareMessage::type m_ackMsgType;
	} m_body;

public:
	ShareMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, ShareMessage::type dreamBrowserMessageType, ShareMessage::type dreamBrowserAckType = ShareMessage::type::INVALID);
	~ShareMessage();

	RESULT SetAckMessageType(ShareMessage::type ackMsgType);

	ShareMessage::type GetAckType();
	bool IsAckMessage();
	ShareMessage::type GetShareMessageType();

	virtual RESULT PrintMessage() override;
};

#endif	// ! DREAM_BROWSER_MESSAGE_H_