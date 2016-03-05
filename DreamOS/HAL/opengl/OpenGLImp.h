#ifndef OPEN_GL_IMP_H_
#define OPEN_GL_IMP_H_

// Dream OS
// DreamOS/HAL/opengl/OpenGLImp.h
// This is the top level header for OpenGL for either native or

#include "./RESULT/EHM.h"
#include "./HAL/HALImp.h"

#if defined(_WIN32)
	#include <windows.h>

	#include <gl\gl.h>
	#include <gl\glu.h>                               // Header File For The GLu32 Library
	#include <gl\glext.h>
#elif defined(__APPLE__)
	#import <OpenGL/gl.h>
	#import <OpenGL/glu.h>
	#include <OpenGL/glext.h>
	//#include <OpenGL/wglext.h>
#endif

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "OpenGLRenderingContext.h"

#include "OpenGLShader.h"
#include "OGLVertexShader.h"
#include "OGLFragmentShader.h"

#include "Primitives/camera.h"

#include "OpenGLExtensions.h"

class SandboxApp; 
class Windows64App;

class OpenGLImp : public HALImp {
private:

	// TODO: Create an OpenGL Program class which should combine
	// the shaders since we might want to jump around OGL programs in the future
	GLuint m_idOpenGLProgram;

	// TODO: Fix this architecture 
	OpenGLRenderingContext *m_pOpenGLRenderingContext;

	int m_versionMajor;
	int m_versionMinor;
	int m_versionGLSL;

	// Viewport
	// TODO: Move this into an object?
private:
	int m_pxViewWidth;
	int m_pxViewHeight;

public:
	int GetViewWidth() { return m_pxViewWidth; }
	int GetViewHeight() { return m_pxViewHeight; }

public:
	OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext);
	~OpenGLImp();

public:
	RESULT Resize(int pxWidth, int pxHeight);
	RESULT ShutdownImplementaiton();
	RESULT Render();
	RESULT PrintVertexAttributes();
	RESULT PrintActiveUniformVariables();

private:
	//RESULT InitializeExtensions();
	RESULT InitializeGLContext();
	RESULT InitializeOpenGLVersion();

	RESULT PrepareScene();

	// TODO: Temporary, replace with object store arch
	RESULT SetData();

private:
	// TODO: Move this into OGLProgram class (implement)
	OGLVertexShader *m_pVertexShader;
	OGLFragmentShader *m_pFragmentShader;
	// TODO: Other shaders

	camera *m_pCamera;
	RESULT Notify(SenseKeyboardEvent *kbEvent);
	RESULT Notify(SenseMouseEvent *mEvent);

public:
	inline RESULT EnableVertexPositionAttribute();
	inline RESULT EnableVertexColorAttribute();

	// Utility TODO: Move to a different object
public:
	static const char *GetOGLTypeString(GLushort GLType);

// TODO: Unify access to extensions
public:

	
	// TODO: Unify extension call / wrappers 

	RESULT glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params);
	RESULT glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
	RESULT glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);

	RESULT glGenVertexArrays(GLsizei n, GLuint *arrays);
	RESULT glBindVertexArray(GLuint gluiArray);
	RESULT glGenBuffers(GLsizei n, GLuint *buffers);
	RESULT glBindBuffer(GLenum target, GLuint buffer);
	
	RESULT glDeleteBuffers(GLsizei n, const GLuint *buffers);

	RESULT glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
	RESULT glEnableVertexAtrribArray(GLuint index);
	RESULT glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

	RESULT glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
	RESULT glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);

	RESULT BindAttribLocation(unsigned int index, char* pszName);

	// Uniform Variables
	RESULT glGetUniformLocation(GLuint program, const GLchar *name, GLint *pLocation);
	RESULT glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

	// Shaders
	RESULT CreateShader(GLenum type, GLuint *shaderID);
	RESULT ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length);
	RESULT CompileShader(GLuint shaderID);
	RESULT GetShaderiv(GLuint programID, GLenum pname, GLint *params);
	RESULT GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

// Extension Mappings
private:
	RESULT CheckGLError();
	RESULT CreateGLProgram();
	char* GetInfoLog();
	RESULT UseProgram();
	RESULT LinkProgram();

public:
	RESULT AttachShader(OpenGLShader *pOpenGLShader);

// OpengGL Extension Function Pointers
// TODO: Push this to another object to manage the extensions
private:
	OpenGLExtensions m_OpenGLExtensions;
	/*
	// OGL Program 
	PFNGLCREATEPROGRAMPROC m_glCreateProgram;
	PFNGLDELETEPROGRAMPROC m_glDeleteProgram;
	PFNGLISPROGRAMPROC m_glIsProgram;
	PFNGLGETPROGRAMINTERFACEIVPROC m_glGetProgramInterfaceiv;
	PFNGLGETPROGRAMRESOURCEIVPROC m_glGetProgramResourceiv;
	PFNGLGETPROGRAMRESOURCENAMEPROC m_glGetProgramResourceName;

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
	PFNGLDELETEBUFFERSPROC m_glDeleteBuffers;

	// VAO
	PFNGLGENVERTEXARRAYSPROC m_glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC m_glBindVertexArray;
	PFNGLDELETEVERTEXARRAYSPROC m_glDeleteVertexArrays;
	*/
};

#endif // ! OPEN_GL_IMP_H