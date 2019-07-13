#ifndef OGLPROGRAM_CUBEMAP_CONVOLUTION_H_
#define OGLPROGRAM_CUBEMAP_CONVOLUTION_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProgramCubemapConvolution.h
// OGLProgramConvolution is an OGLProgram that takes an input 
// cubemap and outputs a convolution cubemap

// TODO: 
// Will only run one time - similar to the scattering cube 
// this should be evolved to using pipeline flags 

#include "OGLProgram.h"

class skybox;
class stereocamera;
class cubemap;

class OGLProgramCubemapConvolution : public OGLProgram {
public:
	OGLProgramCubemapConvolution(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;
	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;

	// TODO: this is shared with scatter cube - should put into general 
	// functionality or subclass the two from a shared interface
	RESULT SetCameraUniforms(GLenum glCubeMapFace, int pxWidth, int pxHeight);

public:
	// Temporary until we set up nodes
	RESULT SetCubemap(cubemap *pCubemap);

protected:
	stereocamera *m_pCamera = nullptr;
	skybox *m_pSkybox = nullptr;

	cubemap *m_pCubemap = nullptr;
	OGLFramebuffer* m_pOGLInputFramebufferCubemap = nullptr;

	VirtualObj *m_pReflectionObject = nullptr;

	OGLFramebuffer* m_pOGLFramebufferCubemap = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition = nullptr;
	OGLVertexAttributeColor *m_pVertexAttributeColor = nullptr;

	//OGLUniformMatrix4 *m_pUniformModelMatrix;
	//OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	//OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	//OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformViewOrientationMatrix = nullptr;

	OGLUniformBool *m_pUniformHasTextureCubemap = nullptr;
	OGLUniformSamplerCube *m_pUniformTextureCubemap = nullptr;
};

#endif // ! OGLPROGRAM_CUBEMAP_CONVOLUTION_H_