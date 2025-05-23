#ifndef OGLPROGRAM_FLAT_H_
#define OGLPROGRAM_FLAT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramFlat.h

#include "OGLProgram.h"

#include "OGLFramebuffer.h"

#include "hal/FlatProgram.h"

class OGLProgramFlat : public OGLProgram, public FlatProgram {
public:
	OGLProgramFlat(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID = 0) override;

	// FlatProgram
	virtual RESULT RenderFlatContext(FlatContext *pFlatContext) override;
	virtual RESULT SetFlatFramebuffer(framebuffer *pFramebuffer) override;
	virtual RESULT SetCamera(stereocamera *pCamera) override;
	virtual RESULT SetFlatContext(FlatContext *pFlatContext) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

private:
	stereocamera *m_pCamera = nullptr;
	FlatContext *m_pFlatContext = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasColorTexture;

	OGLUniform *m_pUniformBuffer;
	OGLUniform *m_pUniformGamma;
	OGLUniformBool *m_pUniformfDistanceMap;

};

#endif // ! OGLPROGRAM_FLAT_H_
