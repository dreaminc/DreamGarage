#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"
#include <vector>

#define NUM_CUBE_MAP_TEXTURES 6

class texture : public valid {
public:
	// The texture type and channel
	enum class TEXTURE_TYPE {
		TEXTURE_COLOR = 0,
		TEXTURE_BUMP = 1,
		TEXTURE_CUBE = 2,
		TEXTURE_HEIGHT = 3,
		TEXTURE_INVALID = 32
	};

	enum class CUBE_MAP {
		CUBE_MAP_POS_X = 0,
		CUBE_MAP_NEG_X = 1,
		CUBE_MAP_POS_Y = 3,
		CUBE_MAP_NEG_Y = 2,
		CUBE_MAP_POS_Z = 5,
		CUBE_MAP_NEG_Z = 4,
		CUBE_MAP_INVALID 
	};

	enum class PixelFormat {
		Unspecified, // this will generate an RGB/RGBA based on the number of channels
		RGB,
		RGBA,
		BGR,
		BGRA
	};

public:
	texture();
	texture(texture::TEXTURE_TYPE type);
	texture(texture::TEXTURE_TYPE type, int width, int height, int channels);
	texture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n);
	texture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture(wchar_t * pszName, std::vector<std::wstring> cubeMapFiles);
	~texture();

	// TODO: There's a redundancy with number/type that should be resolved
	// Texture number should be resolved on OGLTexture side - type held in texture
	RESULT SetTextureType(texture::TEXTURE_TYPE textureType);
	texture::TEXTURE_TYPE GetTextureType();
	size_t GetTextureSize();
	size_t GetCubeMapSize();
	
	//int GetTextureNumber();
	//RESULT SetTextureNumber(int texNum);

	RESULT GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath);
	//RESULT GetCubeMapFilePath(const wchar_t *pszName, wchar_t * &n_pszFilePath);
	RESULT GetCubeMapFiles(const wchar_t *pszName, std::vector<std::wstring> &vstrFiles);

	RESULT FlipTextureVertical();
	RESULT ReleaseTextureData();

	RESULT LoadTextureFromPath(wchar_t *pszFilepath);
	RESULT LoadTextureFromFile(wchar_t *pszFilename);
	//RESULT LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight);
	RESULT LoadCubeMapFromFiles(wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles);
	RESULT LoadCubeMapByName(wchar_t * pszName);
	RESULT CopyTextureBuffer(int width, int height, int channels, void *pBuffer, int pBuffer_n);

	virtual RESULT Update(unsigned char* pixels, int width, int height, texture::PixelFormat format);

	static CUBE_MAP GetCubeMapTypeFromFilename(std::wstring strFilename);

	double GetValueAtUV(double uValue, double vValue);

	int GetWidth() {
		return m_width;
	}

	int GetHeight() {
		return m_height;
	}

	int GetChannels() {
		return m_channels;
	}

	PixelFormat GetPixelFormat() {
		return m_format;
	}

	RESULT SetWidth(int width) {
		if (m_width != NULL) {
			return R_FAIL;
		}
		else {
			m_width = width;
			return R_PASS;
		}
	}

	RESULT SetHeight(int height) {
		if (m_height != NULL) {
			return R_FAIL;
		}
		else {
			m_height = height;
			return R_PASS;
		}
	}

	RESULT SetChannels(int channels) {
		if (m_channels != NULL) {
			return R_FAIL;
		}
		else {
			m_channels = channels;
			return R_PASS;
		}
	}

	RESULT SetFormat(PixelFormat format) {
		m_format = format;
		return R_PASS;
	}

protected:
	int m_width;
	int m_height;
	int m_channels;
	PixelFormat	m_format = PixelFormat::Unspecified;

	unsigned char *m_pImageBuffer;

	//int m_textureNumber;
	TEXTURE_TYPE m_type;

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_
