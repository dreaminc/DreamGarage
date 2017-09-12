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

	// Clipping Projection
	float m_left = -2.0f;
	float m_right = 2.0f;
	float m_top = 2.0f;
	float m_bottom = -2.0f;
	float m_nearPlane = -2.0f;
	float m_farPlane = 2.0f;

	OGLUniformBool *m_pUniformClippingEnabled;
	OGLUniformMatrix4 *m_pUniformClippingProjection;

	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_UI_STAGE_H_