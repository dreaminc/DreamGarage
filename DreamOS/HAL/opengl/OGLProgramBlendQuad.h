#ifndef OGLPROGRAM_BLEND_QUAD_H_
#define OGLPROGRAM_BLEND_QUAD_H_

// Dream OS
// DreamOS/HAL/opengl/OGLBlendQuad.h
// OGL Blend Quad program will allow multiple the output to be blended
// from the inputs

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLQuad;

class OGLProgramBlendQuad : public OGLProgram {
public:
	OGLProgramBlendQuad(OpenGLImp *pParentImp);

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
	OGLQuad *m_pQuad = nullptr;
	bool m_fRenderDepth = false;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;
	OGLUniformVector *m_pUniformBackgroundColor = nullptr;
};

#endif // ! OGLPROGRAM_BLEND_QUAD_H_