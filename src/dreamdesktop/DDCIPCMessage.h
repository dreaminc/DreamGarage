#ifndef _DDCIPCMSG_H_
#define _DDCIPCMSG_H_

class DDCIPCMessage {

public:
	enum class type : unsigned long {
		PING,
		ACK,
		START,
		STOP,
		FRAME,
		RESIZE,
		INVALID
	};

	/*DDCIPCMessage::type GetMessageType() {
		return m_body.m_msgType;
	}

	HRESULT SetType(DDCIPCMessage::type msgType) {
		m_body.m_msgType = msgType;
		return S_OK;
	}
	__declspec(align(8)) struct MessageBody {
		DDCIPCMessage::type m_msgType;
		unsigned int pxWidth;
		unsigned int pxHeight;
		unsigned char pDataBuffer[829440];
	} m_body;*/

	DDCIPCMessage::type m_msgType;
	unsigned int pxWidth;
	unsigned int pxHeight;
	unsigned char* pDataBuffer;

};

#endif
