#ifndef OGL_UNIFORM_BLOCK_H_
#define OGL_UNIFORM_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLUniformBlock.h
// OpenGL Uniform Block - Represents a contiguous section of memory passed into the shader through GL uniform blocks

#include "../OpenGLCommon.h"
#include "../GLSLObject.h"

#include <string>

//class OpenGLImp;
class OGLProgram;

class OGLUniformBlock : public GLSLObject {
public:
	OGLUniformBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName);

	RESULT OGLInitialize();

	RESULT ReleaseOGLUniformBlockBuffers();
	RESULT UpdateOGLUniformBlockBuffers();
	RESULT UpdateUniformBlockIndexFromShader(const char* pszUniformBlockName);

	RESULT BindUniformBlock();
	
	//virtual RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);

	RESULT SetBufferIndex(GLint bufferIndex);
	GLint GetBufferIndex();

	RESULT SetBlockIndex(GLint blockIndex);
	GLint GetBlockIndex();

	RESULT SetBindingPoint(GLint bindingPointIndex);
	GLint GetBindingPoint();

	const char* GetUniformBlockName() {
		return m_strUniformBlockName.c_str();
	}

private:
	std::string m_strUniformBlockName;

	GLint m_uniformLocationIndex;
	GLint m_uniformBlockIndex;
	GLuint m_uniformBlockBufferIndex;
	GLint m_uniformBlockBindingPoint;

	GLint m_uniformBlockDataSize;

protected:
	void *m_pUniformBufferData; 
	GLsizeiptr m_pUniformBufferData_n;
};

#endif // !OGL_UNIFORM_BLOCK_H_