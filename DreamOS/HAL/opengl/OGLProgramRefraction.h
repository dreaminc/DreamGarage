#ifndef OGLPROGRAM_REFRACTION_H_
#define OGLPROGRAM_REFRACTION_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramReflection.h
// This is the shader to get the reflection map for a plane

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class ObjectStore;
class stereocamera;

class OGLProgramRefraction : public OGLProgram {
public:
	OGLProgramRefraction(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

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

public:
	RESULT SetRefractionObject(VirtualObj *pRefractionObject);

private:
	RESULT SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;
	VirtualObj *m_pRefractionObject = nullptr;

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
	OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformReflectionMatrix = nullptr;

	OGLUniformVector *m_pUniformClippingPlane = nullptr;
	OGLUniformFloat *m_pUniformClippingOffset = nullptr;

	OGLUniformBool *m_pUniformHasTextureBump = nullptr;
	OGLUniformSampler2D *m_pUniformTextureBump = nullptr;

	OGLUniformBool *m_pUniformHasTextureColor = nullptr;
	OGLUniformSampler2D *m_pUniformTextureColor = nullptr;

	OGLUniformBool *m_pUniformHasTextureAmbient = nullptr;
	OGLUniformSampler2D *m_pUniformTextureAmbient = nullptr;
	OGLUniformBool *m_pUniformHasTextureDiffuse = nullptr;
	OGLUniformSampler2D *m_pUniformTextureDiffuse = nullptr;
	OGLUniformBool *m_pUniformHasTextureSpecular = nullptr;
	OGLUniformSampler2D *m_pUniformTextureSpecular = nullptr;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock = nullptr;
	OGLMaterialBlock *m_pMaterialsBlock = nullptr;
};

#endif // ! OGLPROGRAM_REFRACTION_H_