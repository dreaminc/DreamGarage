#ifndef OGL_SHADER_H_
#define OGL_SHADER_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/shaders/OpenGLShader.h

// This is the container object for an OpenGL Shader
// These should be accessed through the OpenGLShaderFactory object

#include "hal/shader.h"

#include "hal/ogl/OGLCommon.h"
#include "hal/ogl/GLSLObject.h"

#include "core/types/version.h"

#include "core/matrix/matrix.h"

class point;
class quaternion;
class vector;
class OGLProgram;

class OGLShader : public Shader, public GLSLObject {
public:
	OGLShader(OGLProgram *pParentProgram);
	OGLShader(OGLProgram *pParentProgram, GLenum shaderType);
	~OGLShader(void);

	RESULT InitializeFromFile(const wchar_t *pszFilename, version versionFile);
	
	RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename);
	RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile);
	RESULT LoadShaderCodeFromString(const char* pszSource);

	RESULT Compile(void);
	RESULT PrintInfoLog();

	char* GetInfoLog();
	
	// Access functions
	GLuint GetShaderID();
	GLenum GetShaderType();

	std::vector<std::string> GetShaderCode();
	RESULT AddShaderCode(const char *pszShaderCode);
	RESULT AddShaderCode(const std::string &strShaderCode);
	RESULT ClearShaderCode();

protected:
	char* FileRead(wchar_t *pszFileName);

protected:
	GLenum	m_shaderType;		// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_shaderID;

	std::vector<std::string> m_shaderCode;
};

#endif // ! OGL_SHADER_H_