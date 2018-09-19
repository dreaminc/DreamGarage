#ifndef OGLPROGRAM_SSAO_H_
#define OGLPROGRAM_SSAO_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSSAO.h
// SSAO Shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLQuad;

class OGLProgramSSAO : public OGLProgram {
public:
	OGLProgramSSAO(OpenGLImp *pParentImp, std::string strName = "oglssao");

	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

private:
	OGLFramebuffer *m_pOGLFramebufferInput = nullptr;

private:
	OGLQuad *m_pScreenQuad = nullptr;
	bool m_fRenderDepth = false;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformSampler2D *m_pUniformColorTexture = nullptr;
	OGLUniformSampler2D *m_pUniformColorTextureMS = nullptr;

	OGLUniformBool *m_pFUniformTextureMS = nullptr;
	OGLUniformInt *m_pUniformColorTextureMS_n = nullptr;

	OGLUniformFloat *m_pUniformWindowWidth = nullptr;
	OGLUniformFloat *m_pUniformWindowHeight = nullptr;

	OGLUniformVector *m_pUniformBackgroundColor = nullptr;
};

#endif // ! OGLPROGRAM_SSAO_H_