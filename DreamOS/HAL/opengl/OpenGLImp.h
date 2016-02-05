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

// TODO: This break architecture
#include "OpenGLShader.h"

class SandboxApp; 
class Windows64App;

class OpenGLImp : public HALImp {
private:
	GLuint m_idOpenGLProgram;
	HGLRC m_hglrc;		// OpenGL rendering context

	// TODO: Fix this architecture 
	//HDC e_hDC;
	Windows64App *m_pWindows64App;

	int m_versionMajor;
	int m_versionMinor;
	int m_versionGLSL;

public:
	SandboxApp *GetParentApp();	// TODO: This should go into the HALImp

public:
	//OpenGLImp(HDC hDC);
	OpenGLImp(Windows64App *pWindows64App);
	~OpenGLImp();

public:
	RESULT Resize(int pxWidth, int pxHeight);
	RESULT ShutdownImplementaiton();
	RESULT Render();

private:
	RESULT InitializeExtensions();
	RESULT InitializeGLContext();
	RESULT InitializeOpenGLVersion();

	RESULT PrepareScene();

	// TODO: Temporary, replace with object store arch
	GLuint m_vboID[2];	// Temporary 
	GLuint m_vaoID;		// Temporary
	RESULT SetData();

private:
	OpenGLShader *m_pVertexShader;
	OpenGLShader *m_pFragmentShader;
	// Other shaders


// TODO: Unify access to extensions
public:
	inline GLuint glCreateProgram(void) { return m_glCreateProgram(); }
	inline void glDeleteProgram(GLuint programID) { return m_glDeleteProgram(programID); }
	inline GLuint glCreateShader(GLenum type) { return m_glCreateShader(type); }
	inline void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) {
		return m_glShaderSource(shader, count, string, length);
	}

	inline void glCompileShader(GLuint shader) { return m_glCompileShader(shader); }
	inline void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
		return m_glGetShaderiv(shader, pname, params);
	}

	inline void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
		return m_glGetShaderInfoLog(shader, bufSize, length, infoLog);
	}

	inline GLboolean glIsProgram(GLuint programID) {
		return m_glIsProgram(programID);
	}

// Extension Mappings
private:
	RESULT CheckGLError();
	RESULT CreateGLProgram();
	char* GetInfoLog();
	RESULT UseProgram();
	RESULT LinkProgram();
	RESULT BindAttribLocation(unsigned int index, char* pszName);

public:
	RESULT AttachShader(OpenGLShader *pOpenGLShader);

// OpengGL Extension Function Pointers
// TODO: Push this to another object to manage the extensions
private:
	PFNGLCREATEPROGRAMPROC m_glCreateProgram;
	PFNGLDELETEPROGRAMPROC m_glDeleteProgram;
	PFNGLISPROGRAMPROC m_glIsProgram;

	PFNGLUSEPROGRAMPROC m_glUseProgram;
	PFNGLATTACHSHADERPROC m_glAttachShader;
	PFNGLDETACHSHADERPROC m_glDetachShader;
	PFNGLLINKPROGRAMPROC m_glLinkProgram;
	PFNGLGETPROGRAMIVPROC m_glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC m_glGetProgramInfoLog;
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

	// VAO
	PFNGLGENVERTEXARRAYSPROC m_glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC m_glBindVertexArray;
};

#endif // ! OPEN_GL_IMP_H