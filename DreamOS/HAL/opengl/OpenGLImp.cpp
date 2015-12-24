#include "OpenGLImp.h"


OpenGLImp::OpenGLImp(HDC hDC) :
	m_ID(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0)
{
	ACRM(InitializeExtensions(), "Failed to initialize extensions");
	e_hDC = hDC;

	ACRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
}

OpenGLImp::~OpenGLImp() {
	m_glDeleteProgram(m_ID);
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

	// Move this eventually?
	if (m_glCreateProgram != NULL)
		m_ID = m_glCreateProgram();

	InitializeOpenGLVersion();

	/*
	if (m_versionMajor < 3 || (m_versionMajor == 3 && m_versionMinor < 2))
		AfxMessageBox(_T("OpenGL 3.2 is not supported!"));

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, m_versionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, m_versionMinor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB != NULL)
		m_hrc = wglCreateContextAttribsARB(pDC->m_hDC, 0, attribs);

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);


	if (!m_hrc) {
		AfxMessageBox(_T("OpenGL 3.x RC was not created!"));
		return R_FAIL;
	}
	*/

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

	/*
	// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");

	// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

	// VBO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	*/

	return R_PASS;
}