#ifndef OGLPROGRAM_SKYBOX_SCATTER_H_
#define OGLPROGRAM_SKYBOX_SCATTER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "../SkyboxScatterProgram.h"

#include "Primitives/matrix/RotationMatrix.h"

class cubemap;

class OGLProgramSkyboxScatter : public OGLProgram, public SkyboxScatterProgram {
public:
	OGLProgramSkyboxScatter(OpenGLImp *pParentImp);

	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);

	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

public:
	RESULT SetReflectionObject(VirtualObj *pReflectionObject);

// SkyboxScatterProgram
public:
	RESULT SetSunDirection(vector vSunDirection) override;

private:
	float m_SunY = 0.25f;
	float m_theta = 0.0f;
	float m_delta = 0.001f;	//0.00005f;

	VirtualObj *m_pReflectionObject = nullptr;

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;
	OGLFramebuffer* m_pOGLFramebuffer = nullptr;
	OGLFramebuffer* m_pOGLFramebufferCubemap = nullptr;

	cubemap* m_pOutputCubemap = nullptr;

	vector m_sunDirection = vector(0.0f, 1.0f, 0.0f);

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
#endif // ! OGLPROGRAM_SKYBOX_SCATTER_H_
