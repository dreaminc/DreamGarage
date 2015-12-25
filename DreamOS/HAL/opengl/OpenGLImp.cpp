#include "OpenGLImp.h"


OpenGLImp::OpenGLImp(HDC hDC) :
	m_idOpenGLProgram(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL)
{
	ACRM(InitializeExtensions(), "Failed to initialize extensions");
	e_hDC = hDC;

	ACRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
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
	m_glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	m_glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	m_glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	m_glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	m_glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	m_glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	m_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	m_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	m_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	m_glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	m_glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	m_glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	m_glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	m_glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	m_glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	m_glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	m_glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	m_glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	m_glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	m_glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	m_glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	m_glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	m_glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	m_glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	m_glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	m_glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	m_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	m_glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	
	// Not supported yet?
	m_glDisableVertexAttribArray = NULL;
	m_glGetActiveUniform = NULL;

	// Shader
	m_glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	m_glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	m_glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	m_glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	m_glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

	// VBO
	m_glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	m_glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	m_glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	m_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

	return R_PASS;
}