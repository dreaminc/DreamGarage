#include "OpenGLImp.h"


OpenGLImp::OpenGLImp(HDC hDC) {
	m_ID = m_glCreateProgram();
	e_hDC = hDC;

	ACRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
}

OpenGLImp::~OpenGLImp() {
	m_glDeleteProgram(m_ID);
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

Error:
	return r;
}

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	if (pxHeight <= 0)
		pxHeight = 1;

	int aspectratio = pxWidth / pxHeight;

	/* TODO: Resize and such
	glViewport(0, 0, pxWidth, pxHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspectratio, 0.2f, 255.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

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

	return R_PASS;
}