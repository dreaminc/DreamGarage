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

class image;

class texture : public valid {
public:
	enum class flags : uint16_t {
		NONE = 0,
		DISTANCE_MAP = 1 << 0,
		UV_VERTICAL_FLIPPED = 1 << 1,
		INVALID			= 0xFFFF
	};
	

	// The texture type and channel
	enum class type {
		TEXTURE_2D,
		RECTANGLE,
		CUBE,
		INVALID
	};

public:
	texture();
	texture(const texture& tex);
	texture(texture::type texType);
	texture(texture::type texType, int width, int height, int channels, int samples = 0);
	texture(texture::type texType, int width, int height, int channels, void *pBuffer, int pBuffer_n, int samples = 0);
	texture(texture::type texType, int width, int height, PIXEL_FORMAT format, int channels, void *pBuffer, int pBuffer_n, int samples = 0);

	// Loads from a file buffer (file loaded into buffer)
	texture(texture::type texType, uint8_t *pBuffer, size_t pBuffer_n);

	texture(texture::type texType, wchar_t *pszFilename);
	
	~texture();

	texture::type GetTextureType() {
		return m_type;
	}

	size_t GetTextureSize();
	
	//int GetTextureNumber();
	//RESULT SetTextureNumber(int texNum);

	RESULT GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath);
	

	RESULT ReleaseTextureData();

	RESULT LoadTextureFromPath(const wchar_t *pszFilepath);
	RESULT LoadTextureFromFile(const wchar_t *pszFilename);
	RESULT LoadTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n);
	
	RESULT CopyTextureImageBuffer(int width, int height, int channels, void *pBuffer, size_t pBuffer_n);
	virtual RESULT LoadBufferFromTexture(void *pBuffer, size_t pBuffer_n);
	virtual RESULT LoadFlippedBufferFromTexture(void *pBuffer, size_t pBuffer_n);
	virtual RESULT UpdateTextureFromBuffer(void *pBuffer, size_t pBuffer_n);
	virtual RESULT UpdateTextureRegionFromBuffer(void *pBuffer, int x, int y, int width, int height);

	virtual RESULT LoadImageFromTexture(int level, PIXEL_FORMAT pixelFormat);

	virtual RESULT Update(unsigned char* pBuffer, int width, int height, PIXEL_FORMAT pixelFormat);
	virtual RESULT UpdateDimensions(int width, int height);	

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

	bool IsUVVerticalFlipped();
	RESULT SetUVVerticalFlipped();

	uint8_t *GetImageBuffer();

protected:
	PIXEL_FORMAT m_pixelFormat = PIXEL_FORMAT::INVALID;
	texture::type m_type = texture::type::INVALID;

	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	int m_samples = 0;
	int m_levels = 0;
	int m_bitsPerPixel = 8;

	flags m_flags = texture::flags::NONE;

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
