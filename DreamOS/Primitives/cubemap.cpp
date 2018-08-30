#include "cubemap.h"

#include "Sandbox/PathManager.h"

//#include <string>
#include <locale>
#include <codecvt>
#include <utility>
#include <regex>

#include "Primitives/image/ImageFactory.h"

cubemap::cubemap() :
	texture()
{
	// empty
}

cubemap::cubemap(wchar_t *pszName, std::vector<std::wstring> cubeMapFiles)
{
	RESULT r = R_PASS;

	CR(LoadCubeMapFromFiles(pszName, cubeMapFiles));

	Validate();
	return;
Error:
	Invalidate();
	return;
}

RESULT cubemap::GetCubeMapFiles(const wchar_t *pszName, std::vector<std::wstring> &vstrFiles) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	std::vector<std::wstring> vstrPathFiles;

	CRM(pPathManager->GetFilesForNameInPath(PATH_TEXTURE_CUBE, pszName, vstrPathFiles), "Failed to get files for %S cube map", pszName);

	// Filter out only the names - note: this will fail with unicode
	for (auto &strFilename : vstrPathFiles) {
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> StringConverter;
		std::string strFilenameConverted = StringConverter.to_bytes(strFilename);

		std::regex strRegEx("((pos|neg)(x|y|z))\\.(([a-z]){3,3})");

		if (std::regex_match(strFilenameConverted, strRegEx))
			vstrFiles.push_back(strFilename);
	}

Error:
	return r;
}

RESULT cubemap::LoadCubeMapByName(const wchar_t *pszName) {
	RESULT r = R_PASS;

	std::vector<std::wstring> vstrCubeMapFiles;
	PathManager *pPathManager = PathManager::instance();

	CR(GetCubeMapFiles(pszName, vstrCubeMapFiles));

	CR(LoadCubeMapFromFiles(pszName, vstrCubeMapFiles));

	CN(m_pImage);

Error:
	return r;
}

cubemap::texture_type cubemap::GetCubeMapTypeFromFilename(std::wstring strFilename) {
	cubemap::texture_type retType = cubemap::texture_type::CUBE_MAP_INVALID;

	std::wstring strFace = strFilename.substr(0, 3);
	wchar_t wchAxis = tolower(strFilename[3]);

	switch (wchAxis) {
	case 'x': {
		if (strFace == L"pos")
			retType = cubemap::texture_type::CUBE_MAP_POS_X;
		else if (strFace == L"neg")
			retType = cubemap::texture_type::CUBE_MAP_NEG_X;
	} break;

	case 'y': {
		if (strFace == L"pos")
			retType = cubemap::texture_type::CUBE_MAP_POS_Y;
		else if (strFace == L"neg")
			retType = cubemap::texture_type::CUBE_MAP_NEG_Y;
	} break;

	case 'z': {
		if (strFace == L"pos")
			retType = cubemap::texture_type::CUBE_MAP_POS_Z;
		else if (strFace == L"neg")
			retType = cubemap::texture_type::CUBE_MAP_NEG_Z;
	} break;
	}

	return retType;
}

//RESULT texture::LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight) {
RESULT cubemap::LoadCubeMapFromFiles(const wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles) {
	RESULT r = R_PASS;

	return R_FAIL;

	// TODO: Fix cube maps

	/*
	PathManager *pPathManager = PathManager::instance();
	uint8_t *pBuffers[NUM_CUBE_MAP_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

	int widths[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int heights[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };
	int channels[NUM_CUBE_MAP_TEXTURES] = { 0, 0, 0, 0, 0, 0 };

	CBM((vstrCubeMapFiles.size() == 6), "LoadCubeMapFromFiles expects 6 files to be provided only %llu found", vstrCubeMapFiles.size());

	for (auto &strFilename : vstrCubeMapFiles) {
	std::wstring strFilePath;
	int CubeMapFace = static_cast<int>(GetCubeMapTypeFromFilename(strFilename));
	CRM(pPathManager->GetFilePathForName(PATH_TEXTURE_CUBE, pszName, strFilename, strFilePath), "Failed to get %S path for %S cube map", pszName, strFilename.c_str());

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wstrConverter;
	std::string strFilePathConverted = wstrConverter.to_bytes(strFilePath);

	pBuffers[CubeMapFace] = SOIL_load_image(strFilePathConverted.c_str(), &widths[CubeMapFace], &heights[CubeMapFace], &channels[CubeMapFace], SOIL_LOAD_AUTO);
	CN(pBuffers[CubeMapFace]);

	// Ensure all heights are the same
	if(m_width == 0) {
	m_width = widths[CubeMapFace];
	m_height = heights[CubeMapFace];
	m_channels = channels[CubeMapFace];
	}
	else {
	CBM((m_width == widths[CubeMapFace]), "Cube map width %d mismatches %d", widths[CubeMapFace], m_width);
	CBM((m_height == heights[CubeMapFace]), "Cube map height %d mismatches %d", heights[CubeMapFace], m_height);
	CBM((m_channels == channels[CubeMapFace]), "Cube map channels %d mismatches %d", channels[CubeMapFace], m_channels);
	}
	}

	// Stitch it together here
	size_t sizeSide = GetTextureSize();
	size_t sizeTexture = sizeSide * NUM_CUBE_MAP_TEXTURES;
	m_pImageBuffer = new unsigned char[sizeTexture];
	CNM(m_pImageBuffer, "Failed to allocate Image Buffer for cube map");

	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
	unsigned char *ptrOffset = m_pImageBuffer + i * (sizeSide);
	memcpy(ptrOffset, pBuffers[i], sizeSide);
	}

	Error:
	for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
	if(pBuffers[i] != nullptr) {
	delete[] pBuffers[i];
	pBuffers[i] = nullptr;
	}
	}

	return r;
	*/
}

size_t cubemap::GetCubeMapSize() {
	return GetTextureSize() * NUM_CUBE_MAP_TEXTURES;
}