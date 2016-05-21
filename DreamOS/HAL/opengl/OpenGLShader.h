#ifndef OPEN_GL_SHADER_H_
#define OPEN_GL_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLShader.h
// This is the container object for an OpenGL Shader
// These should be accessed through the OpenGLShaderFactory object

#include "./RESULT/EHM.h"
#include "./HAL/Shader.h"

#include "OpenGLCommon.h"
#include "Primitives/version.h"
#include "Primitives/matrix.h"

#include "GLSLObject.h"

class point;
class quaternion;
class vector;
//class OpenGLImp;	// Declare OpenGLImp class
class OGLProgram;

class OpenGLShader : public Shader, public GLSLObject {
public:
	OpenGLShader(OGLProgram *pParentProgram, GLenum shaderType);
	~OpenGLShader(void);

	RESULT InitializeFromFile(const wchar_t *pszFilename, version versionFile);
	
	RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename);
	RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile);
	RESULT LoadShaderCodeFromString(const char* pszSource);

	//RESULT SetUniform();
	RESULT SetUniformInteger(GLint value, const char* pszUniformName);
	
	RESULT SetUniform4fv(GLfloat *pVal4fv, const char* pszUniformName);
	RESULT SetQuaternionUniform(quaternion q, const char* pszUniformName);
	RESULT SetPointUniform(point pt, const char* pszUniformName);
	RESULT SetVectorUniform(vector v, const char* pszUniformName);

	RESULT Set44MatrixUniform(matrix<float, 4, 4> mat, const char* pszUniformName);

	virtual RESULT GetAttributeLocationsFromShader() = 0;

	// TODO: Uniform Blocks (uniforms in general?) should be pushed into OpenGL program
	// Nearly all of these can just be pushed into the OpenGLShader using a uniform/block/attribute registration system
	virtual RESULT BindAttributes() = 0;
	virtual RESULT BindUniformBlocks() = 0;
	virtual RESULT GetUniformLocationsFromShader() = 0;
	virtual RESULT InitializeUniformBlocks() = 0;
	virtual RESULT UpdateUniformBlockBuffers() = 0;
	//virtual RESULT EnableAttributes() = 0;

	RESULT Compile(void);
	RESULT PrintInfoLog();

	char* GetInfoLog();
	
	// Access functions
	GLuint GetShaderID();
	GLenum GetShaderType();
	const char *GetShaderCode();

	/*
	RESULT CreateAndLinkProgram();
	RESULT Enable();
	RESULT Disable();
	RESULT AddAttribute(const char *pszAttribute);
	RESULT AddUniform(const char *pszUniform);
	RESULT DeleteShader();
	*/

	//RESULT AttachShader();  This goes in the OGLProgram

protected:
	char* FileRead(wchar_t *pszFileName);

protected:
	GLenum	m_shaderType;		// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_shaderID;
	char*	m_pszShaderCode;
};

#endif // ! OPEN_GL_SHADER_H_