#include "ImageFactory.h"

#include "FreeImageObj.h"
#include "MemoryImageObj.h"

image* ImageFactory::MakeImageFromPath(IMAGE_TYPE type, std::wstring wstrImageFilename) {
	RESULT r = R_PASS;

	image *pImage = nullptr;

	switch (type) {
		case IMAGE_FREEIMAGE: {
			pImage = new FreeImageObj(wstrImageFilename);
			CN(pImage);
		} break;

		// Currently no other supported formats 
		default: {
			pImage = nullptr;
		} break;
	}
	
	CR(pImage->LoadFromPath());

	return pImage;

Error:
	if (pImage != nullptr) {
		delete pImage;
		pImage = nullptr;
	}

	return nullptr;
}

image* ImageFactory::MakeImageFromMemory(IMAGE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n) {
	RESULT r = R_PASS;

	image *pImage = nullptr;

	switch (type) {
		case IMAGE_FREEIMAGE: {
			pImage = new FreeImageObj(pBuffer, pBuffer_n);
			CN(pImage);
		} break;

		// Currently no other supported formats 
		default: {
			pImage = nullptr;
		} break;
	}

	CR(pImage->LoadFromMemory());

	return pImage;

Error:
	if (pImage != nullptr) {
		delete pImage;
		pImage = nullptr;
	}

	return nullptr;
}

image* ImageFactory::MakeMemoryImage(IMAGE_TYPE type, int width, int height, int channels) {
	RESULT r = R_PASS;

	image *pImage = nullptr;

	switch (type) {
	case IMAGE_MEMORY: {
		pImage = new MemoryImageObj(width, height, channels);
		CN(pImage);
	} break;

		// Currently no other supported formats 
	default: {
		pImage = nullptr;
	} break;
	}

	CR(pImage->LoadFromMemory());

	return pImage;

Error:
	if (pImage != nullptr) {
		delete pImage;
		pImage = nullptr;
	}

	return nullptr;
}