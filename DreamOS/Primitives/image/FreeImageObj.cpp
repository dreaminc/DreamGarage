#include "FreeImageObj.h"

#include <codecvt>

bool FreeImageObj::m_fFreeImageInitialized = false;

RESULT FreeImageObj::InitializeFreeImage() {
	RESULT r = R_PASS;

	CBR((m_fFreeImageInitialized == false), R_SKIPPED);

	//  ----- Initialize the FreeImage library -----
	// Note: Flag is whether we should load ONLY local (built-in) libraries, so
	// false means 'no, use external libraries also', and 'true' means - use built
	// in libs only, so it's like using the library as a static version of itself.

	FreeImage_Initialise(true);
	m_fFreeImageInitialized = true;

Error:
	return r;
}

FreeImageObj::FreeImageObj(std::wstring wstrFilename) :
	image(wstrFilename)
{
	RESULT r = R_PASS;

	Validate();
	return;

//Error:
	Invalidate();
	return;
}

FreeImageObj::FreeImageObj(uint8_t *pBuffer, size_t pBuffer_n) :
	image(pBuffer, pBuffer_n)
{
	RESULT r = R_PASS;

	Validate();
	return;

	//Error:
	Invalidate();
	return;
}

RESULT FreeImageObj::Release() {
	RESULT r = R_PASS;

	// Unload the 32-bit color bitmap
	if (m_pfiBitmap32 != nullptr) {
		if (m_pfiBitmap != m_pfiBitmap32 && m_pfiBitmap != nullptr) {
			FreeImage_Unload(m_pfiBitmap);
			m_pfiBitmap = nullptr;
		}

		FreeImage_Unload(m_pfiBitmap32);
		m_pfiBitmap32 = nullptr;
	}

	m_pImageBuffer = nullptr;

	//Error:
	return r;
}

RESULT FreeImageObj::LoadFromPath() {
	RESULT r = R_PASS;

	//std::wstring wstrFilepath(pszFilepath);
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
	std::string strFilepath = wstrConverter.to_bytes(m_wstrFilename);

	CR(InitializeFreeImage());

	m_fiImageFormat = FreeImage_GetFileType(strFilepath.c_str(), 0);
	CBM((m_fiImageFormat != -1), "Could not load %s", strFilepath.c_str());

	// Image found, but format not determined, try to use file extension
	// This is slower and more error prone than above method
	if (m_fiImageFormat == FIF_UNKNOWN) {
		m_fiImageFormat = FreeImage_GetFIFFromFilename(strFilepath.c_str());

		// Check FI support for the format, otherwise quit trying
		CBM((FreeImage_FIFSupportsReading(m_fiImageFormat)), "Detected image format cannot be read!");
	}

	m_pfiBitmap = FreeImage_Load(m_fiImageFormat, strFilepath.c_str());
	CN(m_pfiBitmap);

	m_fiBitsPerPixel = FreeImage_GetBPP(m_pfiBitmap);

	// Set to 32 bits per pixel (not sure if we want this)
	if (m_fiBitsPerPixel == 32) {
		m_pfiBitmap32 = m_pfiBitmap;
	}
	else {
		m_pfiBitmap32 = FreeImage_ConvertTo32Bits(m_pfiBitmap);
	}

	m_width = FreeImage_GetWidth(m_pfiBitmap32);
	m_height = FreeImage_GetHeight(m_pfiBitmap32);
	m_scanWidth = FreeImage_GetPitch(m_pfiBitmap32);
	m_channels = 4;	// TODO: This is guaranteed by the conversion to 32 bits above

	// Get a handle to the data buffer
	m_pImageBuffer = FreeImage_GetBits(m_pfiBitmap32);
	CN(m_pImageBuffer);

Error:
	return r;
}

RESULT FreeImageObj::LoadFromMemory() {
	RESULT r = R_PASS;

	CR(InitializeFreeImage());

	FIMEMORY *pfiMemory = FreeImage_OpenMemory(m_pSourceBuffer, (DWORD)(m_pSourceBuffer_n));
	CN(pfiMemory);

	// get the file type 
	m_fiImageFormat = FreeImage_GetFileTypeFromMemory(pfiMemory, 0);
	CBM((m_fiImageFormat != FIF_UNKNOWN), "Could not load image from memory");

	m_pfiBitmap = FreeImage_LoadFromMemory(m_fiImageFormat, pfiMemory, 0);
	CN(m_pfiBitmap);

	m_fiBitsPerPixel = FreeImage_GetBPP(m_pfiBitmap);

	///*
	// Set to 32 bits per pixel (not sure if we want this)
	if (m_fiBitsPerPixel == 32) {
		m_pfiBitmap32 = m_pfiBitmap;
	}
	else {
		m_pfiBitmap32 = FreeImage_ConvertTo32Bits(m_pfiBitmap);
	}
	//*/

	m_width = FreeImage_GetWidth(m_pfiBitmap32);
	m_height = FreeImage_GetHeight(m_pfiBitmap32);
	m_scanWidth = FreeImage_GetPitch(m_pfiBitmap32);
	m_channels = 4;	// TODO: This is guaranteed by the conversion to 32 bits above

	// Get a handle to the data buffer
	m_pImageBuffer = FreeImage_GetBits(m_pfiBitmap32);
	CN(m_pImageBuffer);

	// TODO: Do we still need the original buffer?

Error:
	return r;
}