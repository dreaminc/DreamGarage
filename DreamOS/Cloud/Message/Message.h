#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Cloud/Message/Message.h
// Base Message object

#include <string>
#include <time.h>

#define MESSAGE_INVALID 0xFFFF
#define MESSAGE_SPACE_BIT_SIZE 8

class Message {
public:
	typedef uint64_t DataType;

	enum class type : DataType {
		CLOUD = 1,
		OS = Message::type::CLOUD << MESSAGE_SPACE_BIT_SIZE,
		INVALID = MESSAGE_INVALID
	};


private:
	__declspec(align(8)) struct MessageHeader {
		long senderUserID;
		long receiverUserID;
		time_t timestamp;
		DataType type;
		long messageSize;
	} m_header;

public:
	Message() {
		m_header.senderUserID = -1;
		m_header.receiverUserID = -1;
		m_header.timestamp = time(nullptr);
		m_header.type = (DataType)(type::INVALID);
		m_header.messageSize = sizeof(Message);
	}

	Message(long senderUserID, long receiverUserID, Message::type msgType, long messageSize) {
		m_header.senderUserID = senderUserID;
		m_header.receiverUserID = receiverUserID;
		m_header.timestamp = time(nullptr);
		m_header.type = (DataType)(msgType);
		m_header.messageSize = messageSize;
	}

	~Message() {
		// empty
	}

	Message::DataType GetType() { return m_header.type; }
	long GetSize() { return m_header.messageSize; }
	time_t GetTimeStamp() { return m_header.timestamp; }
	long GetSenderUserID() { return m_header.senderUserID; }
	long GetReceiverUserID() { return m_header.receiverUserID; }

	virtual RESULT PrintMessage() {
		DEBUG_LINEOUT("Message from id %d to id %d at %s size:%d", 
			m_header.senderUserID, m_header.receiverUserID, ctime(&(m_header.timestamp)), m_header.messageSize);

		return R_PASS;
	}
};

#endif	// ! USER_H_