#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"

class texture : public valid {
public:
	// The texture type and channel
	enum class TEXTURE_TYPE {
		TEXTURE_COLOR = 0,
		TEXTURE_BUMP = 1,
		TEXTURE_INVALID = 32
	};

public:
	texture();
	texture(wchar_t *pszFilename);
	~texture();

	// TODO: There's a redundancy with number/type that should be resolved
	RESULT SetTextureType(texture::TEXTURE_TYPE textureType);
	int GetTextureNumber();
	texture::TEXTURE_TYPE GetTextureType();
	RESULT SetTextureNumber(int texNum);

	RESULT GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath);
	RESULT FlipTextureVertical();
	RESULT ReleaseTextureData();
	RESULT LoadTextureFromPath(wchar_t *pszFilepath);
	RESULT LoadTextureFromFile(wchar_t *pszFilename);

protected:
	int m_width;
	int m_height;
	int m_channels;

	unsigned char *m_pImageBuffer;

	int m_textureNumber;

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_
