#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Message/Message.h
// Base Message object

#include <string>
#include <time.h>

class Message {
public:
	typedef enum class MESSAGE_TYPE : uint16_t {
		MESSAGE_UPDATE_HEAD,
		MESSAGE_UPDATE_HAND,
		MESSAGE_UPDATE_CHAT,
		MESSAGE_INVALID
	} MessageType;

private:
	__declspec(align(8)) struct MessageHeader {
		long senderUserID;
		long receiverUserID;
		time_t timestamp;
		MessageType type;
		long messageSize;
	} m_header;

public:
	Message() {
		m_header.senderUserID = -1;
		m_header.receiverUserID = -1;
		m_header.timestamp = time(nullptr);
		m_header.type = MessageType::MESSAGE_INVALID;
		m_header.messageSize = sizeof(Message);
	}

	Message(long senderUserID, long receiverUserID, MessageType type, long messageSize) {
		m_header.senderUserID = senderUserID;
		m_header.receiverUserID = receiverUserID;
		m_header.timestamp = time(nullptr);
		m_header.type = type;
		m_header.messageSize = messageSize;
	}

	~Message() {
		// empty
	}

	Message::MessageType GetType() {
		return m_header.type;
	}

	virtual RESULT PrintMessage() = 0;
};

#endif	// ! USER_H_