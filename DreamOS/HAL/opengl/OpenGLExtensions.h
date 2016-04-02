#ifndef OPEN_GL_EXTENSION_H_
#define OPEN_GL_EXTENSION_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"

// Dream OS
// DreamOS/HAL/opengl/OpenGLExtensions.h
// This encapsulates all of the OpenGL Extensions

#include "OpenGLCommon.h"

class OpenGLExtensions {
public:
	OpenGLExtensions() {
		// empty
	}

	~OpenGLExtensions() {
		// empty
	}

	RESULT InitializeExtensions();

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

	inline void glTexParamteri(GLenum target, GLenum pname, GLint param) {
		return m_glTexParameteri(target, pname, param);
	}

	inline void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
		return m_glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
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

private:
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

	// Shader
	PFNGLATTACHSHADERPROC m_glAttachShader;
	PFNGLDETACHSHADERPROC m_glDetachShader;
	PFNGLCREATESHADERPROC m_glCreateShader;
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
	PFNGLTEXPARAMETERIPROC m_glTexParameteri;
	PFNGLTEXIMAGE2DPROC m_glTexImage2D;

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
};

#endif // ! OPEN_GL_EXTENSION_H_