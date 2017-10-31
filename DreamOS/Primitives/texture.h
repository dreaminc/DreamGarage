#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"
#include <vector>

#include "color.h"

#define NUM_CUBE_MAP_TEXTURES 6

class image;

class texture : public valid {
public:
	enum class flags : uint16_t {
		NONE			= 0,
		DISTANCE_MAP	= 1 << 0,
		INVALID			= 0xFFFF
	};

	// The texture type and channel
	enum class TEXTURE_TYPE {
		TEXTURE_DIFFUSE,
		TEXTURE_BUMP,
		TEXTURE_SPECULAR,
		TEXTURE_AMBIENT,
		TEXTURE_CUBE,
		TEXTURE_HEIGHT,
		TEXTURE_DEPTH,
		TEXTURE_RECTANGLE,
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

public:
	texture();
	texture(const texture& tex);
	texture(texture::TEXTURE_TYPE type);
	texture(texture::TEXTURE_TYPE type, int width, int height, int channels, int samples = 0);
	texture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n, int samples = 0);
	texture(texture::TEXTURE_TYPE type, int width, int height, PIXEL_FORMAT format, int channels, void *pBuffer, int pBuffer_n, int samples = 0);

	// Loads from a file buffer (file loaded into buffer)
	texture(texture::TEXTURE_TYPE type, uint8_t *pBuffer, size_t pBuffer_n);

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

	RESULT ReleaseTextureData();

	RESULT LoadTextureFromPath(const wchar_t *pszFilepath);
	RESULT LoadTextureFromFile(const wchar_t *pszFilename);
	RESULT LoadTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n);
	//RESULT LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight);
	RESULT LoadCubeMapFromFiles(const wchar_t *pszName, std::vector<std::wstring> vstrCubeMapFiles);
	RESULT LoadCubeMapByName(const wchar_t * pszName);
	RESULT CopyTextureImageBuffer(int width, int height, int channels, void *pBuffer, size_t pBuffer_n);

	virtual RESULT LoadImageFromTexture(int level, PIXEL_FORMAT pixelFormat);

	virtual RESULT Update(unsigned char* pBuffer, int width, int height, PIXEL_FORMAT pixelFormat);
	virtual RESULT UpdateDimensions(int width, int height);

	static CUBE_MAP GetCubeMapTypeFromFilename(std::wstring strFilename);

	double GetAverageValueAtUV(double uValue, double vValue);

	int GetWidth() { return m_width; }
	int GetHeight() { return m_height; }
	int GetChannels() { return m_channels; }
	int GetSamples() { return m_samples; }
	int GetLevels() { return m_levels; }

	PIXEL_FORMAT GetPixelFormat() {
		return m_pixelFormat;
	}

	RESULT SetParams(int pxWidth, int pxHeight, int channels, int samples = 1, int levels = 0);

	RESULT SetWidth(int width) {
		if (m_width != 0) {
			return R_FAIL;
		}
		else {
			m_width = width;
			return R_PASS;
		}
	}

	RESULT SetHeight(int height) {
		if (m_height != 0) {
			return R_FAIL;
		}
		else {
			m_height = height;
			return R_PASS;
		}
	}

	RESULT SetChannels(int channels) {
		if (m_channels != 0) {
			return R_FAIL;
		}
		else {
			m_channels = channels;
			return R_PASS;
		}
	}

	RESULT SetSamples(int samples) {
		if (m_samples != 0) {
			return R_FAIL;
		}
		else {
			m_samples = samples;
			return R_PASS;
		}
	}

	RESULT SetLevels(int levels) {
		if (m_levels != 0) {
			return R_FAIL;
		}
		else {
			m_levels = levels;
			return R_PASS;
		}
	}

	RESULT SetFormat(PIXEL_FORMAT format) {
		m_pixelFormat = format;
		return R_PASS;
	}

	bool IsDistanceMapped();
	RESULT SetDistanceMapped();

	uint8_t *GetImageBuffer();

protected:
	PIXEL_FORMAT m_pixelFormat = PIXEL_FORMAT::Unspecified;
	TEXTURE_TYPE m_type;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	int m_samples = 0;
	int m_levels = 0;

	flags m_flags = texture::flags::NONE;

	//unsigned char *m_pImageBuffer = nullptr;
	image *m_pImage = nullptr;

private:
	UID m_uid;
};

inline constexpr texture::flags operator | (const texture::flags &lhs, const texture::flags &rhs) {
	return static_cast<texture::flags>(
		static_cast<std::underlying_type<texture::flags>::type>(lhs) | static_cast<std::underlying_type<texture::flags>::type>(rhs)
		);
}

inline constexpr texture::flags operator & (const texture::flags &lhs, const texture::flags &rhs) {
	return static_cast<texture::flags>(
		static_cast<std::underlying_type<texture::flags>::type>(lhs) & static_cast<std::underlying_type<texture::flags>::type>(rhs)
		);
}

#endif // ! TEXTURE_H_
