#ifndef OGL_PROGRAM_FACTORY_H_
#define OGL_PROGRAM_FACTORY_H_

// DREAM OS
// DreamOS/HAL/opengl/OGLProgramFractory.h
// A factory for the creation of OGLPrograms

#include "OGLProgram.h"

typedef enum {
	OGLPROGRAM_MINIMAL,
	OGLPROGRAM_SKYBOX,
	OGLPROGRAM_BLINNPHONG,
	OGLPROGRAM_BLINNPHONG_TEXTURE,
	OGLPROGRAM_CUSTOM,
	OGLPROGRAM_INVALID
} OGLPROGRAM_TYPE;

class OGLProgramFactory {
public:
	static OGLProgram* MakeOGLProgram(OGLPROGRAM_TYPE type, OpenGLImp *pParentImp, version versionOGL);
};

#endif // ! OGL_PROGRAM_FACTORY_H_