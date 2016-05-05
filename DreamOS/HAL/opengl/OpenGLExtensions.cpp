#include "OpenGLExtensions.h"

// Initialize all of the extensions
// TODO: Stuff this into an object?
RESULT OpenGLExtensions::InitializeExtensions() {
	RESULT r = R_PASS;

	CNMW((m_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram")),
		"Failed to initialzie glCreateProgram extension");

	CNMW((m_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram")),
		"Failed to initialize glDeleteProgram extension");

	CNMW((m_glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram")),
		"Failed to initialize glIsProgram extension");

	CNMW((m_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)wglGetProcAddress("glGetProgramInterfaceiv")),
		"Failed to initialize glGetProgramInterfaceiv extension");

	CNMW((m_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)wglGetProcAddress("glGetProgramResourceiv")),
		"Failred to initialize glGetProgramResourceiv extension");

	CNMW((m_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)wglGetProcAddress("glGetProgramResourceName")),
		"Failred to initialize glGetProgramResourceName extension");

	CNMW((m_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram")),
		"Failed to initialzie glUseProgram extension");

	CNMW((m_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader")),
		"Failed to initialize glAttachShader extension");

	CNMW((m_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader")),
		"Failed to initialize glDetachShader extension");

	CNMW((m_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram")),
		"Failed to initialize glLinkProgram extension");

	CNMW((m_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog")),
		"Failed to initialize glGetProgramiv extension");

	CNMW((m_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv")),
		"Failed to initialize glGetProgramiv extension");

	CNMW((m_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog")),
		"Failed to initialize glGetShaderInfoLog extension");

	CNMW((m_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation")),
		"Failed to initialize glGetUniformLocation extension");

	CNMW((m_glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i")),
		"Failed to initialize glUniform1i extension");

	CNMW((m_glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv")),
		"Failed to initialize glUniform1iv extension");

	CNMW((m_glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv")),
		"Failed to initialize glUniform2iv extension");

	CNMW((m_glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv")),
		"Failed to initialize glUniform3iv extension");

	CNMW((m_glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv")),
		"Failed to initialize glUniform4iv extension");

	CNMW((m_glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f")),
		"Failed to initialize glUniform1f extension");

	CNMW((m_glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv")),
		"Failed to initialize glUniform1fv extension");

	CNMW((m_glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv")),
		"Failed to initialize glUniform2fv extension");

	CNMW((m_glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv")),
		"Failed to initialize glUniform3fv extension");

	CNMW((m_glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv")),
		"Failed to initialize glUniform4fv extension");

	CNMW((m_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv")),
		"Failed to initialize glUniformMatrix4fv extension");

	// Uniform Blocks
	CNMW((m_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress("glGetUniformBlockIndex")),
		"Failed to initialize glGetUniformBlockIndex extension");
 
	CNMW((m_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress("glUniformBlockBinding")),
		"Failed to initialize glUniformBlockBinding extension");

	CNMW((m_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase")),
		"Failed to initialize glBindBufferBase extension");

	// Attributes
	CNMW((m_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation")),
		"Failed to initialize glGetAttribLocation extension");

	CNMW((m_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f")),
		"Failed to initialize glVertexAttrib1f extension");

	CNMW((m_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv")),
		"Failed to initialize glVertexAttrib1fv extension");

	CNMW((m_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv")),
		"Failed to initialize glVertexAttrib2fv extension");

	CNMW((m_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv")),
		"Failed to initialize glVertexAttrib3fv extension");

	CNMW((m_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv")),
		"Failed to initialize glVertexAttrib4fv extension");

	CNMW((m_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray")),
		"Failed to initialize glEnableVertexAttribArray extension");

	CNMW((m_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation")),
		"Failed to initialize glBindAttribLocation extension");

	// Not supported yet?
	m_glDisableVertexAttribArray = NULL;
	m_glGetActiveUniform = NULL;

	// Shader
	CNMW((m_glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader")),
		"Failed to initialize glCreateShader extension");

	CNMW((m_glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader")),
		"Failed to initialize glDeleteShader extension");

	CNMW((m_glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource")),
		"Failed to initialize glShaderSource extension");

	CNMW((m_glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader")),
		"Failed to initialize glCompileShader extension");

	CNMW((m_glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv")),
		"Failed to initialize glGetShaderiv extension");

	// Textures
	/*
	CNMW((m_glGenTextures = (PFNGLGENTEXTURESPROC)wglGetProcAddress("glGenTextures")),
		"Failed to initialize glGenTextures extension");
	*/

	CNMW((m_glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture")),
		"Failed to initialize glActiveTexture extension");
	
	CNMW((m_glBindTextures = (PFNGLBINDTEXTURESPROC)wglGetProcAddress("glBindTextures")),
		"Failed to initialize glBindTextures extension");

	CNMW((m_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D")),
		"Failed to initialize glTexStorage2D extension");

	CNMW((m_glTexParameteri = (PFNGLTEXPARAMETERIPROC)wglGetProcAddress("glTexParameteri")),
		"Failed to initialize glTexParameteri extension");

	CNMW((m_glTexImage2D = (PFNGLTEXIMAGE2DPROC)wglGetProcAddress("m_glTexImage2D")),
		"Failed to initialize m_glTexImage2D extension");

	// FBO
	CNMW((m_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers")),
		"Failed to initialzie glGenFramebuffers extension");

	CNMW((m_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer")),
		"Failed to initialize glBindFramebuffer extension");

	// VBO
	CNMW((m_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers")),
		"Failed to initialzie glGenBuffers extension");

	CNMW((m_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer")),
		"Failed to initialize glBindBuffer extension");

	CNMW((m_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData")),
		"Failed to initialize glBuifferData extension");

	CNMW((m_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData")),
		"Failed to initialize glBuifferSubData extension");

	CNMW((m_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer")),
		"Failed to initialize glVertexAttribPointer extension");

	CNMW((m_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers")),
		"Failed to initialize glDeleteBuffers extension");

	// VAO
	CNMW((m_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays")),
		"Failed to initialize glGenVertexArrays extension");

	CNMW((m_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray")),
		"Failed to initialize glBindVertexArray extension");

	CNMW((m_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays")),
		"Failed to initialize glDeleteVertexArrays extension");

	return r;
}