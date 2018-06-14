#ifndef OGL_PROGRAM_FACTORY_H_
#define OGL_PROGRAM_FACTORY_H_

// DREAM OS
// DreamOS/HAL/opengl/OGLProgramFractory.h
// A factory for the creation of OGLPrograms

#include "OGLProgram.h"

#include <string>
#include <map>

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
	OGLPROGRAM_REFERENCE,
	
	// Production shaders - using features from previous shaders 
	OGLPROGRAM_ENVIRONMENT_OBJECTS, // used for models in the environment such as head and hands
	OGLPROGRAM_SCREEN_QUAD,
	OGLPROGRAM_DEPTH_PEEL,
	OGLPROGRAM_BLEND_QUAD,
	OGLPROGRAM_BLUR_QUAD,
	OGLPROGRAM_UI_STAGE,

	// Testing Shader Programs
	OGLPROGRAM_DEBUG_OVERLAY,
	OGLPROGRAM_VISUALIZE_NORMALS,

	OGLPROGRAM_INVALID
} OGLPROGRAM_TYPE;

class OGLProgramFactory {
private:
	static const std::map<std::string, OGLPROGRAM_TYPE> m_OGLProgramNameType;

public:
	static ProgramNode* MakeOGLProgram(OGLPROGRAM_TYPE type, OpenGLImp *pParentImp, version versionOGL);
	static 	OGLPROGRAM_TYPE OGLProgramTypeFromstring(std::string strProgramName);
};

#endif // ! OGL_PROGRAM_FACTORY_H_