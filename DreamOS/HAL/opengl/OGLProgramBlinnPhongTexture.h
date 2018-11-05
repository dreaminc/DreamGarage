#ifndef OGLPROGRAM_BLINN_PHONG_TEXTURE_H_
#define OGLPROGRAM_BLINN_PHONG_TEXTURE_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramBlinnPhongTexture.h
// Blinn Phong Texture OGL shader program - this uses the blinn phong
// shading model along with one color texture

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramBlinnPhongTexture : public OGLProgram {
public:
	OGLProgramBlinnPhongTexture(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;

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
	//OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;

	OGLUniformBool *m_pUniformUseColorTexture;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_H_