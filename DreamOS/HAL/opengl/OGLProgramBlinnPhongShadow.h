#ifndef OGLPROGRAM_BLINN_PHONG_SHADOW_H_
#define OGLPROGRAM_BLINN_PHONG_SHADOW_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramBlinnPhongShadow.h
// OGLProgramBlinnPhongShadow is an OGL Program that utilizes the Blinn Phong
// algorithm and also employs shadow mapping

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

#include "Primitives/matrix/BiasMatrix.h"
#include "OpenGLImp.h"
#include "OGLProgramShadowDepth.h"

class OGLProgramBlinnPhongShadow : public OGLProgram {
public:
	OGLProgramBlinnPhongShadow(OpenGLImp *pParentImp);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetLights(std::vector<light*> *pLights);
	RESULT SetMaterial(material *pMaterial);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

	OGLFramebuffer *m_pInputFramebufferShadowDepth = nullptr;

private:
	// Vertex Attribute
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeVector *m_pVertexAttributeNormal;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;
	OGLVertexAttributeVector *m_pVertexAttributeTangent;
	OGLVertexAttributeVector *m_pVertexAttributeBitangent;

	// Uniforms
	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformDepthViewProjectionMatrix;

	OGLUniformPoint *m_pUniformObjectCenter;
	OGLUniformPoint *m_pUniformEyePosition;

	// Booleans
	OGLUniformBool *m_pUniformfBillboard;
	OGLUniformBool *m_pUniformfScale;

	// Textures
	OGLUniformSampler2D *m_pUniformTextureDepth;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_SHADOW_H_
