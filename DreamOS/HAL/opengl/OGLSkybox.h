#ifndef OGL_SKYBOX_H_
#define OGL_SKYBOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLSkybox.h
// OGL Skybox
// TODO: Remove this potentially 

#include "OGLObj.h"
#include "OGLTexture.h"
#include "Primitives/skybox.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
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

	/*
	RESULT OGLActivateCubeMapTexture() {
		RESULT r = R_PASS;

		OGLTexture *pCubeMapTexture = reinterpret_cast<OGLTexture*>(m_pCubeMapTexture);

		CN(pCubeMapTexture);
		CR(pCubeMapTexture->OGLActivateTexture());

	Error:
		return r;
	}
	*/

	// Face culling
	
	///*
	//RESULT Render() {
	RESULT Render() {
		RESULT r = R_PASS;
		bool fCullingEnabled = glIsEnabled(GL_CULL_FACE)!=0;

		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);

		CR(OGLObj::Render());

	Error:
		glDepthMask(GL_TRUE);

		if(fCullingEnabled)
			glEnable(GL_CULL_FACE);

		return r;
	}
	//*/
};
#pragma warning(pop)

#endif	// ! OGL_SKYBOX_H_