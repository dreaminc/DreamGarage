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

#include "TimeManager/TimeManager.h"

//#include "Primitives/camera.h"

#include "Primitives/valid.h"
#include "Primitives/version.h"

#include "Primitives/stereocamera.h"

#include "OpenGLExtensions.h"

#include "Scene/SceneGraph.h"
#include "Primitives/DimObj.h"
#include "Primitives/material.h"

class SandboxApp; 
class Windows64App;

class OpenGLImp : public HALImp, public valid {
private:

	// TODO: Create an OpenGL Program class which should combine
	// the shaders since we might want to jump around OGL programs in the future
	GLuint m_idOpenGLProgram;

	// TODO: Fix this architecture 
	OpenGLRenderingContext *m_pOpenGLRenderingContext;

	version m_versionOGL;
	version m_versionGLSL;

	// Viewport
	// TODO: Move this into an object?
private:
	int m_pxViewWidth;
	int m_pxViewHeight;

public:
	int GetViewWidth() { return m_pxViewWidth; }
	int GetViewHeight() { return m_pxViewHeight; }
	GLuint GetOGLProgramID() { return m_idOpenGLProgram; }

public:
	OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext);
	~OpenGLImp();

public:

	RESULT SetStereoViewTarget(EYE_TYPE eye);
	RESULT Resize(int pxWidth, int pxHeight);
	RESULT ShutdownImplementaiton();
	
	RESULT Render(SceneGraph *pSceneGraph);
	RESULT RenderStereo(SceneGraph *pSceneGraph);

	RESULT SendObjectToShader(DimObj *pDimObj);
	RESULT SendLightsToShader(std::vector<light*> *pLights);

	RESULT PrintVertexAttributes();
	RESULT PrintActiveUniformVariables();
	
	camera *GetCamera();
	RESULT UpdateCamera();
	RESULT SetCameraMatrix(EYE_TYPE viewTarget);

	RESULT LoadScene(SceneGraph *pSceneGraph, TimeManager *pTimeObj);

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

	stereocamera *m_pCamera;
	RESULT Notify(SenseKeyboardEvent *kbEvent);
	RESULT Notify(SenseMouseEvent *mEvent);

public:
	// TODO: [SHADER] This should be baked into Shader
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
	RESULT EnableVertexUVCoordAttribute();
	RESULT EnableVertexTangentAttribute();
	RESULT EnableVertexBitangentAttribute();

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
	RESULT glBufferSubData(GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data);
	RESULT glEnableVertexAtrribArray(GLuint index);
	RESULT glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

	RESULT glDeleteVertexArrays(GLsizei n, const GLuint *arrays);
	RESULT glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);

	RESULT BindAttribLocation(GLint index, char* pszName);

	RESULT BindUniformBlock(GLint uniformBlockIndex, GLint uniformBlockBindingPoint);
	RESULT BindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex);

	RESULT glGetAttribLocation(GLuint programID, const GLchar *pszName, GLint *pLocation);

	// Uniform Variables
	RESULT glGetUniformLocation(GLuint program, const GLchar *name, GLint *pLocation);
	RESULT glUniform1i(GLint location, GLint v0);
	RESULT glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
	RESULT glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

	// Uniform Blocks
	RESULT glGetUniformBlockIndex(GLuint programID, const GLchar *pszName, GLint *pLocation);
	RESULT glUniformBlockBinding(GLuint programID, GLint uniformBlockIndex, GLint uniformBlockBindingPoint);
	RESULT glBindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex);

	// Shaders
	RESULT CreateShader(GLenum type, GLuint *shaderID);
	RESULT ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length);
	RESULT CompileShader(GLuint shaderID);
	RESULT GetShaderiv(GLuint programID, GLenum pname, GLint *params);
	RESULT GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

	// Textures
	RESULT GenerateTextures(GLsizei n, GLuint *textures);
	RESULT glActiveTexture(GLenum texture);
	RESULT glBindTextures(GLuint first, GLsizei count, const GLuint *textures);
	RESULT BindTexture(GLenum target, GLuint texture);
	RESULT glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	//RESULT glTexParamteri(GLenum target, GLenum pname, GLint param);
	RESULT TexParamteri(GLenum target, GLenum pname, GLint param);
	//RESULT glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	RESULT TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	RESULT TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

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