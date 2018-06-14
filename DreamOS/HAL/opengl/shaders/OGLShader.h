#ifndef OGL_SHADER_H_
#define OGL_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLShader.h
// This is the container object for an OpenGL Shader
// These should be accessed through the OpenGLShaderFactory object

#include "./RESULT/EHM.h"
#include "./HAL/Shader.h"

#include "../OpenGLCommon.h"
#include "Primitives/version.h"
#include "Primitives/matrix/matrix.h"

#include "../GLSLObject.h"

class point;
class quaternion;
class vector;
//class OpenGLImp;	// Declare OpenGLImp class
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

	//const char *GetShaderCode();
	std::vector<std::string> GetShaderCode();
	RESULT AddShaderCode(const char *pszShaderCode);
	RESULT AddShaderCode(const std::string &strShaderCode);
	RESULT ClearShaderCode();

protected:
	char* FileRead(wchar_t *pszFileName);

protected:
	GLenum	m_shaderType;		// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_shaderID;

	//char*	m_pszShaderCode = nullptr;
	std::vector<std::string> m_shaderCode;
};

#endif // ! OGL_SHADER_H_