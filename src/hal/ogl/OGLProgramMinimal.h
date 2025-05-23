#ifndef OGLPROGRAM_MINIMAL_H_
#define OGLPROGRAM_MINIMAL_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramMinimal.h

// OGLProgramMinimal is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader that simply takes in a vertex point and color
// and renders it using the usual suspects of required matrices (no lights, no textures)

#include "OGLProgram.h"

class ObjectStore;
class stereocamera;

class OGLProgramMinimal : public OGLProgram {
public:
	OGLProgramMinimal(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);
	OGLProgramMinimal(OGLImp *pParentImp, std::string strName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();

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

#endif // ! OGLPROGRAM_MINIMAL_H_