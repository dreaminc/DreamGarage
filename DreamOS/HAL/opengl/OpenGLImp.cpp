#include "OpenGLImp.h"


OpenGLImp::OpenGLImp() {
	m_ID = m_glCreateProgram();
}

OpenGLImp::~OpenGLImp() {
	m_glDeleteProgram(m_ID);
}

// Initialize all of the extensions
// TODO: Stuff this into an object?
RESULT OpenGLImp::InitializeExtensions() {
	m_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	m_glDeleteProgram = NULL;
	m_glUseProgram = NULL;
	m_glAttachShader = NULL;
	m_glDetachShader = NULL;
	m_glLinkProgram = NULL;
	m_glGetProgramiv = NULL;
	m_glGetShaderInfoLog = NULL;
	m_glGetUniformLocation = NULL;
	m_glUniform1i = NULL;
	m_glUniform1iv = NULL;
	m_glUniform2iv = NULL;
	m_glUniform3iv = NULL;
	m_glUniform4iv = NULL;
	m_glUniform1f = NULL;
	m_glUniform1fv = NULL;
	m_glUniform2fv = NULL;
	m_glUniform3fv = NULL;
	m_glUniform4fv = NULL;
	m_glUniformMatrix4fv = NULL;
	m_glGetAttribLocation = NULL;
	m_glVertexAttrib1f = NULL;
	m_glVertexAttrib1fv = NULL;
	m_glVertexAttrib2fv = NULL;
	m_glVertexAttrib3fv = NULL;
	m_glVertexAttrib4fv = NULL;
	m_glEnableVertexAttribArray = NULL;
	m_glDisableVertexAttribArray = NULL;
	m_glBindAttribLocation = NULL;
	m_glGetActiveUniform = NULL;

	// Shader
	m_glCreateShader = NULL;
	m_glDeleteShader = NULL;
	m_glShaderSource = NULL;
	m_glCompileShader = NULL;
	m_glGetShaderiv = NULL;

	// VBO
	m_glGenBuffers = NULL;
	m_glBindBuffer = NULL;
	m_glBufferData = NULL;
	m_glVertexAttribPointer = NULL;

	return R_PASS;
}