#ifndef OGLPROGRAM_SHADOW_DEPTH_H_
#define OGLPROGRAM_SHADOW_DEPTH_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramShadowDepth.h
// OGLProgramShadowDepth is an OGLProgram that encapsulates the OGLProgram 
// for a shader used to simply capture a depth map as set by a given light

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "Primitives/matrix/ProjectionMatrix.h"
#include "Primitives/matrix/ViewMatrix.h"

#define SHADOW_MAP_WIDTH 1024 * 4
#define SHADOW_MAP_HEIGHT 1024 * 4

class OGLProgramShadowDepth : public OGLProgram {
public:
<<<<<<< HEAD
	OGLProgramShadowDepth(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) :
		OGLProgram(pParentImp, "oglshadowdepth", optFlags),
		m_pShadowEmitter(nullptr)
	{
		// empty
	}
=======
	OGLProgramShadowDepth(OpenGLImp *pParentImp);
>>>>>>> shadow HAL test set up and relevant shaders (using blinn phong shadow for now) moved to new arch

	RESULT OGLInitialize();

	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetLights(std::vector<light*> *pLights);
	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);

	matrix<virtual_precision, 4, 4>GetViewProjectionMatrix();
	vector GetShadowEmitterDirection();

	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);

	RESULT SetShadowCastingLightSource(light *pLight);

	light *pGetShadowCastingLight();

private:
	ObjectStore *m_pSceneGraph = nullptr;

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

private:
	light *m_pShadowEmitter;
};

#endif // ! OGLPROGRAM_SHADOW_DEPTH_H_