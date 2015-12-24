#ifndef OPEN_GL_IMP_H_
#define OPEN_GL_IMP_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLImp.h
// This is the top level header for OpenGL for either native or

#include "./RESULT/EHM.h"
#include "./HAL/HALImp.h"

// #include <gl\gl.h>                                // Header File For The OpenGL32 Library
// #include <gl\glu.h>                               // Header File For The GLu32 Library
// #include <gl\glaux.h>                             // Header File For The GLaux Library

// TODO: Get rid of this eventually (OpenGL dependencies)
#include <windows.h>                              // Header File For Windows

// Open GL Includes
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\glu.h>                               // Header File For The GLu32 Library
#include <gl\glext.h>
#include <gl\wglext.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

class OpenGLImp : public HALImp {
private:
	GLuint m_ID;
	HGLRC m_hglrc;		// OpenGL rendering context

	// TODO: Fix this architecture 
	HDC e_hDC;

	int m_versionMajor;
	int m_versionMinor;
	int m_versionGLSL;

public:
	OpenGLImp(HDC hDC);
	~OpenGLImp();

public:
	RESULT Resize(int pxWidth, int pxHeight);
	RESULT ShutdownImplementaiton();
	RESULT Render();

private:
	RESULT InitializeExtensions();
	RESULT InitializeGLContext();
	RESULT InitializeOpenGLVersion();

private:
	// OpengGL Extension Function Pointers
	PFNGLCREATEPROGRAMPROC m_glCreateProgram;
	PFNGLDELETEPROGRAMPROC m_glDeleteProgram;
	PFNGLUSEPROGRAMPROC m_glUseProgram;
	PFNGLATTACHSHADERPROC m_glAttachShader;
	PFNGLDETACHSHADERPROC m_glDetachShader;
	PFNGLLINKPROGRAMPROC m_glLinkProgram;
	PFNGLGETPROGRAMIVPROC m_glGetProgramiv;
	PFNGLGETSHADERINFOLOGPROC m_glGetShaderInfoLog;
	PFNGLGETUNIFORMLOCATIONPROC m_glGetUniformLocation;
	PFNGLUNIFORM1IPROC m_glUniform1i;
	PFNGLUNIFORM1IVPROC m_glUniform1iv;
	PFNGLUNIFORM2IVPROC m_glUniform2iv;
	PFNGLUNIFORM3IVPROC m_glUniform3iv;
	PFNGLUNIFORM4IVPROC m_glUniform4iv;
	PFNGLUNIFORM1FPROC m_glUniform1f;
	PFNGLUNIFORM1FVPROC m_glUniform1fv;
	PFNGLUNIFORM2FVPROC m_glUniform2fv;
	PFNGLUNIFORM3FVPROC m_glUniform3fv;
	PFNGLUNIFORM4FVPROC m_glUniform4fv;
	PFNGLUNIFORMMATRIX4FVPROC m_glUniformMatrix4fv;
	PFNGLGETATTRIBLOCATIONPROC m_glGetAttribLocation;
	PFNGLVERTEXATTRIB1FPROC m_glVertexAttrib1f;
	PFNGLVERTEXATTRIB1FVPROC m_glVertexAttrib1fv;
	PFNGLVERTEXATTRIB2FVPROC m_glVertexAttrib2fv;
	PFNGLVERTEXATTRIB3FVPROC m_glVertexAttrib3fv;
	PFNGLVERTEXATTRIB4FVPROC m_glVertexAttrib4fv;
	PFNGLENABLEVERTEXATTRIBARRAYPROC m_glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC m_glDisableVertexAttribArray;
	PFNGLBINDATTRIBLOCATIONPROC m_glBindAttribLocation;
	PFNGLGETACTIVEUNIFORMPROC m_glGetActiveUniform;

	// Shader
	PFNGLCREATESHADERPROC m_glCreateShader;
	PFNGLDELETESHADERPROC m_glDeleteShader;
	PFNGLSHADERSOURCEPROC m_glShaderSource;
	PFNGLCOMPILESHADERPROC m_glCompileShader;
	PFNGLGETSHADERIVPROC m_glGetShaderiv;

	// VBO
	PFNGLGENBUFFERSPROC m_glGenBuffers;
	PFNGLBINDBUFFERPROC	m_glBindBuffer;
	PFNGLBUFFERDATAPROC	m_glBufferData;
	PFNGLVERTEXATTRIBPOINTERPROC m_glVertexAttribPointer;
};

#endif // ! OPEN_GL_IMP_H