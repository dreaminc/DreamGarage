#ifndef OPEN_GL_SHADER_H_
#define OPEN_GL_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLShader.h
// This is the container object for an OpenGL Shader
// These should be accessed through the OpenGLShaderFactory object

#include "./RESULT/EHM.h"
#include "./HAL/Shader.h"

// Header File For The OpenGL32 Library
#if defined(_WIN32)
    #include <windows.h>                              // Header File For Windows TODO: This should not be necessary

    #include <gl\gl.h>
    #include <gl\glu.h>                               // Header File For The GLu32 Library
    #include <gl\glext.h>
    #include <gl\wglext.h>
#elif defined(__APPLE__)
    #import <OpenGL/gl.h>
    #import <OpenGL/glu.h>
    #include <OpenGL/glext.h>
    //#include <OpenGL/wglext.h>
#endif

class OpenGLImp;	// Declare OpenGLImp class

class OpenGLShader : public Shader {
public:
	OpenGLShader(OpenGLImp *pParentImp, GLenum shaderType);
	~OpenGLShader(void);

	RESULT InitializeFromFile(wchar_t *pszFilename);
	
	RESULT LoadShaderCodeFromFile(wchar_t *pszFilename);
	RESULT LoadShaderCodeFromString(const char* pszSource);

	RESULT Compile(void);
	RESULT PrintInfoLog();

	char* GetInfoLog();
	
	// Access functions
	GLuint GetShaderID() { return m_shaderID; }
	GLenum GetShaderType() { return m_shaderType; }
	const char *GetShaderCode() { return m_pszShaderCode; }

	/*
	RESULT CreateAndLinkProgram();
	RESULT Enable();
	RESULT Disable();
	RESULT AddAttribute(const char *pszAttribute);
	RESULT AddUniform(const char *pszUniform);
	RESULT DeleteShader();
	*/

	RESULT AttachShader();

protected:
	char* FileRead(wchar_t *pszFileName);

protected:
	GLenum	m_shaderType;		// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_shaderID;
	char*	m_pszShaderCode;

protected:
	OpenGLImp *m_pParentImp;
};

#endif // ! OPEN_GL_SHADER_H_