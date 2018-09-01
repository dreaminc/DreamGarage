#ifndef OGLPROGRAM_SKYBOX_SCATTER_CUBE_H_
#define OGLPROGRAM_SKYBOX_SCATTER_CUBE_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkyboxCube.h

#include "OGLProgram.h"
#include "../SkyboxScatterProgram.h"

#include "Primitives/matrix/RotationMatrix.h"

class cubemap;
class skybox;

class OGLProgramSkyboxScatterCube : public OGLProgram, public SkyboxScatterProgram {
public:
	OGLProgramSkyboxScatterCube(OpenGLImp *pParentImp);

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;

	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;

	RESULT SetCameraUniforms(GLenum glCubeMapFace, int pxWidth, int pxHeight);

// SkyboxScatterProgram
public:
	RESULT SetSunDirection(vector vSunDirection) override;

private:
	float m_SunY = 0.25f;
	float m_theta = 0.0f;
	float m_delta = 0.001f;	//0.00005f;

	skybox *m_pSkybox = nullptr;

private:
	stereocamera *m_pCamera = nullptr;
	
	OGLFramebuffer* m_pOGLFramebufferCubemap = nullptr;
	cubemap* m_pOutputCubemap = nullptr;

	//vector m_sunDirection = vector(0.0f, 1.0f, 1.0f);
	vector m_sunDirection = vector(0.0f, 0.0f, 1.0f);

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformViewOrientationMatrix;

	OGLUniform *m_pUniformViewWidth;
	OGLUniform *m_pUniformViewHeight;
	OGLUniformVector *m_pUniformSunDirection;
};
#endif // ! OGLPROGRAM_SKYBOX_SCATTER_CUBE_H_
