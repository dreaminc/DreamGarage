#ifndef IMAGE_H_
#define IMAGE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/image/image.h
// Base type for image which represents an array
// of pixels 

#include "Primitives/DObject.h"

#include <string>

class image : public DObject {
public:
	image(std::wstring wstrFilename);
	image(uint8_t *pBuffer, size_t pBuffer_n);

protected:
	image(int width, int height, int channels);

public:
	~image();

	virtual RESULT LoadFromPath() = 0;
	virtual RESULT LoadFromMemory() = 0;
	virtual RESULT Release() { return R_NOT_IMPLEMENTED; };
	virtual RESULT LoadImage() { return R_NOT_IMPLEMENTED; };

	RESULT FlipVertical();
	
	double GetAverageValueAtUV(double uValue, double vValue);

	RESULT CopyBuffer(int width, int height, int channels, void *pBuffer, size_t pBuffer_n);

	unsigned char *GetImageBuffer();
	size_t GetImageBufferSize();

	int GetWidth();
	int GetHeight();
	int GetChannels();

protected:
	std::wstring m_wstrFilename;

	// TODO: This may be temporary
	uint8_t *m_pSourceBuffer = nullptr;
	size_t m_pSourceBuffer_n = 0;

	int m_width = 0;
	int m_height = 0;
	int m_scanWidth = 0;
	int m_channels = 0;
	int m_bitsPerPixel = 0;
	// TODO: IMAGE FORMAT

	unsigned char *m_pImageBuffer = nullptr;
	size_t m_pImageBuffer_n = 0;
};

#endif // !IMAGE_H_