#ifndef OGLPROGRAM_IRRADIANCE_MAP_H_
#define OGLPROGRAM_IRRADIANCE_MAP_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramIrradianceMap.h

// OGLProgramIrradianceMap is an OGLProgram for developing the
// irradiance image based lighting approach of global illumination

#include "OGLProgram.h"

class stereocamera;
class ObjectStore;
class cubemap;

class OGLProgramIrradianceMap : public OGLProgram {
public:
	OGLProgramIrradianceMap(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

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

	// Temporary until we set up nodes
	RESULT SetCubemap(cubemap *pCubemap);

private:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

	cubemap *m_pCubemap = nullptr;
	OGLFramebuffer* m_pOGLInputFramebufferCubemap = nullptr;

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

	OGLUniformVector *m_pUniformEyePosition = nullptr;

	OGLUniformBool *m_pUniformHasTextureCubemap = nullptr;
	OGLUniformSamplerCube *m_pUniformTextureCubemap = nullptr;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock = nullptr;
	OGLMaterialBlock *m_pMaterialsBlock = nullptr;
};

#endif // ! OGLPROGRAM_IRRADIANCE_MAP_H_