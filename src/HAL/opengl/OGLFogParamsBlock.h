#ifndef OGL_FOG_PARAMS_BLOCK_H_
#define OGL_FOG_PARAMS_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLFogParamsBlock.h
// OpenGL Fog Block - This is a block representing the current fog configuration

#include "shaders/OGLUniformBlock.h"
#include "Primitives/FogParams.h"

#define FOG_UNIFORM_BLOCK_BINDING_POINT 2	// Oh boy, see OGLLightsBlock

class OGLFogParamsBlock : public OGLUniformBlock {
public:
	OGLFogParamsBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName);
	~OGLFogParamsBlock();

	//RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);

	RESULT ClearFogParams();
	RESULT SetFogParams(const FogParams &fogParams);

private:

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	struct FogParamsBlock {
		FogParams m_fogParams;
	};
#pragma pack(pop)
};

#endif // ! OGL_FOG_BLOCK_H_
