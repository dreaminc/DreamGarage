#include "OpenGLImp.h"

#include "Sandbox/SandboxApp.h"
#include "Sandbox/Windows/Windows64App.h"	

OpenGLImp::OpenGLImp(Windows64App *pWindows64App) :
	m_idOpenGLProgram(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL),
	m_pWindows64App(pWindows64App),
	m_hglrc(NULL)
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

SandboxApp* OpenGLImp::GetParentApp() {		
	return (SandboxApp*)(m_pWindows64App); 
}

RESULT OpenGLImp::CreateGLProgram() {
	RESULT r = R_PASS;

	CBM((m_idOpenGLProgram == NULL), "Cannot CreateGLProgram if program id not null");
	CNM(m_glCreateProgram, "glCreateProgram extension is NULL");

	m_idOpenGLProgram = glCreateProgram();
	CBM((m_idOpenGLProgram != 0), "Failed to create program id");

	GLboolean fIsProg = glIsProgram(m_idOpenGLProgram);
	CBM(fIsProg, "Failed to create program");

	DEBUG_LINEOUT("Created GL program ID %d", m_idOpenGLProgram);

Error:
	return r;
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

	int nPixelFormat = ChoosePixelFormat(m_pWindows64App->GetDeviceContext(), &pfd);

	CBM((nPixelFormat != NULL), "nPixelFormat is NULL");
	CBM((SetPixelFormat(m_pWindows64App->GetDeviceContext(), nPixelFormat, &pfd)), "Failed to SetPixelFormat");

	HGLRC hglrcTemp = wglCreateContext(m_pWindows64App->GetDeviceContext());
	CNM(hglrcTemp, "Failed to Create GL Context");

	CBM((wglMakeCurrent(m_pWindows64App->GetDeviceContext(), hglrcTemp)), "Failed OGL wglMakeCurrent");

	// Should be called after context is created and made current
	ACRM(InitializeExtensions(), "Failed to initialize extensions");
	
	InitializeOpenGLVersion();

	if (m_versionMajor < 3 || (m_versionMajor == 3 && m_versionMinor < 2)) {
		DEBUG_LINEOUT("OpenGL 3.2+ Not Supported");
		m_hglrc = hglrcTemp;
		goto Error;
	}

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, m_versionMajor,
		WGL_CONTEXT_MINOR_VERSION_ARB, m_versionMinor,
		//WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		//WGL_CONTEXT_MINOR_VERSION_ARB, 2,		
		//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,		// This makes non VAO work
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	CNM(wglCreateContextAttribsARB, "wglCreateContextAttribsARB cannot be NULL");

	m_hglrc = wglCreateContextAttribsARB(m_pWindows64App->GetDeviceContext(), 0, attribs);
	DWORD werr = GetLastError();
	DEBUG_LINEOUT("Created OpenGL Rendering Context 0x%x", werr);
	
	// Should be called after context is created and made current
	ACRM(InitializeExtensions(), "Failed to initialize extensions");

	CBM(wglMakeCurrent(NULL, NULL), "Failed to release rendering context");
	CBM(wglDeleteContext(hglrcTemp), "Failed to delete temporary rendering context");

	CNM(m_hglrc, "OpenGL 3.x RC was not created!");

Error:
	return r;
}

RESULT OpenGLImp::CheckGLError() {
	RESULT r = R_PASS;

	GLenum glerr = glGetError();
	switch (glerr) {
		case GL_NO_ERROR: return R_PASS; break;
		case GL_INVALID_ENUM: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_ENUM"); break;
		case GL_INVALID_VALUE: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_VALUE"); break;
		case GL_INVALID_OPERATION: CBRM(false, R_FAIL, "CheckGLError: GL_INVALID_OPERATION"); break;
		case GL_STACK_OVERFLOW: CBRM(false, R_FAIL, "CheckGLError: GL_STACK_OVERFLOW"); break;
		case GL_STACK_UNDERFLOW: CBRM(false, R_FAIL, "CheckGLError: GL_STACK_UNDERFLOW"); break;
		case GL_OUT_OF_MEMORY: CBRM(false, R_FAIL, "CheckGLError: GL_OUT_OF_MEMORY"); break;
	}

Error:
	return r;
}

RESULT OpenGLImp::AttachShader(OpenGLShader *pOpenGLShader) {
	RESULT r = R_PASS;
	GLint param;
	GLint numShaders;
	GLenum glerr = GL_NO_ERROR;

	OpenGLShader *&pOGLShader = this->m_pVertexShader;

	switch (pOpenGLShader->GetShaderType()) {
		case GL_VERTEX_SHADER: {
			pOGLShader = this->m_pVertexShader;
		} break;

		case GL_FRAGMENT_SHADER: {
			pOGLShader = this->m_pFragmentShader;
		} break;

		default: {
			CBM((0), "Shader type 0x%x cannot be attached", pOpenGLShader->GetShaderType());
		}
	}

	CBM((pOGLShader == NULL), "Current shader 0x%x already assigned, detach existing shader", pOpenGLShader->GetShaderType());

	CNM(m_glAttachShader, "glAttachShader extension is NULL");

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//numShaders = param;

	m_glAttachShader(m_idOpenGLProgram, pOpenGLShader->GetShaderID());

	CRM(CheckGLError(), "AttachShader failed with GL log:%s", pOpenGLShader->GetInfoLog());

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//CBM((param = numShaders + 1), "Failed to attach shader, num shaders attached %d", param);

	// Assign the shader to the implementation stage
	pOGLShader = pOpenGLShader;
	DEBUG_LINEOUT("Attached shader %d type 0x%x", pOpenGLShader->GetShaderID(), pOpenGLShader->GetShaderType());

Error:
	return r;
}

RESULT OpenGLImp::BindAttribLocation(unsigned int index, char* pszName) {
	RESULT r = R_PASS;
	GLenum glerr;
	DWORD werr;

	CNM(m_glBindAttribLocation, "glBindAttribLocation extension is NULL");

	m_glBindAttribLocation(m_idOpenGLProgram, index, pszName);

	CRM(CheckGLError(), "BindAttribLocation failed");

	werr = GetLastError();
	DEBUG_LINEOUT("Bound attribute %s to index location %d err:0x%x", pszName, index, werr);

Error:
	return r;
}

RESULT OpenGLImp::UseProgram() {
	RESULT r = R_PASS;

	CNM(m_glUseProgram, "glUseProgram extension is NULL");
	m_glUseProgram(m_idOpenGLProgram);

	CRM(CheckGLError(), "UseProgram failed");

Error:
	return r;
}

// TODO: Inconsistent since it doesn't check to see that m_glGetPRogramInfoLog is not NULL
char* OpenGLImp::GetInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = NULL;
	//int pszInfoLog_n = -1;
	int pszInfoLog_n = 4096;
	int charsWritten_n = -1;

	m_glGetProgramiv(m_idOpenGLProgram, GL_INFO_LOG_LENGTH, &pszInfoLog_n);
	CBM((pszInfoLog_n > 0), "Program Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	memset(pszInfoLog, 0, sizeof(char) * pszInfoLog_n);
	m_glGetProgramInfoLog(m_idOpenGLProgram, pszInfoLog_n, &charsWritten_n, pszInfoLog);

Error:
	return pszInfoLog;
}

RESULT OpenGLImp::LinkProgram() {
	RESULT r = R_PASS;

	CNM(m_glLinkProgram, "glLinkProgram extension is NULL");
	CNM(m_glGetProgramiv, "glGetProgramiv extension is NULL");

	m_glLinkProgram(m_idOpenGLProgram);
	CRM(CheckGLError(), "glLinkProgram failed");
	
	GLint param = GL_FALSE;

	m_glGetProgramiv(m_idOpenGLProgram, GL_LINK_STATUS, &param);
	CBM((param == GL_TRUE), "Failed to link GL Program: %s", GetInfoLog());
	
	DEBUG_LINEOUT("Successfully linked program ID %d", m_idOpenGLProgram);

Error:
	return r;
}

RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	CBM(wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc), "Failed to make current rendering context");

	// Clear Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CRM(CreateGLProgram(), "Failed to create GL program");
	CRM(CheckGLError(), "CreateGLProgram failed");

	// TODO: Should be stuffed into factory arch - return NULL on fail
	// TODO: More complex shader handling - right now statically calling minimal shader
	// TODO: Likely put into factory
	OpenGLShader *pVertexShader = new OpenGLShader(this, GL_VERTEX_SHADER);
	CRM(CheckGLError(), "Create OpenGL Vertex Shader failed");
	CRM(pVertexShader->InitializeFromFile(L"minimal.vert"), "Failed to initialize vertex shader from file");

	OpenGLShader *pFragmentShader = new OpenGLShader(this, GL_FRAGMENT_SHADER);
	CRM(CheckGLError(), "Create OpenGL Fragment Shader failed");
	CRM(pFragmentShader->InitializeFromFile(L"minimal.frag"), "Failed to initialize fragment shader from file");

	// Some Shader Routing
	CRM(BindAttribLocation(0, "in_Position"), "Failed to bind in_Position attribute");
	CRM(BindAttribLocation(1, "in_Color"), "Failed to bind in_Color attribute");
	
	// Link OpenGL Program
	// TODO: Fix the error handling here (driver issue?)
	CRM(LinkProgram(), "Failed to link program");
	CRM(UseProgram(), "Failed to use open gl program");

	// TODO: Temporary, get some data into the funnel for now
	CRM(SetData(), "Failed to set some data");

Error:
	if (!wglMakeCurrent(NULL, NULL))
		DEBUG_LINEOUT("Failed to release rendering context");

	return r;
}

#include "Primitives/ProjectionMatrix.h"

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	// Attempt to play with a projection matrix
	DEBUG_LINEOUT("Setting Projection Matrix width %d height %d", pxWidth, pxHeight);
	ProjectionMatrix projMatrix(PROJECTION_MATRIX_PERSPECTIVE, pxWidth, pxHeight, 1.0f, 100.0f, 45.0f);
	projMatrix.PrintMatrix();

	/*
	projMatrix.NumbersByElement(0.0f, 1.0f);
	projMatrix.PrintMatrix();

	projMatrix.Numbers(0.0f, 1.0f);
	projMatrix.PrintMatrix();

	for (int i = 0; i < projMatrix.rows(); i++)
		for (int j = 0; j < projMatrix.cols(); j++)
			projMatrix.element(i, j) = i * projMatrix.cols() + j;
	projMatrix.PrintMatrix();

	for (int i = 0; i < projMatrix.rows(); i++)
		for (int j = 0; j < projMatrix.cols(); j++)
			projMatrix(i, j) = i * projMatrix.cols() + j;
	projMatrix.PrintMatrix();
	*/

	/*
	for (int i = 0; i < projMatrix.rows(); i++)
		for (int j = 0; j < projMatrix.cols(); j++)
			projMatrix[i][j] = i * projMatrix.cols() + j;
	projMatrix.PrintMatrix();
	*/

	CBM(wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc), "Failed to make current rendering context");	
	glViewport(0, 0, (GLsizei)pxWidth, (GLsizei)pxHeight);

Error:
	if (!wglMakeCurrent(NULL, NULL))
		DEBUG_LINEOUT("Failed to release rendering context");

	return r;
}
// TODO: Get this out of here
#include "Primitives/Vertex.h"
#include "OGLTriangle.h"
#include "Primitives/color.h"

OGLTriangle *g_pTriangle = NULL;

// This is temporary - replace with ObjectStore architecture soon
RESULT OpenGLImp::SetData() {
	RESULT r = R_PASS;

	float z = 0.0f;
	float height = 0.8f;
	float width = 0.8f;
	vertex vertTemp[3];
	vertTemp[0].SetPoint(0.0f, height, z);
	vertTemp[0].SetColor(1.0f, 0.0f, 0.0f);

	vertTemp[1].SetPoint(-width, -height, z);
	vertTemp[1].SetColor(0.0f, 1.0f, 0.0f);

	vertTemp[2].SetPoint(width, -height, z);
	vertTemp[2].SetColor(0.0f, 0.0f, 1.0f);
	
	g_pTriangle = new OGLTriangle(this, 0.8f);

	// TODO: Update this so that any changes force a change?
	g_pTriangle->CopyVertices(vertTemp, 3);
	g_pTriangle->UpdateOGLBuffers();

Error:
	return r;
}

RESULT OpenGLImp::Render() {
	RESULT r = R_PASS;

	CBM(wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc), "Failed to make current rendering context");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	g_pTriangle->Render();
	
	/*
	GLfloat z = 0.0f;
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		glVertex3f(0.0f, 1.0f, z);              // Top
		glVertex3f(-1.0f, -1.0f, z);              // Bottom Left
		glVertex3f(1.0f, -1.0f, z);              // Bottom Right
	glEnd();                            // Finished Drawing The Triangle
	//*/
	
	glFlush();
	//SwapBuffers(m_pWindows64App->GetDeviceContext()); // This is done in the App

Error:
	if (!wglMakeCurrent(NULL, NULL))
		DEBUG_LINEOUT("Failed to release rendering context");

	return r;
}

RESULT OpenGLImp::ShutdownImplementaiton() {
	RESULT r = R_PASS;

	CBM((wglDeleteContext(m_hglrc)), "Failed to wglDeleteContext(hglrc)");

	/* TODO:  Add this stuff
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//--------------------------------
	m_pProgram->DetachShader(m_pVertSh);
	m_pProgram->DetachShader(m_pFragSh);

	delete m_pProgram;
	m_pProgram = NULL;

	delete m_pVertSh;
	m_pVertSh = NULL;
	delete m_pFragSh;
	m_pFragSh = NULL;

	wglMakeCurrent(NULL, NULL);
	//--------------------------------
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
	*/

Error:
	return r;
}

// Open GL / Wrappers

RESULT OpenGLImp::glGenVertexArrays(GLsizei n, GLuint *arrays) {
	RESULT r = R_PASS;

	m_glGenVertexArrays(n, arrays);  //create VAO container and get ID for it
	CRM(CheckGLError(), "glGenVertexArrays failed");

Error:
	return r;
}

// Bind Array to OpenGL context
RESULT OpenGLImp::glBindVertexArray(GLuint gluiArray) {
	RESULT r = R_PASS;

	m_glBindVertexArray(gluiArray);
	CRM(CheckGLError(), "glBindVertexArray failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenBuffers(GLsizei n, GLuint *buffers) {
	RESULT r = R_PASS;

	m_glGenBuffers(n, buffers);
	CRM(CheckGLError(), "glGenBuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindBuffer(GLenum target, GLuint gluiBuffer) {
	RESULT r = R_PASS;

	m_glBindBuffer(target, gluiBuffer);
	CRM(CheckGLError(), "glBindBuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
	RESULT r = R_PASS;

	m_glBufferData(target, size, data, usage);
	CRM(CheckGLError(), "glBufferData failed");

Error:
	return r;
}

RESULT OpenGLImp::glEnableVertexAtrribArray(GLuint index) {
	RESULT r = R_PASS;

	m_glEnableVertexAttribArray(index);
	CRM(CheckGLError(), "glEnableVertexAttribArray failed");

Error:
	return r;
}

RESULT OpenGLImp::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
	RESULT r = R_PASS;

	m_glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	CRM(CheckGLError(), "glVertexAttribPointer failed");

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

	CNMW((m_glIsProgram = (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram")),
		"Failed to initialize glIsProgram extension");

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

	// VAO
	CNMW((m_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays")),
		"Failed to initialize glGenVertexArrays extension");

	CNMW((m_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray")),
		"Failed to initialize glBindVertexArray extension");

Error:
	return r;
}