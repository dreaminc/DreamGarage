#ifndef OGLPROGRAM_DEPTH_PEEL_H_
#define OGLPROGRAM_DEPTH_PEEL_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramMinimal.h
// OGLProgramMinimal is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader that simply takes in a vertex point and color
// and renders it using the usual suspects of required matrices (no lights, no textures)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class ObjectStore;
class stereocamera;
class OGLQuery;

class OGLProgramBlendQuad;

#define MAX_DEPTH_PEEL_LAYERS 6

class OGLProgramDepthPeel : public OGLProgram {
public:
	OGLProgramDepthPeel(OpenGLImp *pParentImp);

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;
	virtual RESULT PreProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

private:
	RESULT InitializeBlendQuadProgram();

protected:
	stereocamera *m_pCamera = nullptr;
	ObjectStore *m_pSceneGraph = nullptr;

private:
	OGLFramebuffer *m_pOGLFramebufferInputA = nullptr;
	OGLFramebuffer *m_pOGLFramebufferInputB = nullptr;

	OGLFramebuffer *m_pOGLFramebufferOutputA = nullptr;
	OGLFramebuffer *m_pOGLFramebufferOutputB = nullptr;

	int m_numSamplesProcessed;
	int *m_pNumSamplesProcessedLastPass = nullptr;

	int m_depth;
	int *m_pLastDepth = nullptr;

	OGLQuery *m_pOGLQuery = nullptr;

	OGLProgramBlendQuad *m_pOGLProgramBlendQuad = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureDepth = nullptr;
};

#endif // ! OGLPROGRAM_DEPTH_PEEL_H_