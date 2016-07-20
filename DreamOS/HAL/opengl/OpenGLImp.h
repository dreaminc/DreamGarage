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

#include "TimeManager/TimeManager.h"

//#include "Primitives/camera.h"
#include "Primitives/version.h"


#include "OpenGLExtensions.h"
#include "Primitives/DimObj.h"
#include "Primitives/material.h"

//#include "OGLProgram.h"
#include "OGLProgramFactory.h"

#include <memory>

class SandboxApp; 
class Windows64App;
class OGLProfiler;

class OpenGLImp : public HALImp {
private:
	// TODO: Create an OpenGL Program class which should combine
	// the shaders since we might want to jump around OGL programs in the future
	OGLProgram *m_pOGLRenderProgram;
	OGLProgram *m_pOGLProgramShadowDepth;
	OGLProgram *m_pOGLProgramCapture;		// temp for testing
	OGLProgram *m_pOGLSkyboxProgram;
	OGLProgram *m_pOGLOverlayProgram;

	// TODO: Fix this architecture 
	OpenGLRenderingContext *m_pOpenGLRenderingContext;

	version m_versionOGL;
	version m_versionGLSL;

	std::unique_ptr<OGLProfiler>	m_pOGLProfiler;

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

	// Object Factory Methods
public:
	light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	quad* MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr);
	sphere* MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c);
	volume* MakeVolume(double side);
	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	skybox *MakeSkybox();
	model *MakeModel(wchar_t *pszModelName);
	model *MakeModel(const std::vector<vertex>& vertices);
	model *MakeModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	// TODO: Convert to composite
	RESULT LoadModel(SceneGraph* pSceneGraph, const std::wstring& strRootFolder, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, point_precision rotateY = 0);

public:
	// TODO: Consolidate all of these (one Render function)
	RESULT SetMonoViewTarget();
	RESULT SetStereoViewTarget(EYE_TYPE eye);
	RESULT SetStereoFramebufferViewTarget(EYE_TYPE eye);

	RESULT Render(SceneGraph *pSceneGraph);
	RESULT RenderStereo(SceneGraph *pSceneGraph);
	RESULT RenderStereoFramebuffers(SceneGraph *pSceneGraph);

	RESULT Resize(int pxWidth, int pxHeight);
	RESULT Shutdown();

	//RESULT InitializeStereoFramebuffers(HMD *pHMD);
	//RESULT SetHMD(HMD *pHMD);

	// Rendering Context 
	RESULT MakeCurrentContext();
	RESULT ReleaseCurrentContext();

private:
	//RESULT InitializeExtensions();
	RESULT InitializeGLContext();
	RESULT InitializeOpenGLVersion();

	RESULT PrepareScene();

private:
	RESULT Notify(SenseKeyboardEvent *kbEvent);
	RESULT Notify(SenseMouseEvent *mEvent);

	// TODO: The Eye Buffers shouldn't be in the OpenGLImp
	// Best to push into FrameBuffer -> OGLFrameBuffer then attach to HMD or stereo camera


// TODO: Unify access to extensions
public:

	// TODO: Make these generic WGL Functions
	RESULT wglSwapIntervalEXT(int interval);

	// TODO: Unify extension call / wrappers 

	// OGL Program
	RESULT CreateProgram(GLuint *pOGLProgramIndex);
	RESULT IsProgram(GLuint m_OGLProgramIndex);
	RESULT DeleteProgram(GLuint OGLProgramIndex);
	RESULT UseProgram(GLuint OGLProgramIndex);
	RESULT LinkProgram(GLuint OGLProgramIndex);
	RESULT glGetProgramInfoLog(GLuint programID, GLsizei bufSize, GLsizei *length, GLchar *infoLog);

	RESULT glGetProgramiv(GLuint programID, GLenum pname, GLint *params);
	RESULT glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params);
	RESULT glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
	RESULT glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);

	// FBO
	RESULT glGenFramebuffers(GLsizei n, GLuint *framebuffers);
	RESULT glBindFramebuffer(GLenum target, GLuint gluiFramebuffer);

	RESULT glGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
	RESULT glBindRenderbuffer(GLenum target, GLuint renderbuffer);
	RESULT glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	RESULT glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	RESULT glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	RESULT glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
	RESULT CheckFramebufferStatus(GLenum target);
	RESULT glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	RESULT glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	RESULT glDrawBuffers(GLsizei n, const GLenum *bufs);


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

	//RESULT BindAttribLocation(GLint index, const char* pszName);

	//RESULT BindUniformBlock(GLint uniformBlockIndex, GLint uniformBlockBindingPoint);
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
	RESULT glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices);

	// Shaders
	RESULT CreateShader(GLenum type, GLuint *shaderID);
	RESULT ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length);
	RESULT CompileShader(GLuint shaderID);
	RESULT GetShaderiv(GLuint programID, GLenum pname, GLint *params);
	RESULT GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
	RESULT glAttachShader(GLuint program, GLuint shader);

	// Textures
	RESULT GenerateTextures(GLsizei n, GLuint *textures);
	RESULT glActiveTexture(GLenum texture);
	RESULT glBindTextures(GLuint first, GLsizei count, const GLuint *textures);
	RESULT BindTexture(GLenum target, GLuint texture);
	RESULT glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	RESULT glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
	//RESULT glTexParamteri(GLenum target, GLenum pname, GLint param);
	RESULT TexParameteri(GLenum target, GLenum pname, GLint param);
	//RESULT glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	RESULT TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
	RESULT TextureSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
	RESULT glGenerateMipmap(GLenum target);

public:
	RESULT CheckGLError();
	//char* GetInfoLog();

// OpengGL Extension Function Pointers
private:
	OpenGLExtensions m_OpenGLExtensions;
};

#endif // ! OPEN_GL_IMP_H