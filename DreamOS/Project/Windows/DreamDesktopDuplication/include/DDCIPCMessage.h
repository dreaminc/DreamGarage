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
		INVALID
	};

	enum class PIXEL_FORMAT {
		Unspecified, // this will generate an RGB/RGBA based on the number of channels - 1 to 1 with color.h
		RGB,
		RGBA,
		BGR,
		BGRA,
		INVALID
	};
	
	DDCIPCMessage::type GetMessage() {
		return m_body.m_msgType;
	}

	HRESULT SetType(DDCIPCMessage::type msgType) {
		m_body.m_msgType = msgType;
		return S_OK;
	}

	__declspec(align(8)) struct MessageBody {
		DDCIPCMessage::type m_msgType;
		unsigned char* pDataBuffer;
		PIXEL_FORMAT pixelFormat;
	} m_body;
};

#endif
