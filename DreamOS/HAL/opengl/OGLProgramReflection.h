#ifndef OGLPROGRAM_REFLECTION_H_
#define OGLPROGRAM_REFLECTION_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramReflection.h
// This is the shader to get the reflection map for a plane

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class ObjectStore;
class stereocamera;

class OGLProgramReflection : public OGLProgram {
public:
	OGLProgramReflection(OpenGLImp *pParentImp);

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
	RESULT SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;
	plane *m_pReflectionPlane = nullptr;

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

	OGLUniformBool *m_pUniformHasTextureBump;
	OGLUniformSampler2D *m_pUniformTextureBump;

	OGLUniformBool *m_pUniformHasTextureColor;
	OGLUniformSampler2D *m_pUniformTextureColor;

	OGLUniformBool *m_pUniformHasTextureAmbient;
	OGLUniformSampler2D *m_pUniformTextureAmbient;
	OGLUniformBool *m_pUniformHasTextureDiffuse;
	OGLUniformSampler2D *m_pUniformTextureDiffuse;
	OGLUniformBool *m_pUniformHasTextureSpecular;
	OGLUniformSampler2D *m_pUniformTextureSpecular;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_REFLECTION_H_