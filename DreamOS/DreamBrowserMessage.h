#ifndef DREAM_BROWSER_MESSAGE_H_
#define DREAM_BROWSER_MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamBrowserMessage.h
// A base message for dream app messages

#include <string>
#include "DreamAppMessage.h"
#include "Primitives/Types/UID.h"

#define DREAM_BROWSER_APP_NAME "DreamBrowser"

class DreamBrowserMessage : public DreamAppMessage {

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
		DreamBrowserMessage::type m_msgType;
		DreamBrowserMessage::type m_ackMsgType;
	} m_body;

public:
	DreamBrowserMessage(long senderUserID, long receiverUserID, UID uidSenderDreamApp, DreamBrowserMessage::type dreamBrowserMessageType, DreamBrowserMessage::type dreamBrowserAckType = DreamBrowserMessage::type::INVALID);
	~DreamBrowserMessage();

	RESULT SetAckMessageType(DreamBrowserMessage::type ackMsgType);

	DreamBrowserMessage::type GetAckType();
	bool IsAckMessage();
	DreamBrowserMessage::type GetMessageType();

	virtual RESULT PrintMessage() override;
};

#endif	// ! DREAM_BROWSER_MESSAGE_H_