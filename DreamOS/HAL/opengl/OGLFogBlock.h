#ifndef OGL_FOG_BLOCK_H_
#define OGL_FOG_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLFogBlock.h
// OpenGL Fog Block - This is a block representing the current fog configuration

#include "shaders/OGLUniformBlock.h"
#include "Primitives/vector.h"
#include "Primitives/point.h"

#define FOG_UNIFORM_BLOCK_BINDING_POINT 2	// Oh boy, see OGLLightsBlock

class OGLFogBlock : public OGLUniformBlock {
public:
	OGLFogBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName);
	~OGLFogBlock();

	//RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);

	RESULT ClearFogConfig();
	RESULT SetFogConfig(float startDistance, float endDistance, float density, vector color);

private:

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	struct FogBlock {
		GLfloat startDistance;
		GLfloat endDistance;
		GLfloat density;
		vector color;
	};
#pragma pack(pop)
};

#endif // ! OGL_FOG_BLOCK_H_
