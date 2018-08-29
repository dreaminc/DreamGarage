#ifndef SKY_BOX_H_
#define SKY_BOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/skybox.h
// Skybox Primitive

#include "volume.h"
#include "texture.h"

// TODO: Not sure that this needs to be an explicit primitive 

#define DEFAULT_SKYBOX_SIZE 1000.0f

class skybox : public volume {

protected:
	texture *m_pCubeMapTexture;

public:
	skybox() :
		volume(DEFAULT_SKYBOX_SIZE),
		m_pCubeMapTexture(nullptr)
	{
		// empty
	}

	RESULT SetCubeMapTexture(texture *pTexture) {
		RESULT r = R_PASS;

		CBM((m_pCubeMapTexture == nullptr), "Cannot overwrite cube map texture");

		m_pCubeMapTexture = pTexture;
		m_pCubeMapTexture->SetTextureType(texture::TEXTURE_TYPE::TEXTURE_CUBE);

	Error:
		return r;
	}

	RESULT ClearCubeMapTexture() {
		RESULT r = R_PASS;

		CB((m_pCubeMapTexture != nullptr));
		m_pCubeMapTexture = nullptr;

	Error:
		return r;
	}
};

#endif	// ! SKY_BOX_H_
