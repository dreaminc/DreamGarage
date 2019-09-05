#ifndef OGLPROGRAM_BLEND_QUAD_H_
#define OGLPROGRAM_BLEND_QUAD_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLBlendQuad.h

// OGL Blend Quad program will allow multiple the output to be blended
// from the inputs

#include "OGLProgram.h"

class OGLQuad;

class OGLProgramBlendQuad : public OGLProgram {
public:
	OGLProgramBlendQuad(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

	RESULT ClearFrameBuffer();

private:
	OGLFramebuffer *m_pOGLFramebufferInput = nullptr;

private:
	OGLQuad *m_pQuad = nullptr;
	int m_pass = 0;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;
	OGLUniformVector *m_pUniformBackgroundColor = nullptr;
};

#endif // ! OGLPROGRAM_BLEND_QUAD_H_