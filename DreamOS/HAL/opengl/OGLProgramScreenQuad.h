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
	OGLProgramScreenQuad(OpenGLImp *pParentImp);

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

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;
	OGLUniformVector *m_pUniformBackgroundColor = nullptr;
};

#endif // ! OGLPROGRAM_MINIMAL_H_