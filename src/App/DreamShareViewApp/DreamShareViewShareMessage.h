#ifndef DREAM_SHARE_VIEW_SHARE_MESSAGE_H_
#define DREAM_SHARE_VIEW_SHARE_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamShareViewMessage.h
// A base message for dream app messages

#include <string>
#include "DreamShareViewMessage.h"

#include "Primitives/Types/UID.h"

class DreamShareViewShareMessage : public DreamShareViewMessage {

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
		DreamShareViewShareMessage::type m_msgType;
		DreamShareViewShareMessage::type m_ackMsgType;
	} m_body;

public:
	DreamShareViewShareMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamShareViewShareMessage::type dreamBrowserMessageType, DreamShareViewShareMessage::type dreamBrowserAckType = DreamShareViewShareMessage::type::INVALID);
	~DreamShareViewShareMessage();

	RESULT SetAckMessageType(DreamShareViewShareMessage::type ackMsgType);

	DreamShareViewShareMessage::type GetAckType();
	bool IsAckMessage();
	DreamShareViewShareMessage::type GetShareMessageType();

	virtual RESULT PrintMessage() override;
};

#endif	// ! DREAM_SHARE_VIEW_SHARE_MESSAGE_H_