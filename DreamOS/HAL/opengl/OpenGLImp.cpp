#include "OpenGLImp.h"

#include "Sandbox/SandboxApp.h"
#include "Sandbox/Windows/Windows64App.h"	

//OpenGLImp::OpenGLImp(HDC hDC) :
OpenGLImp::OpenGLImp(Windows64App *pWindows64App) :
	m_idOpenGLProgram(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL),
	//e_hDC(hDC),
	m_pWindows64App(pWindows64App)
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

	//int nPixelFormat = ChoosePixelFormat(e_hDC, &pfd);
	int nPixelFormat = ChoosePixelFormat(m_pWindows64App->GetDeviceContext(), &pfd);

	CBM((nPixelFormat != NULL), "nPixelFormat is NULL");
	//CBM((SetPixelFormat(e_hDC, nPixelFormat, &pfd)), "Failed to SetPixelFormat");
	CBM((SetPixelFormat(m_pWindows64App->GetDeviceContext(), nPixelFormat, &pfd)), "Failed to SetPixelFormat");

	//m_hglrc = wglCreateContext(e_hDC);
	m_hglrc = wglCreateContext(m_pWindows64App->GetDeviceContext());
	ACNM(m_hglrc, "Failed to Create GL Context");

	//CBM((wglMakeCurrent(e_hDC, m_hglrc)), "Failed OGL wglMakeCurrent");
	CBM((wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc)), "Failed OGL wglMakeCurrent");

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

	if (wglCreateContextAttribsARB != NULL) {
		//m_hglrc = wglCreateContextAttribsARB(e_hDC, 0, attribs);
		m_hglrc = wglCreateContextAttribsARB(m_pWindows64App->GetDeviceContext(), 0, attribs);
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrcTemp);

	CNM(m_hglrc, "OpenGL 3.x RC was not created!");

Error:
	return r;
}

RESULT OpenGLImp::AttachShader(OpenGLShader *pOpenGLShader) {
	RESULT r = R_PASS;

	CNM(m_glAttachShader, "glAttachShader extension is NULL");

	m_glAttachShader(m_idOpenGLProgram, pOpenGLShader->GetShaderID());

	DEBUG_LINEOUT("Attached shader %d", pOpenGLShader->GetShaderID());

Error:
	return r;
}

RESULT OpenGLImp::BindAttribLocation(unsigned int index, char* pszName) {
	RESULT r = R_PASS;
	unsigned int err;
	DWORD werr;

	CNM(m_glBindAttribLocation, "glBindAttribLocation extension is NULL");

	m_glBindAttribLocation(m_idOpenGLProgram, index, pszName);

	// Check for errors
	// TODO: Actually check these
	err = glGetError();
	werr = GetLastError();

Error:
	return r;
}

RESULT OpenGLImp::UseProgram() {
	RESULT r = R_PASS;

	CNM(m_glUseProgram, "glUseProgram extension is NULL");
	m_glUseProgram(m_idOpenGLProgram);

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
	
	// TODO: REALLY BAD - identifying potential driver bug
	/*
	GLint param = GL_FALSE;
	m_glGetProgramiv(m_idOpenGLProgram, GL_LINK_STATUS, &param);

	CBM((param == GL_TRUE), "Failed to link GL Program: %s", GetInfoLog());

	DEBUG_LINEOUT("Successfully linked program ID %d", m_idOpenGLProgram);
	*/

Error:
	return r;
}

// This is temporary - replace with ObjectStore architecture soon
RESULT OpenGLImp::SetData() {
	RESULT r = R_PASS;

	float* vert = new float[9]; // vertex array
	float* col = new float[9]; // color array

	vert[0] = 0.0f; 
	vert[1] = 0.8f; 
	vert[2] = -1.0f;

	vert[3] = -0.8f; 
	vert[4] = -0.8f; 
	vert[5] = -1.0f;

	vert[6] = 0.8f; 
	vert[7] = -0.8f; 
	vert[8] = -1.0f;

	col[0] = 1.0f; 
	col[1] = 0.0f; 
	col[2] = 0.0f;

	col[3] = 0.0f; 
	col[4] = 1.0f; 
	col[5] = 0.0f;

	col[6] = 0.0f; 
	col[7] = 0.0f; 
	col[8] = 1.0f;

	m_glGenBuffers(2, &m_vboID[0]);

	m_glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
	m_glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vert, GL_STATIC_DRAW);
	m_glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	m_glEnableVertexAttribArray(0);

	m_glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
	m_glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), col, GL_STATIC_DRAW);
	m_glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	m_glEnableVertexAttribArray(1);

	delete[] vert;
	delete[] col;

Error:
	return r;
}

RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;

	//wglMakeCurrent(e_hDC, m_hglrc);
	wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc);

	// Clear Background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	m_pVertexShader = new OpenGLShader(this, GL_VERTEX_SHADER);
	m_pFragmentShader = new OpenGLShader(this, GL_FRAGMENT_SHADER);

	// Load the vertex shader
	// TODO: More complex shader handling - right now statically calling minimal shader
	// TODO: Likely put into factory
	CRM(m_pVertexShader->LoadFromFile(L"minimal.vert"), "Failed to load minimal vertex shader");
	CRM(m_pFragmentShader->LoadFromFile(L"minimal.frag"), "Failed to load minimal fragment shader");

	// Compile the shaders
	CRM(m_pVertexShader->Compile(), "Failed to compile vertex shader");
	CRM(m_pFragmentShader->Compile(), "Failed to compile fragment shader");

	// Attach the shaders
	CRM(AttachShader(m_pVertexShader), "Failed to attach vertex shader");
	CRM(AttachShader(m_pFragmentShader), "Failed to attach fragment shader");

	// Some shader routing
	CRM(BindAttribLocation(0, "in_Position"), "Failed to bind in_Position attribute");
	CRM(BindAttribLocation(1, "in_Color"), "Failed to bind in_Color attribute");

	// Link OpenGL Program
	// TODO: Fix the error handling here (driver issue?)
	CRM(LinkProgram(), "Failed to link program");
	CRM(UseProgram(), "Failed to use open gl program");

	// TODO: Temporary, get some data into the funnel for now
	CRM(SetData(), "Failed to set some data");

Error:
	wglMakeCurrent(NULL, NULL);
	return r;
}

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;
	
	///*
	if (pxHeight <= 0)
		pxHeight = 1;

	int aspectratio = pxWidth / pxHeight;

	glViewport(0, 0, pxWidth, pxHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspectratio, 0.2f, 255.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//*/

	/*
	wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc);
	glViewport(0, 0, (GLsizei)pxWidth, (GLsizei)pxHeight);
	*/

Error:
	wglMakeCurrent(NULL, NULL);
	return r;
}

RESULT OpenGLImp::Render() {
	RESULT r = R_PASS;

	wglMakeCurrent(m_pWindows64App->GetDeviceContext(), m_hglrc);
	
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	glFlush();
	//SwapBuffers(m_pWindows64App->GetDeviceContext());

Error:
	wglMakeCurrent(NULL, NULL);
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

Error:
	return r;
}