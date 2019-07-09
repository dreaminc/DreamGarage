#include "MemoryImageObj.h"

MemoryImageObj::MemoryImageObj(int width, int height, int channels) :
	image(width, height, channels)
{
	// empty
}

RESULT MemoryImageObj::LoadFromMemory() {
	RESULT r = R_PASS;

	CBM((m_pImageBuffer == nullptr), "Image buffer is not null");

	m_pImageBuffer_n = sizeof(unsigned char) * m_width * m_height * m_channels;

	m_pImageBuffer = (unsigned char*)malloc(m_pImageBuffer_n);
	CN(m_pImageBuffer);

	memset(m_pImageBuffer, 0, m_pImageBuffer_n);

Error:
	return r;
}