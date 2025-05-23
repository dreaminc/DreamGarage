#ifndef OPEN_GL_EXTENSION_H_
#define OPEN_GL_EXTENSION_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OpenGLExtensions.h

// This encapsulates all of the OpenGL Extensions

#include "OGLCommon.h"

#include "core/types/UID.h"

class OGLExtensions {
public:
	OGLExtensions() {
		// empty
	}

	~OGLExtensions() {
		// empty
	}

	RESULT InitializeExtensions();

	// TODO: WGL Extensions should be elsewhere

	inline BOOL wglSwapIntervalEXT(int interval) {
		return m_wglSwapIntervalEXT(interval);
	}

	// OGL Program
	inline GLuint glCreateProgram(void) {
		return m_glCreateProgram();
	}

	inline void glDeleteProgram(GLuint programID) {
		return m_glDeleteProgram(programID);
	}

	inline GLboolean glIsProgram(GLuint programID) {
		return m_glIsProgram(programID);
	}

	inline void glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params) {
		return m_glGetProgramInterfaceiv(program, programInterface, pname, params);
	}

	inline void glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params) {
		return m_glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params);
	}

	inline void glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name) {
		return m_glGetProgramResourceName(program, programInterface, index, bufSize, length, name);
	}

	inline void glUseProgram(GLuint programID) {
		return m_glUseProgram(programID);
	}

	inline void glLinkProgram(GLuint programID) {
		return m_glLinkProgram(programID);
	}

	inline void glGetProgramiv(GLuint programID, GLenum pname, GLint *params) {
		return m_glGetProgramiv(programID, pname, params);
	}

	inline void glGetProgramInfoLog(GLuint programID, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
		return m_glGetProgramInfoLog(programID, bufSize, length, infoLog);
	}

	// Draw
	inline void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices) {
		return m_glDrawRangeElements(mode, start, end, count, type, indices);
	}

	// Uniform Variables
	inline GLint glGetUniformLocation(GLuint programID, const GLchar *pszName) {
		return m_glGetUniformLocation(programID, pszName);
	}

	inline void glUniform1i(GLint location, GLint v0) {
		return m_glUniform1i(location, v0);
	}

	inline void glUniform1iv(GLint location, GLsizei count, const GLint *value) {
		return m_glUniform1iv(location, count, value);
	}

	inline void glUniform2iv(GLint location, GLsizei count, const GLint *value) {
		return m_glUniform2iv(location, count, value);
	}

	inline void glUniform3iv(GLint location, GLsizei count, const GLint *value) {
		return m_glUniform3iv(location, count, value);
	}

	inline void glUniform4iv(GLint location, GLsizei count, const GLint *value) {
		return m_glUniform4iv(location, count, value);
	}

	inline void glUniform1f(GLint location, GLfloat v0) {
		return m_glUniform1f(location, v0);
	}

	inline void glUniform1fv(GLint location, GLsizei count, const GLfloat *value) {
		return m_glUniform1fv(location, count, value);
	}

	inline void glUniform2fv(GLint location, GLsizei count, const GLfloat *value) {
		return m_glUniform2fv(location, count, value);
	}

	inline void glUniform3fv(GLint location, GLsizei count, const GLfloat *value) {
		return m_glUniform3fv(location, count, value);
	}

	inline void glUniform4fv(GLint location, GLsizei count, const GLfloat *value) {
		return m_glUniform4fv(location, count, value);
	}

	inline void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
		return m_glUniformMatrix4fv(location, count, transpose, value);
	}

	inline void glGetActiveUniform(GLuint programID, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *pszName) {
		return m_glGetActiveUniform(programID, index, bufSize, length, size, type, pszName);
	}

	// Uniform Blocks
	inline GLint glGetUniformBlockIndex(GLuint programID, const GLchar *pszName) {
		return m_glGetUniformBlockIndex(programID, pszName);
	}

	inline void glUniformBlockBinding(GLuint programID, GLint uniformBlockIndex, GLint uniformBlockBindingPoint) {
		return m_glUniformBlockBinding(programID, uniformBlockIndex, uniformBlockBindingPoint);
	}

	inline void glBindBufferBase(GLenum target, GLuint bindingPointIndex, GLuint bufferIndex) {
		return m_glBindBufferBase(target, bindingPointIndex, bufferIndex);
	}

	inline void glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const*uniformNames, GLuint *uniformIndices) {
		return m_glGetUniformIndices(program, uniformCount, uniformNames, uniformIndices);
	}

	// Attributes
	inline GLint glGetAttribLocation(GLuint programID, const GLchar *pszName) {
		return m_glGetAttribLocation(programID, pszName);
	}

	inline void glVertexAttrib1f(GLuint index, GLfloat x) {
		return m_glVertexAttrib1f(index, x);
	}

	inline void glVertexAttrib1fv(GLuint index, const GLfloat *v) {
		return m_glVertexAttrib1fv(index, v);
	}

	inline void glVertexAttrib2fv(GLuint index, const GLfloat *v) {
		return m_glVertexAttrib2fv(index, v);
	}

	inline void glVertexAttrib3fv(GLuint index, const GLfloat *v) {
		return m_glVertexAttrib3fv(index, v);
	}

	inline void glVertexAttrib4fv(GLuint index, const GLfloat *v) {
		return m_glVertexAttrib4fv(index, v);
	}

	inline void glEnableVertexAttribArray(GLuint index) {
		return m_glEnableVertexAttribArray(index);
	}

	inline void glDisableVertexAttribArray(GLuint index) {
		return m_glDisableVertexAttribArray(index);
	}

	inline void glBindAttribLocation(GLuint programID, GLuint index, const GLchar *pszName) {
		return m_glBindAttribLocation(programID, index, pszName);
	}

	// Blending

	inline void glBlendEquation(GLenum mode) {
		return m_glBlendEquation(mode);
	}

	inline void glBlendFuncSeparate(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha) {
		return m_glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
	}

	// Shader
	inline void glAttachShader(GLuint programID, GLuint shaderID) {
		return m_glAttachShader(programID, shaderID);
	}

	inline void glDetachShader(GLuint programID, GLuint shaderID) {
		return m_glDetachShader(programID, shaderID);
	}

	inline GLuint glCreateShader(GLenum type) {
		return m_glCreateShader(type);
	}

	inline GLuint glCreateShaderObject(GLenum type) {
		return m_glCreateShaderObject(type);
	}

	inline void glDeleteShader(GLuint shader) {
		return m_glDeleteShader(shader);
	}

	inline void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length) {
		return m_glShaderSource(shader, count, string, length);
	}

	inline void glCompileShader(GLuint shaderID) {
		return m_glCompileShader(shaderID);
	}

	inline void glGetShaderiv(GLuint shader, GLenum pname, GLint *params) {
		return m_glGetShaderiv(shader, pname, params);
	}

	inline void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
		return m_glGetShaderInfoLog(shader, bufSize, length, infoLog);
	}

	// Textures
	/*
	inline void glGenTextures(GLsizei n, GLuint *textures) {
		return m_glGenTextures(n, textures);
	}
	*/

	inline void glActiveTexture(GLenum texture) {
		return m_glActiveTexture(texture);
	}

	inline void glBindTextures(GLuint first, GLsizei count, const GLuint *textures) {
		return m_glBindTextures(first, count, textures);
	}

	inline void glTextStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
		return m_glTexStorage2D(target, levels, internalformat, width, height);
	}

	inline void glTexImage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) {
		return m_glTexImage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
	}

	/*
	inline void glTexParameteri(GLenum target, GLenum pname, GLint param) {
		return m_glTexParameteri(target, pname, param);
	}
	*/

	/*
	inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
		return m_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	}
	*/

	inline void glGetnTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) {
		return m_glGetnTexImage(target, level, format, type, bufSize, pixels);
	}

	inline void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels) {
		return m_glGetTextureImage(texture, level, format, type, bufSize, pixels);
	}

	inline void glGenerateMipmap(GLenum target) {
		return m_glGenerateMipmap(target);
	}

	// FBO
	inline void glGenFramebuffers(GLsizei n, GLuint *framebuffers) {
		return m_glGenFramebuffers(n, framebuffers);
	}

	inline void glBindFramebuffer(GLenum target, GLuint framebufferID) {
		return m_glBindFramebuffer(target, framebufferID);
	}

	inline void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers) {
		return m_glDeleteFrameBuffers(n, framebuffers);
	}

	inline void glGenRenderbuffers(GLsizei n, GLuint *renderbuffers) {
		return m_glGenRenderbuffers(n, renderbuffers);
	}

	inline void glDeleteRenderbuffers(GLsizei n, GLuint *renderbuffers) {
		return m_glDeleteRenderbuffers(n, renderbuffers);
	}

	inline void glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
		return m_glBindRenderbuffer(target, renderbuffer);
	}

	inline void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) {
		return m_glRenderbufferStorage(target, internalformat, width, height);
	}

	inline void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height) {
		return m_glRenderBufferStorageMultisample(target, samples, internalformat, width, height);
	}

	inline void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) {
		return m_glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
	}

	inline void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level) {
		return m_glFramebufferTexture(target, attachment, texture, level);
	}

	inline GLenum glCheckFramebufferStatus(GLenum target) {
		return m_glCheckFramebufferStatus(target);
	}

	inline void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
		return m_glFramebufferTexture2D(target, attachment, textarget, texture, level);
	}

	inline void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter) {
		return m_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	}

	inline void glDrawBuffers(GLsizei n, const GLenum *bufs) {
		return m_glDrawBuffers(n, bufs);
	}

	// PBO
	inline void* glMapBuffer(GLenum target, GLenum access) {
		return m_glMapBuffer(target, access);
	}

	inline bool glUnmapBuffer(GLenum target) {
		return m_glUnmapBuffer(target);
	}

	// VBO
	inline void glGenBuffers(GLsizei n, GLuint *buffers) {
		return m_glGenBuffers(n, buffers);
	}

	inline void glBindBuffer(GLenum target, GLuint bufferID) {
		return m_glBindBuffer(target, bufferID);
	}

	inline void glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
		return m_glBufferData(target, size, data, usage);
	}

	inline void glBufferSubData(GLenum target, GLsizeiptr offset, GLsizeiptr size, const void *data) {
		return m_glBufferSubData(target, offset, size, data);
	}

	inline void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
		return m_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	}

	inline void glDeleteBuffers(GLsizei n, const GLuint *buffers) {
		return m_glDeleteBuffers(n, buffers);
	}

	// VAO
	inline void glGenVertexArrays(GLsizei n, GLuint *arrays) {
		return m_glGenVertexArrays(n, arrays);
	}

	inline void glBindVertexArray(GLuint array) {
		return m_glBindVertexArray(array);
	}

	inline void glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
		return m_glDeleteVertexArrays(n, arrays);
	}

	// Queries 
	inline void glGenQueries(GLsizei n, GLuint *ids) {
		return m_glGenQueries(n, ids);
	}

	inline void glDeleteQueries(GLsizei n, const GLuint *ids) {
		return m_glDeleteQueries(n, ids);
	}

	inline GLboolean glIsQuery(GLuint id) {
		return m_glIsQuery(id);
	}

	inline void glBeginQuery(GLenum target, GLuint id) {
		return m_glBeginQuery(target, id);
	}

	inline void glEndQuery(GLenum target) {
		return m_glEndQuery(target);
	}

	inline void glGetQueryiv(GLenum target, GLenum pname, GLint *params) {
		return m_glGetQueryiv(target, pname, params);
	}

	inline void glGetQueryObjectiv(GLuint id, GLenum pname, GLint *params) {
		return m_glGetQueryObjectiv(id, pname, params);
	}

	inline void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint *params) {
		return m_glGetQueryObjectuiv(id, pname, params);
	}

private:
	// TODO: WGL stuff should be elsewhere
	//PFNWGLSWAPINTERVALEXTPROC m_wglSwapIntervalEXT;
	BOOL(WINAPI * m_wglSwapIntervalEXT) (int interval);

	// OGL Program 
	PFNGLCREATEPROGRAMPROC m_glCreateProgram;
	PFNGLDELETEPROGRAMPROC m_glDeleteProgram;
	PFNGLISPROGRAMPROC m_glIsProgram;
	PFNGLGETPROGRAMINTERFACEIVPROC m_glGetProgramInterfaceiv;
	PFNGLGETPROGRAMRESOURCEIVPROC m_glGetProgramResourceiv;
	PFNGLGETPROGRAMRESOURCENAMEPROC m_glGetProgramResourceName;
	PFNGLUSEPROGRAMPROC m_glUseProgram;
	PFNGLLINKPROGRAMPROC m_glLinkProgram;
	PFNGLGETPROGRAMIVPROC m_glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC m_glGetProgramInfoLog;

	// Draw
	PFNGLDRAWRANGEELEMENTSPROC m_glDrawRangeElements;

	// Uniform Variables
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
	PFNGLGETACTIVEUNIFORMPROC m_glGetActiveUniform;

	// Uniform Blocks
	PFNGLGETUNIFORMBLOCKINDEXPROC m_glGetUniformBlockIndex;
	PFNGLUNIFORMBLOCKBINDINGPROC m_glUniformBlockBinding;
	PFNGLBINDBUFFERBASEPROC m_glBindBufferBase;
	PFNGLGETUNIFORMINDICESPROC m_glGetUniformIndices;

	// Attributes
	PFNGLGETATTRIBLOCATIONPROC m_glGetAttribLocation;
	PFNGLVERTEXATTRIB1FPROC m_glVertexAttrib1f;
	PFNGLVERTEXATTRIB1FVPROC m_glVertexAttrib1fv;
	PFNGLVERTEXATTRIB2FVPROC m_glVertexAttrib2fv;
	PFNGLVERTEXATTRIB3FVPROC m_glVertexAttrib3fv;
	PFNGLVERTEXATTRIB4FVPROC m_glVertexAttrib4fv;
	PFNGLENABLEVERTEXATTRIBARRAYPROC m_glEnableVertexAttribArray;
	PFNGLDISABLEVERTEXATTRIBARRAYPROC m_glDisableVertexAttribArray;
	PFNGLBINDATTRIBLOCATIONPROC m_glBindAttribLocation;
	
	// Blending
	PFNGLBLENDEQUATIONPROC m_glBlendEquation;
	PFNGLBLENDFUNCSEPARATEPROC m_glBlendFuncSeparate;

	// Shader
	PFNGLATTACHSHADERPROC m_glAttachShader;
	PFNGLDETACHSHADERPROC m_glDetachShader;
	PFNGLCREATESHADERPROC m_glCreateShader;
	PFNGLCREATESHADEROBJECTARBPROC m_glCreateShaderObject;
	PFNGLDELETESHADERPROC m_glDeleteShader;
	PFNGLSHADERSOURCEPROC m_glShaderSource;
	PFNGLCOMPILESHADERPROC m_glCompileShader;
	PFNGLGETSHADERIVPROC m_glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC m_glGetShaderInfoLog;

	// Textures
	//PFNGLGENTEXTURESPROC m_glGenTextures;
	PFNGLACTIVETEXTUREPROC m_glActiveTexture;
	PFNGLBINDTEXTURESPROC m_glBindTextures;
	//PFNGLBINDTEXTUREPROC m_glBindTexture;
	PFNGLTEXSTORAGE2DPROC m_glTexStorage2D;
	PFNGLTEXIMAGE2DMULTISAMPLEPROC m_glTexImage2DMultisample;
	//PFNGLTEXPARAMETERIPROC m_glTexParameteri;
	//PFNGLTEXIMAGE2DPROC m_glTexImage2D;
	//PFNGLCOPYTEXIMAGE2DPROC m_glCopyTexImage2D;	// part of core
	//PFNGLTEXSUBIMAGE2DPROC m_glTexSubImage2D;
	PFNGLGENERATEMIPMAPPROC m_glGenerateMipmap;
	PFNGLGETTEXTUREIMAGEPROC m_glGetTextureImage;
	PFNGLGETNTEXIMAGEPROC m_glGetnTexImage;


	// FBO
	PFNGLGENFRAMEBUFFERSPROC m_glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC m_glBindFramebuffer;
	PFNGLDELETEFRAMEBUFFERSPROC m_glDeleteFrameBuffers;

	PFNGLGENRENDERBUFFERSPROC m_glGenRenderbuffers;
	PFNGLDELETERENDERBUFFERSPROC m_glDeleteRenderbuffers;
	PFNGLBINDRENDERBUFFERPROC m_glBindRenderbuffer;
	PFNGLRENDERBUFFERSTORAGEPROC m_glRenderbufferStorage;
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC m_glRenderBufferStorageMultisample;
	PFNGLFRAMEBUFFERRENDERBUFFERPROC m_glFramebufferRenderbuffer;
	PFNGLFRAMEBUFFERTEXTUREPROC m_glFramebufferTexture;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC m_glCheckFramebufferStatus;
	PFNGLFRAMEBUFFERTEXTURE2DPROC m_glFramebufferTexture2D;
	PFNGLBLITFRAMEBUFFERPROC m_glBlitFramebuffer;

	PFNGLDRAWBUFFERSPROC m_glDrawBuffers;

	// PBO
	PFNGLMAPBUFFERPROC m_glMapBuffer;
	PFNGLUNMAPBUFFERPROC m_glUnmapBuffer;

	// VBO
	PFNGLGENBUFFERSPROC m_glGenBuffers;
	PFNGLBINDBUFFERPROC	m_glBindBuffer;
	PFNGLBUFFERDATAPROC	m_glBufferData;
	PFNGLBUFFERSUBDATAPROC m_glBufferSubData;
	PFNGLVERTEXATTRIBPOINTERPROC m_glVertexAttribPointer;
	PFNGLDELETEBUFFERSPROC m_glDeleteBuffers;

	// VAO
	PFNGLGENVERTEXARRAYSPROC m_glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC m_glBindVertexArray;
	PFNGLDELETEVERTEXARRAYSPROC m_glDeleteVertexArrays;

	// QUERY
	PFNGLGENQUERIESPROC m_glGenQueries;
	PFNGLDELETEQUERIESPROC m_glDeleteQueries;
	PFNGLISQUERYPROC m_glIsQuery;
	PFNGLBEGINQUERYPROC m_glBeginQuery;
	PFNGLENDQUERYPROC m_glEndQuery;
	PFNGLGETQUERYIVPROC m_glGetQueryiv;
	PFNGLGETQUERYOBJECTIVPROC m_glGetQueryObjectiv;
	PFNGLGETQUERYOBJECTUIVPROC m_glGetQueryObjectuiv;
};

#endif // ! OPEN_GL_EXTENSION_H_