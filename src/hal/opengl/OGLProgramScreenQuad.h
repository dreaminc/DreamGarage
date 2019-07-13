#ifndef OGLPROGRAM_SCREEN_QUAD_H_
#define OGLPROGRAM_SCREEN_QUAD_H_

// Dream OS
// DreamOS/HAL/opengl/OGLScreenQuad.h
// OGLProgramMinimal is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader that simply takes in a vertex point and color
// and renders it using the usual suspects of required matrices (no lights, no textures)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLQuad;

class OGLProgramScreenQuad : public OGLProgram {
public:
	OGLProgramScreenQuad(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	OGLProgramScreenQuad(OpenGLImp *pParentImp, std::string strName = "oglscreenquad", PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

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

#endif // ! OGLPROGRAM_MINIMAL_H_