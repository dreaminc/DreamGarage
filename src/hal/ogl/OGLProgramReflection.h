#ifndef OGLPROGRAM_REFLECTION_H_
#define OGLPROGRAM_REFLECTION_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramReflection.h

// This is the shader to get the reflection map for a plane

#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

#include "hal/FogProgram.h"

class ObjectStore;
class stereocamera;
class OGLFogParamsBlock;

class OGLProgramReflection : public OGLProgram, public FogProgram {
public:
	OGLProgramReflection(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

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
	//RESULT SetReflectionPlane(plane reflectionPlane);
	RESULT SetReflectionObject(VirtualObj *pReflectionObject);

private:
	RESULT SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;
	//plane m_reflectionPlane;
	VirtualObj *m_pReflectionObject = nullptr;

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
	OGLFogParamsBlock *m_pFogParamsBlock = nullptr;
};

#endif // ! OGLPROGRAM_REFLECTION_H_