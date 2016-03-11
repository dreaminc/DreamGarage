#ifndef OPEN_GL_IMP_H_
#define OPEN_GL_IMP_H_

#include "RESULT/EHM.h"
#include "OpenGLCommon.h"
#include "OpenGLUtility.h"

// Dream OS
// DreamOS/HAL/opengl/OpenGLImp.h
// This is the top level header for OpenGL for either native or

#include "HAL/HALImp.h"

#include "OpenGLRenderingContext.h"

#include "OpenGLShader.h"
#include "OGLVertexShader.h"
#include "OGLFragmentShader.h"

#include "Primitives/camera.h"

#include "OpenGLExtensions.h"

#include "Scene/SceneGraph.h"

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
	RESULT Render(SceneGraph *pSceneGraph);
	RESULT SendObjectToShader(DimObj *pDimObj);
	RESULT PrintVertexAttributes();
	RESULT PrintActiveUniformVariables();

	// Rendering Context 
	RESULT MakeCurrentContext();
	RESULT ReleaseCurrentContext();

private:
	//RESULT InitializeExtensions();
	RESULT InitializeGLContext();
	RESULT InitializeOpenGLVersion();

	RESULT PrepareScene();

private:
	// TODO: Move this into OGLProgram class (implement)
	OGLVertexShader *m_pVertexShader;
	OGLFragmentShader *m_pFragmentShader;
	// TODO: Other shaders

	camera *m_pCamera;
	RESULT Notify(SenseKeyboardEvent *kbEvent);
	RESULT Notify(SenseMouseEvent *mEvent);

public:
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();

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
private:
	OpenGLExtensions m_OpenGLExtensions;
};

#endif // ! OPEN_GL_IMP_H