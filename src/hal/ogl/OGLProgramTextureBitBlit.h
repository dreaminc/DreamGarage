#ifndef OGLPROGRAM_TEXTURE_BIT_BLIT_H_
#define OGLPROGRAM_TEXTURE_BIT_BLIT_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramTextureBitBlit.h
// OGLProgramTextureBitBlit  is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader with a color texture that simply takes in a point, color, and UV coord
// and renders it using the usual suspects of required matrices (no lights)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramTextureBitBlit : public OGLProgram {
public:
	OGLProgramTextureBitBlit(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;
	virtual RESULT SetupConnections() override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;
	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;

protected:
	stereocamera *m_pCamera = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	//OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasTexture;
};

#endif // ! OGLPROGRAM_TEXTURE_BIT_BLIT_H_