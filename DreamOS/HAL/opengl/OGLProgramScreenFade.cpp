#include "OGLProgramScreenFade.h"

OGLProgramScreenFade::OGLProgramScreenFade(OpenGLImp *pParentImp) :
	OGLProgramScreenQuad(pParentImp, "oglscreenfade")
{
	// empty
}

RESULT OGLProgramScreenFade::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgramScreenQuad::OGLInitialize());

Error:
	return r;
}

/*
RESULT SetupConnections();
RESULT ProcessNode(long frameID);

RESULT SetObjectTextures(OGLObj *pOGLObj);
RESULT SetObjectUniforms(DimObj *pDimObj);
RESULT SetCameraUniforms(camera *pCamera);
RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye);
//*/