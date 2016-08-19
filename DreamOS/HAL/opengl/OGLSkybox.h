#ifndef OGL_SKYBOX_H_
#define OGL_SKYBOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLSkybox.h
// OGL Skybox

#include "OGLObj.h"
#include "OGLTexture.h"
#include "Primitives/skybox.h"

class OGLSkybox : public skybox, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLSkybox(OpenGLImp *pParentImp) :
		skybox(),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	RESULT OGLActivateCubeMapTexture() {
		RESULT r = R_PASS;

		OGLTexture *pCubeMapTexture = reinterpret_cast<OGLTexture*>(m_pCubeMapTexture);
		CN(pCubeMapTexture);
		CR(pCubeMapTexture->OGLActivateTexture());

	Error:
		return r;
	}

	// Face culling
	
	///*
	//RESULT Render() {
	RESULT Render() {
		RESULT r = R_PASS;
		bool fCullingEnabled = glIsEnabled(GL_CULL_FACE)!=0;

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		CR(OGLObj::Render());

	Error:
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		if(fCullingEnabled)
			glEnable(GL_CULL_FACE);

		return r;
	}

	//*/
};

#endif	// ! OGL_SKYBOX_H_