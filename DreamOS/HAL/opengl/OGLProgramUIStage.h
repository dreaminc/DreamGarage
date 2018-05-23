#ifndef OGLPROGRAM_UI_STAGE_H_
#define OGLPROGRAM_UI_STAGE_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramUIStage.h

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"
#include "../UIStageProgram.h"

class OGLProgramUIStage : public OGLProgram, public UIStageProgram {
public:
	OGLProgramUIStage(OpenGLImp *pParentImp);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetMaterial(material *pMaterial);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

	//UIStageProgram
	virtual RESULT SetOriginPoint(point ptOrigin) override;
	virtual RESULT SetOriginDirection(vector vOrigin) override;

	virtual RESULT SetIsAugmented(bool fAugmented) override;
	virtual RESULT SetClippingThreshold(float clippingThreshold) override;
	virtual RESULT SetClippingRate(float clippingRate) override;

protected:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;
	ObjectStore *m_pClippingSceneGraph = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasTextureColor;

	OGLUniformBool *m_pUniformClippingEnabled;
	OGLUniformBool *m_pUniformAR;

	OGLUniformFloat *m_pUniformClippingThreshold;
	OGLUniformFloat *m_pUniformClippingRate;

	// quad uniforms for clip
	OGLUniformPoint *m_pUniformQuadCenter;
	OGLUniformMatrix4 *m_pUniformParentModelMatrix;

	OGLUniformPoint *m_pUniformptOrigin;
	OGLUniformVector *m_pUniformvOrigin;

	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_UI_STAGE_H_