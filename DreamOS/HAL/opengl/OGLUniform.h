#ifndef OGL_UNIFORM_H_
#define OGL_UNIFORM_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLUniform.h
// This is an OpenGL Uniform variable, owned by a OGLProgram (these are shared across shaders)

#include "OpenGLCommon.h"
#include "GLSLObject.h"

#include <string>

class OGLUniform : public GLSLObject {
public:
	OGLUniform(OGLProgram *pParentProgram, const char *pszUniformName, GLint uniformLocationIndex, GLint GLType) :
		GLSLObject(pParentProgram),
		m_uniformIndex(uniformLocationIndex),
		m_GLType(GLType)
	{
		m_strUniformName.assign(pszUniformName);
	}
	
	~OGLUniform() {
		// empty
	}

	// TODO: Set uniform (balance against type)

private:
	std::string m_strUniformName;
	GLint m_uniformIndex;
	GLint m_GLType;
};

#endif // ! OGL_UNIFORM_H_