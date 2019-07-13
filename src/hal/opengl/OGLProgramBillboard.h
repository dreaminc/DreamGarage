#ifndef OGLPROGRAM_BILLBOARD_H_
#define OGLPROGRAM_BILLBOARD_H_

#include "./RESULT/EHM.h"
#include "OGLProgramMinimal.h"

class ObjectStore;
class stereocamera;

class OGLProgramBillboard : public OGLProgramMinimal {
public:
	OGLProgramBillboard(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	virtual RESULT ProcessNode(long frameID) override;

	virtual RESULT OGLInitialize() override;
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) override;
	virtual RESULT SetCameraUniforms(camera *pCamera) override;

	virtual RESULT SetObjectUniforms(DimObj *pDimObj) override;

	virtual RESULT SetObjectTextures(OGLObj *pOGLObj) override;

protected:
	OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewMatrix = nullptr;

	OGLUniformPoint *m_pUniformCameraOrigin = nullptr;
	OGLUniform *m_pUniformViewWidth = nullptr;
	OGLUniform *m_pUniformViewHeight = nullptr;

	OGLUniformBool *m_pUniformHasTextureColor = nullptr;
	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;

	OGLMaterialBlock *m_pMaterialsBlock = nullptr;
};


#endif // ! OGLPROGRAM_BILLBOARD_H_