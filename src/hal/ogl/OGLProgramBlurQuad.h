#ifndef OGLPROGRAM_BLUR_QUAD_H_
#define OGLPROGRAM_BLUR_QUAD_H_

// Dream OS
// DreamOS/HAL/opengl/OGLBlurQuad.h
// A simple quad based blur pass program

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLQuad;

class OGLProgramBlurQuad : public OGLProgram {
public:
	OGLProgramBlurQuad(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();

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

	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;
	OGLUniformVector *m_pUniformBackgroundColor = nullptr;
};

#endif // ! OGLPROGRAM_MINIMAL_H_