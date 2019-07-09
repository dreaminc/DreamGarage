#ifndef CUBE_MAP_H_
#define CUBE_MAP_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/cubemap.h
// Base type for cubemap

#include "texture.h"

#define NUM_CUBE_MAP_TEXTURES 6

class cubemap : public texture {
public:
	enum class texture_type {
		CUBE_MAP_POS_X = 0,
		CUBE_MAP_NEG_X = 1,
		CUBE_MAP_POS_Y = 3,
		CUBE_MAP_NEG_Y = 2,
		CUBE_MAP_POS_Z = 5,
		CUBE_MAP_NEG_Z = 4,
		CUBE_MAP_INVALID
	};

public:
	cubemap();
	cubemap(wchar_t * pszName, std::vector<std::wstring> cubeMapFiles);

	size_t GetCubeMapSize();

	//RESULT GetCubeMapFilePath(const wchar_t *pszName, wchar_t * &n_pszFilePath);
	RESULT GetCubeMapFiles(const wchar_t *pszName, std::vector<std::wstring> &vstrFiles);

	//RESULT LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight);
	RESULT LoadCubeMapFromFiles(const wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles);
	RESULT LoadCubeMapByName(const wchar_t * pszName);

	static texture_type GetCubeMapTypeFromFilename(std::wstring strFilename);

protected:
	//uint8_t *m_pImageBuffer = nullptr;
	//size_t m_pImageBuffer_n = 0;

	image *m_pCubemapImages[NUM_CUBE_MAP_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

};

#endif // ! CUBE_MAP_H_