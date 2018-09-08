#ifndef OGLPROGRAM_BLINN_PHONG_TEXTURE_BUMP_DISPLACEMENT_H_
#define OGLPROGRAM_BLINN_PHONG_TEXTURE_BUMP_DISPLACEMENT_H_

#include "./RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProgramBlinnPhongTextureBumpDisplacement.h
// Blinn Phong Texture OGL shader program - this uses the blinn phong
// shading model along with one color texture and one bump map texture

#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramBlinnPhongTextureBumpDisplacement : public OGLProgram {
public:
	OGLProgramBlinnPhongTextureBumpDisplacement(OpenGLImp *pParentImp);

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
	OGLVertexAttributePoint *m_pVertexAttributePosition = nullptr;
	OGLVertexAttributeColor *m_pVertexAttributeColor = nullptr;
	OGLVertexAttributeVector *m_pVertexAttributeNormal = nullptr;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord = nullptr;
	OGLVertexAttributeVector *m_pVertexAttributeTangent = nullptr;
	OGLVertexAttributeVector *m_pVertexAttributeBitangent = nullptr;

	// Uniforms
	OGLUniformMatrix4 *m_pUniformModelMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewMatrix = nullptr;
	//OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix = nullptr;

	OGLUniformPoint *m_pUniformEye = nullptr;

	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;
	OGLUniformBool *m_pUniformHasColorTexture = nullptr;

	OGLUniformSampler2D *m_pUniformTextureBump = nullptr;
	OGLUniformBool *m_pUniformHasBumpTexture = nullptr;

	OGLUniformBool *m_pUniformHasDisplacementTexture = nullptr;
	OGLUniformSampler2D *m_pUniformTextureDisplacement = nullptr;

	OGLUniform *m_pUniformTime = nullptr;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_TEXTURE_BUMP_DISPLACEMENT_H_