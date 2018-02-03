#ifndef _DDCIPCMSG_H_
#define _DDCIPCMSG_H_

class DDCIPCmessage {

public:
	enum class type {
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
	
	DDCIPCmessage::type GetMessage() {
		return m_body.m_msgType;
	}

private:
	__declspec(align(8)) struct MessageBody {
		DDCIPCmessage::type m_msgType;
		unsigned char* pDataBuffer;
		PIXEL_FORMAT pixelFormat;
	} m_body;
};

#endif
