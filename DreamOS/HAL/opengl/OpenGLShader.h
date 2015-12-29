#ifndef OPEN_GL_SHADER_H_
#define OPEN_GL_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLShader.h
// This is the container object for an OpenGL Shader
// These should be accessed through the OpenGLShaderFactory object

#include "./RESULT/EHM.h"
#include "Primitives/Types/UID.h"

// Header File For The OpenGL32 Library
#include <windows.h>                              // Header File For Windows
#include <gl\gl.h>                
#include <gl\glu.h>                               // Header File For The GLu32 Library
#include <gl\glext.h>
#include <gl\wglext.h>

class OpenGLImp;	// Declare OpenGLImp class

class OpenGLShader {
public:
	OpenGLShader(OpenGLImp *pParentImp, GLenum shaderType);
	~OpenGLShader(void);

	RESULT LoadFromFile(wchar_t *pszFilename);
	RESULT Compile(void);
	RESULT PrintInfoLog();

	char* GetInfoLog();
	
	GLuint GetShaderID() { return m_shaderID; }

protected:
	char* OpenGLShader::FileRead(wchar_t *pszFileName);

protected:
	GLenum	m_shaderType;		// GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	GLuint	m_shaderID;
	char*	m_pszShaderCode;

private:
	UID m_uid;
	OpenGLImp *m_pParentImp;
};

#endif // ! OPEN_GL_SHADER_H_