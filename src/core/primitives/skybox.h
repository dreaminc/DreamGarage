#ifndef SKY_BOX_H_
#define SKY_BOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/skybox.h
// Skybox Primitive

#include "volume.h"

#include "cubemap.h"

// TODO: Not sure that this needs to be an explicit primitive 

#define DEFAULT_SKYBOX_SIZE 1000.0f

class skybox : public volume {

public:
	skybox() :
		volume(DEFAULT_SKYBOX_SIZE),
		m_pCubemap(nullptr)
	{
		// empty
	}

	RESULT SetCubeMapTexture(cubemap *pCubemap) {
		RESULT r = R_PASS;

		CBM((m_pCubemap == nullptr), "Cannot overwrite cube map texture");

		m_pCubemap = pCubemap;

	Error:
		return r;
	}

	RESULT ClearCubeMapTexture() {
		RESULT r = R_PASS;

		CB((m_pCubemap != nullptr));
		m_pCubemap = nullptr;

	Error:
		return r;
	}

protected:
	cubemap * m_pCubemap;
};

#endif	// ! SKY_BOX_H_
