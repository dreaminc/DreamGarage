#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/texture.h
// Base type for texture

#include "valid.h"
#include "Types/UID.h"

#define NUM_CUBE_MAP_TEXTURES 6

class texture : public valid {
public:
	// The texture type and channel
	enum class TEXTURE_TYPE {
		TEXTURE_COLOR = 0,
		TEXTURE_BUMP = 1,
		TEXTURE_CUBE = 2,
		TEXTURE_INVALID = 32
	};

public:
	texture();
	texture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight);
	~texture();

	// TODO: There's a redundancy with number/type that should be resolved
	// Texture number should be resolved on OGLTexture side - type held in texture
	RESULT SetTextureType(texture::TEXTURE_TYPE textureType);
	texture::TEXTURE_TYPE GetTextureType();
	
	//int GetTextureNumber();
	//RESULT SetTextureNumber(int texNum);

	RESULT GetTextureFilePath(const wchar_t *pszFilename, wchar_t * &n_pszFilePath);
	RESULT FlipTextureVertical();
	RESULT ReleaseTextureData();
	RESULT LoadTextureFromPath(wchar_t *pszFilepath);
	RESULT LoadTextureFromFile(wchar_t *pszFilename);
	RESULT LoadCubeMapFromFiles(wchar_t *pszFilenameFront, wchar_t *pszFilenameBack, wchar_t *pszFilenameTop, wchar_t *pszFilenameBottom, wchar_t *pszFilenameLeft, wchar_t *pszFilenameRight);

protected:
	int m_width;
	int m_height;
	int m_channels;

	unsigned char *m_pImageBuffer;

	//int m_textureNumber;
	TEXTURE_TYPE m_type;

private:
	UID m_uid;
};

#endif // ! TEXTURE_H_
