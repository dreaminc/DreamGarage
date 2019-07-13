#ifndef OGLPROGRAM_GBUFFER_H_
#define OGLPROGRAM_GBUFFER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramGBuffer.h

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class ObjectStore;
class stereocamera;

class OGLProgramGBuffer : public OGLProgram {
public:
	OGLProgramGBuffer(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	OGLProgramGBuffer(OpenGLImp *pParentImp, std::string strName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;
	virtual RESULT SetMaterial(material *pMaterial) override;
	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;
	virtual RESULT SetCameraUniforms(camera *pCamera) override;
	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;

protected:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

protected:
	OGLVertexAttributePoint *m_pVertexAttributePosition = nullptr;
	OGLVertexAttributeColor *m_pVertexAttributeColor = nullptr;

	OGLUniformMatrix4 *m_pUniformModelMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix = nullptr;

	OGLMaterialBlock *m_pMaterialsBlock = nullptr;
};

#endif // ! OGLPROGRAM_GBUFFER_H_