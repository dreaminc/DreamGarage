#ifndef FREE_IMAGE_OBJ_H_
#define FREE_IMAGE_OBJ_H_

#include "RESULT/EHM.h"

// Logging is redefining macros due to CEF, Logging++ and WebRTC
// When we solve logging we need to solve this too
#pragma warning( disable : 4005)

#include <FreeImagePlus.h>

// DREAM OS
// DreamOS/Dimension/Primitives/image/FreeImageObj.h
// The FreeImage Object file 

#include "image.h"
#include <string>

class FreeImageObj : public image {
public:
	FreeImageObj(std::wstring wstrFilename);
	FreeImageObj(uint8_t *pBuffer, size_t pBuffer_n);

	virtual RESULT LoadFromPath() override;
	virtual RESULT LoadFromMemory() override;
	virtual RESULT Release() override;

	virtual RESULT LoadImage() override;
	virtual PIXEL_FORMAT GetPixelFormat() override;

private:
	FREE_IMAGE_FORMAT m_fiImageFormat;
	
	FIBITMAP* m_pfiBitmap = nullptr;
	//FIBITMAP* m_pfiBitmap32 = nullptr;

	int m_fiBitsPerPixel;
	FREE_IMAGE_COLOR_TYPE m_fiColorType;
	BITMAPINFO m_fiHeaderInfo;

private:
	static bool m_fFreeImageInitialized;
	static RESULT InitializeFreeImage();

};

#endif // !FREE_IMAGE_OBJ_H_