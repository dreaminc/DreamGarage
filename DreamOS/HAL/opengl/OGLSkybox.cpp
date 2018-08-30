#include "OGLSkybox.h"

OGLSkybox::OGLSkybox(OpenGLImp *pParentImp) :
	skybox(),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

/*
RESULT OGLSkybox::OGLActivateCubeMapTexture() {
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
RESULT OGLSkybox::Render() {
	RESULT r = R_PASS;

	bool fCullingEnabled = glIsEnabled(GL_CULL_FACE) != 0;

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	CR(OGLObj::Render());

Error:
	glDepthMask(GL_TRUE);

	if (fCullingEnabled)
		glEnable(GL_CULL_FACE);

	return r;
}
//*/