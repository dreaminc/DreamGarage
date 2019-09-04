#include "OpenGLExtensions.h"

// Initialize all of the extensions
// TODO: Stuff this into an object?
RESULT OpenGLExtensions::InitializeExtensions() {
	RESULT r = R_PASS;

	// TODO: WGL stuff should be else where
	WCNM((m_wglSwapIntervalEXT = (BOOL(WINAPI *)(int interval))wglGetProcAddress("wglSwapIntervalEXT")),
		"Failed to initialzie wglSwapIntervalEXT extension");

	WCNM((m_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram")),
		"Failed to initialzie glCreateProgram extension");

	WCNM((m_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram")),
		"Failed to initialize glDeleteProgram extension");

	WCNM((m_glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram")),
		"Failed to initialize glIsProgram extension");

	WCNM((m_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)wglGetProcAddress("glGetProgramInterfaceiv")),
		"Failed to initialize glGetProgramInterfaceiv extension");

	WCNM((m_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)wglGetProcAddress("glGetProgramResourceiv")),
		"Failred to initialize glGetProgramResourceiv extension");

	WCNM((m_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)wglGetProcAddress("glGetProgramResourceName")),
		"Failred to initialize glGetProgramResourceName extension");

	WCNM((m_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram")),
		"Failed to initialzie glUseProgram extension");

	WCNM((m_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader")),
		"Failed to initialize glAttachShader extension");

	WCNM((m_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader")),
		"Failed to initialize glDetachShader extension");

	WCNM((m_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram")),
		"Failed to initialize glLinkProgram extension");

	WCNM((m_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog")),
		"Failed to initialize glGetProgramiv extension");

	WCNM((m_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv")),
		"Failed to initialize glGetProgramiv extension");

	WCNM((m_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog")),
		"Failed to initialize glGetShaderInfoLog extension");

	// Draw
	WCNM((m_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)wglGetProcAddress("glDrawRangeElements")),
		"Failed to initialize glDrawRangeElements extension");

	// Uniform
	WCNM((m_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation")),
		"Failed to initialize glGetUniformLocation extension");

	WCNM((m_glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i")),
		"Failed to initialize glUniform1i extension");

	WCNM((m_glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv")),
		"Failed to initialize glUniform1iv extension");

	WCNM((m_glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv")),
		"Failed to initialize glUniform2iv extension");

	WCNM((m_glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv")),
		"Failed to initialize glUniform3iv extension");

	WCNM((m_glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv")),
		"Failed to initialize glUniform4iv extension");

	WCNM((m_glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f")),
		"Failed to initialize glUniform1f extension");

	WCNM((m_glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv")),
		"Failed to initialize glUniform1fv extension");

	WCNM((m_glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv")),
		"Failed to initialize glUniform2fv extension");

	WCNM((m_glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv")),
		"Failed to initialize glUniform3fv extension");

	WCNM((m_glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv")),
		"Failed to initialize glUniform4fv extension");

	WCNM((m_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv")),
		"Failed to initialize glUniformMatrix4fv extension");

	// Uniform Blocks
	WCNM((m_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)wglGetProcAddress("glGetUniformBlockIndex")),
		"Failed to initialize glGetUniformBlockIndex extension");
 
	WCNM((m_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)wglGetProcAddress("glUniformBlockBinding")),
		"Failed to initialize glUniformBlockBinding extension");

	WCNM((m_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)wglGetProcAddress("glBindBufferBase")),
		"Failed to initialize glBindBufferBase extension");

	WCNM((m_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)wglGetProcAddress("glGetUniformIndices")),
		"Failed to initialize glGetUniformIndices extension");

	// Attributes
	WCNM((m_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation")),
		"Failed to initialize glGetAttribLocation extension");

	WCNM((m_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f")),
		"Failed to initialize glVertexAttrib1f extension");

	WCNM((m_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv")),
		"Failed to initialize glVertexAttrib1fv extension");

	WCNM((m_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv")),
		"Failed to initialize glVertexAttrib2fv extension");

	WCNM((m_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv")),
		"Failed to initialize glVertexAttrib3fv extension");

	WCNM((m_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv")),
		"Failed to initialize glVertexAttrib4fv extension");

	WCNM((m_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray")),
		"Failed to initialize glEnableVertexAttribArray extension");

	WCNM((m_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation")),
		"Failed to initialize glBindAttribLocation extension");

	// Blending

	WCNM((m_glBlendEquation = (PFNGLBLENDEQUATIONPROC)wglGetProcAddress("glBlendEquation")),
		"Failed to initialize glBlendEquation extension");

	WCNM((m_glBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATEPROC)wglGetProcAddress("glBlendFuncSeparate")),
		"Failed to initialize glBlendFuncSeparate extension");

	// Not supported yet?
	m_glDisableVertexAttribArray = NULL;
	m_glGetActiveUniform = NULL;

	// Shader
	WCNM((m_glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader")),
		"Failed to initialize glCreateShader extension");

	WCNM((m_glCreateShaderObject = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB")),
		"Failed to initialize glCreateShaderObjectARB extension");

	WCNM((m_glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader")),
		"Failed to initialize glDeleteShader extension");

	WCNM((m_glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource")),
		"Failed to initialize glShaderSource extension");

	WCNM((m_glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader")),
		"Failed to initialize glCompileShader extension");

	WCNM((m_glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv")),
		"Failed to initialize glGetShaderiv extension");

	// Textures
	/*
	CNMW((m_glGenTextures = (PFNGLGENTEXTURESPROC)wglGetProcAddress("glGenTextures")),
		"Failed to initialize glGenTextures extension");
	*/

	WCNM((m_glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture")),
		"Failed to initialize glActiveTexture extension");
	
	WCNM((m_glBindTextures = (PFNGLBINDTEXTURESPROC)wglGetProcAddress("glBindTextures")),
		"Failed to initialize glBindTextures extension");

	WCNM((m_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D")),
		"Failed to initialize glTexStorage2D extension");

	WCNM((m_glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample")),
		"Failed to initialize glTexImage2DMultisample extension");

	//CNMW((m_glTexParameteri = (PFNGLTEXPARAMETERIPROC)wglGetProcAddress("glTexParameteri")),
	//	"Failed to initialize glTexParameteri extension");

	//CNMW((m_glTexImage2D = (PFNGLTEXIMAGE2DPROC)wglGetProcAddress("glTexImage2D")),
	//	"Failed to initializm_glTexImage2D extension");

	WCNM((m_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap")),
		"Failed to initialize glGenerateMipmap extension");

	WCNM((m_glGetnTexImage = (PFNGLGETNTEXIMAGEPROC)wglGetProcAddress("glGetnTexImage")),
		"Failed to initialize glGetnTexImage extension");

	WCNM((m_glGetTextureImage = (PFNGLGETTEXTUREIMAGEPROC)wglGetProcAddress("glGetTextureImage")),
		"Failed to initialize glGetTextureImage extension");

	// FBO
	WCNM((m_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers")),
		"Failed to initialzie glGenFramebuffers extension");

	WCNM((m_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer")),
		"Failed to initialize glBindFramebuffer extension");

	WCNM((m_glDeleteFrameBuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers")),
		"Failed to initialize glDeleteFramebuffers extension");

	WCNM((m_glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers")),
		"Failed to initialize glGenRenderbuffers extension");

	WCNM((m_glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers")),
		"Failed to initialize glDeleteRenderbuffers extension");

	WCNM((m_glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer")),
		"Failed to initialize glBindRenderbuffer extension");

	WCNM((m_glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage")),
		"Failed to initialize glRenderbufferStorage extension");

	WCNM((m_glRenderBufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)wglGetProcAddress("glRenderbufferStorageMultisample")),
		"Failed to initialize glRenderBufferStorageMultisample extension");

	WCNM((m_glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer")),
		"Failed to initialize glFramebufferRenderbuffer extension");

	WCNM((m_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus")),
		"Failed to initialize glCheckFramebufferStatus extension");

	WCNM((m_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D")),
		"Failed to initialize glFramebufferTexture2D extension");

	WCNM((m_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer")),
		"Failed to initialize glBlitFramebuffer extension");

	WCNM((m_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)wglGetProcAddress("glFramebufferTexture")),
		"Failed to initialize glFramebufferTexture extension");

	// PBO

	WCNM((m_glMapBuffer = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer")),
		"Failed to initialize glMapBuffer extension");

	WCNM((m_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer")),
		"Failed to initialize glMapBuffer extension");

	// VBO
	WCNM((m_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers")),
		"Failed to initialzie glGenBuffers extension");

	WCNM((m_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer")),
		"Failed to initialize glBindBuffer extension");

	WCNM((m_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData")),
		"Failed to initialize glBuifferData extension");

	WCNM((m_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData")),
		"Failed to initialize glBuifferSubData extension");

	WCNM((m_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer")),
		"Failed to initialize glVertexAttribPointer extension");

	WCNM((m_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers")),
		"Failed to initialize glDeleteBuffers extension");

	WCNM((m_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers")),
		"Failed to initialize glDrawBuffers extension");

	// VAO
	WCNM((m_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays")),
		"Failed to initialize glGenVertexArrays extension");

	WCNM((m_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray")),
		"Failed to initialize glBindVertexArray extension");

	WCNM((m_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays")),
		"Failed to initialize glDeleteVertexArrays extension");

	// Queries 
	WCNM((m_glGenQueries = (PFNGLGENQUERIESPROC)wglGetProcAddress("glGenQueries")),
		"Failed to initialize glGenQueries extension");

	WCNM((m_glDeleteQueries = (PFNGLDELETEQUERIESPROC)wglGetProcAddress("glDeleteQueries")),
		"Failed to initialize glDeleteQueries extension");

	WCNM((m_glIsQuery = (PFNGLISQUERYPROC)wglGetProcAddress("glIsQuery")),
		"Failed to initialize glIsQuery extension");

	WCNM((m_glBeginQuery = (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery")),
		"Failed to initialize glBeginQuery extension");

	WCNM((m_glEndQuery = (PFNGLENDQUERYPROC)wglGetProcAddress("glEndQuery")),
		"Failed to initialize glEndQuery extension");

	WCNM((m_glGetQueryiv = (PFNGLGETQUERYIVPROC)wglGetProcAddress("glGetQueryiv")),
		"Failed to initialize glGetQueryiv extension");

	WCNM((m_glGetQueryObjectiv = (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress("glGetQueryObjectiv")),
		"Failed to initialize glGetQueryObjectiv extension");

	WCNM((m_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuiv")),
		"Failed to initialize glGetQueryObjectuiv extension");

	return r;
}