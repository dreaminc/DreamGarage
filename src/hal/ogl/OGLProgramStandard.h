#ifndef OGLPROGRAM_STANDARD_H_
#define OGLPROGRAM_STANDARD_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramEnvironmentObjects.h

// This is a production shader used for environment models such as head and hands

#include <chrono>

#include "hal/EnvironmentProgram.h"

#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class ObjectStore;
class stereocamera;

class OGLProgramStandard : public OGLProgram, public EnvironmentProgram {
public:
	OGLProgramStandard(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

	RESULT OGLInitialize();
	virtual RESULT OGLInitialize(version versionOGL) override;

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

#ifdef _DEBUG
	virtual RESULT PreProcessNode(long frameID) override;
#endif

	virtual RESULT SetIsAugmented(bool fAugmented) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetLights(std::vector<light*> *pLights);
	RESULT SetMaterial(material *pMaterial);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);
	RESULT SetRiverAnimation(bool fRiverAnimation);

public:
	RESULT SetCubemap(cubemap *pCubemap);

private:
	RESULT SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

	cubemap *m_pCubemap = nullptr;
	OGLFramebuffer* m_pOGLInputFramebufferEnvironmentCubemap = nullptr;
	OGLFramebuffer* m_pOGLInputFramebufferIrradianceCubemap = nullptr;

	bool m_fAREnabled = false;

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
	//OGLUniformMatrix4 *m_pUniformProjectionMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix = nullptr;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix = nullptr;

	OGLUniformVector *m_pUniformEyePosition = nullptr;

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

	OGLUniformBool *m_pUniformHasCubemapEnvironment = nullptr;
	OGLUniformSamplerCube *m_pUniformCubemapEnvironment = nullptr;

	OGLUniformBool *m_pUniformHasCubemapIrradiance = nullptr;
	OGLUniformSamplerCube *m_pUniformCubemapIrradiance = nullptr;

	OGLUniformBool *m_pUniformRiverAnimation = nullptr;
	OGLUniformBool *m_pUniformAREnabled = nullptr;

	OGLUniform *m_pUniformTime = nullptr;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock = nullptr;
	OGLMaterialBlock *m_pMaterialsBlock = nullptr;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

	float m_deltaTime; 
};

#endif // ! OGLPROGRAM_STANDARD_H_
