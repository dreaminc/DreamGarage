#ifndef OGLPROGRAM_ENVIRONMENT_H_
#define OGLPROGRAM_ENVIRONMENT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramMinimalTexture.h

// OGLProgramMinimalTexture  is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader with a color texture that simply takes in a point, color, and UV coord
// and renders it using the usual suspects of required matrices (no lights)

#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

#include "hal/FogProgram.h"

class OGLFogParamsBlock;

class OGLProgramEnvironment: public OGLProgram, public FogProgram {
public:
	OGLProgramEnvironment(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetMaterial(material *pMaterial);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

protected:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition = nullptr;
	OGLVertexAttributeColor *m_pVertexAttributeColor = nullptr;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord = nullptr;

	OGLUniformMatrix4 *m_pUniformModelMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix = nullptr;

	OGLUniformBool *m_pUniformHasTextureColor = nullptr;
	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;

	OGLMaterialBlock *m_pMaterialsBlock = nullptr;
	OGLFogParamsBlock *m_pFogParamsBlock = nullptr;
};

#endif // ! OGLPROGRAM_ENVIRONMENT_H_