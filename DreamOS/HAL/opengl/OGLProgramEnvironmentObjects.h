#ifndef OGLPROGRAM_ENVIRONMENT_OBJECTS_H_
#define OGLPROGRAM_ENVIRONMENT_OBJECTS_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramEnvironmentObjects.h
// This is a production shader used for environment models such as head and hands

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

#include <chrono>

class ObjectStore;
class stereocamera;

class OGLProgramEnvironmentObjects : public OGLProgram {
public:
	OGLProgramEnvironmentObjects(OpenGLImp *pParentImp);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetLights(std::vector<light*> *pLights);
	RESULT SetMaterial(material *pMaterial);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);
	RESULT SetRiverAnimation(bool fRiverAnimation);

private:
	void SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform);

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

	OGLUniformBool *m_pUniformRiverAnimation;

	OGLUniform *m_pUniformTime;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

	float m_deltaTime; 

};

#endif // ! OGLPROGRAM_ENVIRONMENT_OBJECTS_H_