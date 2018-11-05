#ifndef OGLPROGRAM_WATER_H_
#define OGLPROGRAM_WATER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramWater.h
// This is a first pass at a water program that will include
// reflection and refraction off of a plane

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"
#include "../EnvironmentProgram.h"

#include <chrono>

class ObjectStore;
class stereocamera;

class OGLProgramWater : public OGLProgram {
public:
	OGLProgramWater(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

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
	RESULT SetPlaneObject(VirtualObj* pReflectionObject);

private:
	stereocamera *m_pCamera = nullptr;
	//ObjectStore *m_pSceneGraph = nullptr;
	std::vector<light*> m_lights;

	OGLFramebuffer *m_pOGLReflectionFramebuffer_in = nullptr;
	OGLFramebuffer *m_pOGLRefractionFramebuffer_in = nullptr;

	VirtualObj *m_pPlaneObject = nullptr;

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

	OGLUniformVector *m_pUniformEye = nullptr;

	OGLUniformMatrix4 *m_pUniformReflectionMatrix = nullptr;
	OGLUniformVector *m_pUniformReflectionPlane = nullptr;

	OGLUniformBool *m_pUniformHasTextureReflection = nullptr;
	OGLUniformSampler2D *m_pUniformTextureReflection = nullptr;

	OGLUniformBool *m_pUniformHasTextureRefraction = nullptr;
	OGLUniformSampler2D *m_pUniformTextureRefraction = nullptr;
	OGLUniformBool *m_pUniformHasTextureRefractionDepth = nullptr;
	OGLUniformSampler2D *m_pUniformTextureRefractionDepth = nullptr;


	OGLUniformBool *m_pUniformHasTextureNormal = nullptr;
	OGLUniformSampler2D *m_pUniformTextureNormal = nullptr;

	//OGLUniformBool *m_pUniformHasTextureDiffuse = nullptr;
	//OGLUniformSampler2D *m_pUniformTextureDiffuse = nullptr;
	//OGLUniformBool *m_pUniformHasTextureSpecular = nullptr;
	//OGLUniformSampler2D *m_pUniformTextureSpecular = nullptr;

	OGLUniform *m_pUniformTime = nullptr;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock = nullptr;
	OGLMaterialBlock *m_pMaterialsBlock = nullptr;

};

#endif // ! OGLPROGRAM_STANDARD_H_