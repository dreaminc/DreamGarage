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
	virtual RESULT SetClippingViewMatrix(ViewMatrix matView) override;
	virtual RESULT SetClippingFrustrum(float left, float right, float top, float bottom, float nearPlane, float farPlane) override;
	virtual RESULT SetClippingFrustrum(float width, float height, float nearPlane, float farPlane, float angle) override;

	virtual RESULT SetOriginPoint(point ptOrigin) override;
	virtual RESULT SetOriginDirection(vector vOrigin) override;

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

	// quad uniforms for clip
	OGLUniformPoint *m_pUniformQuadCenter;
	OGLUniformMatrix4 *m_pUniformParentModelMatrix;

	OGLUniformPoint *m_pUniformptOrigin;
	OGLUniformVector *m_pUniformvOrigin;

	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_UI_STAGE_H_