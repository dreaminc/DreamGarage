#ifndef OGL_UNIFORM_BLOCK_H_
#define OGL_UNIFORM_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLUniformBlock.h
// OpenGL Uniform Block - Represents a contiguous section of memory passed into the shader through GL uniform blocks

#include "OpenGLCommon.h"

class OpenGLImp;

class OGLUniformBlock {
public:
	OGLUniformBlock(OpenGLImp *pParentImp);

	RESULT OGLInitialize();

	RESULT ReleaseOGLUniformBlockBuffers();
	RESULT UpdateOGLUniformBlockBuffers();
	RESULT UpdateUniformBlockIndexFromShader(const char* pszUniformBlockName);

	RESULT BindUniformBlock();
	
	virtual RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferDatA_n) = 0;

	RESULT SetBufferIndex(GLint bufferIndex);
	GLint GetBufferIndex();

	RESULT SetBlockIndex(GLint blockIndex);
	GLint GetBlockIndex();

	RESULT SetBindingPoint(GLint bindingPointIndex);
	GLint GetBindingPoint();

private:
	GLuint m_uniformBlockBufferIndex;
	GLint m_uniformBlockIndex;
	GLint m_uniformBlockBindingPoint;

private:
	OpenGLImp *m_pParentImp;
};

#endif // !OGL_UNIFORM_BLOCK_H_