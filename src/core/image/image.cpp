#include "image.h"

image::image(std::wstring wstrFilename) :
	m_wstrFilename(wstrFilename)
{
	// empty
}

image::image(uint8_t *pBuffer, size_t pBuffer_n) :
	m_pSourceBuffer(pBuffer),
	m_pSourceBuffer_n(pBuffer_n)
{
	// empty
}

image::image(int width, int height, int channels) :
	m_width(width),
	m_height(height),
	m_channels(channels)
{
	// empty
}

image::~image() {
	// Release the image / texture
	Release();
}

RESULT image::FlipVertical() {
	RESULT r = R_PASS;

	CN(m_pImageBuffer);

	for (int i = 0; i * 2 < m_height; i++) {
		int index1 = i * m_width * m_channels;
		int index2 = (m_height - 1 - i) * m_width * m_channels;

		for (int j = m_width * m_channels; j > 0; j--) {
			std::swap(m_pImageBuffer[index1++], m_pImageBuffer[index2++]);
		}
	}

Error:
	return r;
}

double image::GetAverageValueAtUV(double uValue, double vValue) {
	int pxValueX = static_cast<int>(uValue * (m_width - 1));
	int pxValueY = static_cast<int>(vValue * (m_height - 1));

	int lookUp = pxValueX * (sizeof(unsigned char) * m_channels) + (pxValueY * (sizeof(unsigned char) * m_channels * m_width));

	int accum = 0;
	for (int i = 0; i < m_channels; i++) {
		accum += m_pImageBuffer[lookUp + i];
	}

	double retVal = (double)((double)accum / (double)m_channels);
	retVal /= 255.0f;

	return retVal;
}

RESULT image::CopyBuffer(int width, int height, int channels, void *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	// TODO: May need to add size of stuff

	// TODO: Move to member?
	size_t pImageBuffer_n = sizeof(unsigned char) * pBuffer_n;
	m_pImageBuffer = (unsigned char*)malloc(pImageBuffer_n);
	CN(m_pImageBuffer);

	memcpy(m_pImageBuffer, pBuffer, pImageBuffer_n);

Error:
	return r;
}

unsigned char *image::GetImageBuffer() {
	return m_pImageBuffer;
}

size_t image::GetImageBufferSize() {
	return m_pImageBuffer_n;
}

int image::GetWidth() {
	return m_width;
}

int image::GetHeight() {
	return m_height;
}

int image::GetChannels() {
	return m_channels;
}

PIXEL_FORMAT image::GetPixelFormat() {
	switch (m_channels) {
		case 1: {
			return PIXEL_FORMAT::GREYSCALE;
		} break;

		case 3: {
			return PIXEL_FORMAT::BGR;
		} break;

		case 4: {
			return PIXEL_FORMAT::BGRA;
		} break;

	}

	return PIXEL_FORMAT::INVALID;
}