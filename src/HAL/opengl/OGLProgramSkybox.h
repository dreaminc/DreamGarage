#ifndef OGLPROGRAM_SKYBOX_H_
#define OGLPROGRAM_SKYBOX_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "OGLProgram.h"

class skybox;
class stereocamera;
class cubemap;

class OGLProgramSkybox : public OGLProgram {
public:
	OGLProgramSkybox(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;
	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;

public:
	RESULT SetReflectionObject(VirtualObj *pReflectionObject);

	// Temporary until we set up nodes
	RESULT SetCubemap(cubemap *pCubemap);

protected:
	stereocamera *m_pCamera = nullptr;
	skybox *m_pSkybox = nullptr;

	cubemap *m_pCubemap = nullptr;
	OGLFramebuffer* m_pOGLInputFramebufferCubemap = nullptr;

	VirtualObj *m_pReflectionObject = nullptr;

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

#endif // ! OGLPROGRAM_SKYBOX_H_