#include "OpenGLImp.h"


OpenGLImp::OpenGLImp(HDC hDC) :
	m_idOpenGLProgram(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL),
	e_hDC(hDC)
{
	ACRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
	ACRM(PrepareScene(), "Failed to prepare GL Scene");
}

OpenGLImp::~OpenGLImp() {
	glDeleteProgram(m_idOpenGLProgram);
}

RESULT OpenGLImp::InitializeOpenGLVersion() {
	// For all versions
	char* pszVersion = (char*)glGetString(GL_VERSION); // ver = "3.2.0"
	DEBUG_LINEOUT("OpenGL Version %s", pszVersion);

	m_versionMajor = pszVersion[0] - '0';
	m_versionMinor = pszVersion[2] - '0';

	// GL 3.x+
	if (m_versionMajor >= 3) {
		glGetIntegerv(GL_MAJOR_VERSION, &m_versionMajor); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, &m_versionMinor); // minor = 2
	}

	// GLSL
	// "1.50 NVIDIA via Cg compiler"
	// TODO: Parse this out for m_versionGLSL
	pszVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); 
	DEBUG_LINEOUT("OpenGL GLSL Version %s", pszVersion);

	return R_PASS;
}

RESULT OpenGLImp::InitializeGLContext() {
	RESULT r = R_PASS;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(e_hDC, &pfd);

	CBM((nPixelFormat != NULL), "nPixelFormat is NULL");
	CBM((SetPixelFormat(e_hDC, nPixelFormat, &pfd)), "Failed to SetPixelFormat");

	m_hglrc = wglCreateContext(e_hDC);
	ACNM(m_hglrc, "Failed to Create GL Context");

	CBM((wglMakeCurrent(e_hDC, m_hglrc)), "Failed OGL wglMakeCurrent");

	InitializeOpenGLVersion();

	// Should be called after context is created and made current
	ACRM(InitializeExtensions(), "Failed to initialize extensions");

	// Move this eventually?
	if (m_glCreateProgram != NULL)
		m_idOpenGLProgram = glCreateProgram();

	if (m_versionMajor < 3 || (m_versionMajor == 3 && m_versionMinor < 2)) {
		DEBUG_LINEOUT("OpenGL 3.2+ Not Supported");
		goto Error;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, m_versionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, m_versionMinor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	// Save HGLRC to temp
	HGLRC hglrcTemp = m_hglrc;

	if (wglCreateContextAttribsARB != NULL)
		m_hglrc = wglCreateContextAttribsARB(e_hDC, 0, attribs);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrcTemp);

	CNM(m_hglrc, "OpenGL 3.x RC was not created!");

Error:
	return r;
}

RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;

	wglMakeCurrent(e_hDC, m_hglrc);

	// Clear Background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	m_pVertexShader = new OpenGLShader(this, GL_VERTEX_SHADER);
	m_pFragmentShader = new OpenGLShader(this, GL_FRAGMENT_SHADER);

	// Load the vertex shader


Error:
	return r;
}

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	if (pxHeight <= 0)
		pxHeight = 1;

	int aspectratio = pxWidth / pxHeight;

	glViewport(0, 0, pxWidth, pxHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspectratio, 0.2f, 255.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

Error:
	return r;
}

RESULT OpenGLImp::Render() {
	RESULT r = R_PASS;



Error:
	return r;
}

RESULT OpenGLImp::ShutdownImplementaiton() {
	RESULT r = R_PASS;

	CBM((wglDeleteContext(m_hglrc)), "Failed to wglDeleteContext(hglrc)");

Error:
	return r;
}

// Initialize all of the extensions
// TODO: Stuff this into an object?
RESULT OpenGLImp::InitializeExtensions() {
	RESULT r = R_PASS;

	CNMW((m_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram")), 
		"Failed to initialzie glCreateProgram extension");

	CNMW((m_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram")), 
		"Failed to initialize glDeleteProgram extension");

	CNMW((m_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram")), 
		"Failed to initialzie glUseProgram extension");

	CNMW((m_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader")), 
		"Failed to initialize glAttachShader extension");

	CNMW((m_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader")), 
		"Failed to initialize glDetachShader extension");

	CNMW((m_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram")), 
		"Failed to initialize glLinkProgram extension");

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

	// VBO
	CNMW((m_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers")), 
		"Failed to initialzie glGenBuffers extension");

	CNMW((m_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer")), 
		"Failed to initialize glBindBuffer extension");

	CNMW((m_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData")), 
		"Failed to initialize glBuifferData extension");
	
	CNMW((m_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer")), 
		"Failed to initialize glVertexAttribPointer extension");

Error:
	return r;
}