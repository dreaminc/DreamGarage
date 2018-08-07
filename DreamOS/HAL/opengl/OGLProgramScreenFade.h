#ifndef OGLPROGRAM_SCREEN_FADE_H_
#define OGLPROGRAM_SCREEN_FADE_H_

#include "./RESULT/EHM.h"
#include "OGLProgramScreenQuad.h"

class OGLQuad;

class OGLProgramScreenFade : public OGLProgramScreenQuad {
public:
	OGLProgramScreenFade(OpenGLImp *pParentImp);
	
	RESULT OGLInitialize();
	
	/*
	virtual RESULT SetupConnections() override;
	virtual RESULT ProcessNode(long frameID) override;

	RESULT SetObjectTextures(OGLObj *pOGLObj);
	RESULT SetObjectUniforms(DimObj *pDimObj);
	RESULT SetCameraUniforms(camera *pCamera);
	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);
	//*/
};

#endif // ! OGLPROGRAM_SCREEN_FADE_H_