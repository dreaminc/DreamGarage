#ifndef OGL_PROGRAM_FACTORY_H_
#define OGL_PROGRAM_FACTORY_H_

// DREAM OS
// DreamOS/HAL/opengl/OGLProgramFractory.h
// A factory for the creation of OGLPrograms

#include "OGLProgram.h"

typedef enum {
	OGLPROGRAM_MINIMAL,
	OGLPROGRAM_MINIMAL_TEXTURE,
	OGLPROGRAM_SKYBOX,
	OGLPROGRAM_SKYBOX_SCATTER,
	OGLPROGRAM_BLINNPHONG,
	OGLPROGRAM_BLINNPHONG_SHADOW,
	OGLPROGRAM_BLINNPHONG_TEXTURE,
	OGLPROGRAM_BLINNPHONG_TEXTURE_SHADOW,
	OGLPROGRAM_BLINNPHONG_TEXTURE_BUMP,
	OGLPROGRAM_TEXTURE_BITBLIT,
	OGLPROGRAM_FLAT,
	OGLPROGRAM_CUSTOM,
	OGLPROGRAM_SHADOW_DEPTH,
	// Production shaders - using features from previous shaders 
	OGLPROGRAM_ENVIRONMENT_OBJECTS, // used for models in the environment such as head and hands
	// /Production shaders
	OGLPROGRAM_INVALID
} OGLPROGRAM_TYPE;

class OGLProgramFactory {
public:
	static ProgramNode* MakeOGLProgram(OGLPROGRAM_TYPE type, OpenGLImp *pParentImp, version versionOGL);
};

#endif // ! OGL_PROGRAM_FACTORY_H_