#ifndef OGLPROGRAM_MINIMAL_TEXTURE_H_
#define OGLPROGRAM_MINIMAL_TEXTURE_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramMinimalTexture.h
// OGLProgramMinimalTexture  is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader with a color texture that simply takes in a point, color, and UV coord
// and renders it using the usual suspects of required matrices (no lights)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramMinimalTexture : public OGLProgram {
public:
	OGLProgramMinimalTexture(OpenGLImp *pParentImp);

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
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformBool *m_pUniformHasTextureColor;
	OGLUniformSampler2D *m_pUniformTextureColor;

	OGLMaterialBlock *m_pMaterialsBlock;

	bool m_fPassThru = false;
};

#endif // ! OGLPROGRAM_MINIMAL_TEXTURE_H_