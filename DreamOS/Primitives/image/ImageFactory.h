#ifndef IMAGE_FACTORY_H_
#define IMAGE_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/image/image.h
// Base type for image which represents an array
// of pixels 

#include <string>

#include "image.h"

typedef enum {
	IMAGE_FREEIMAGE,
	IMAGE_INVALID
} IMAGE_TYPE;

class ImageFactory {
public:
	static image* MakeImageFromPath(IMAGE_TYPE type, std::wstring wstrImageFilepath);
	static image* MakeImageFromMemory(IMAGE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n);
};

#endif // !IMAGE_FACTORY_H_