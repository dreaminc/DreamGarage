#include "OpenGLImp.h"
#include "OGLObj.h"

#include "Primitives/ProjectionMatrix.h"
#include "Primitives/TranslationMatrix.h"
#include "Primitives/RotationMatrix.h"

OpenGLImp::OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext) :
	m_idOpenGLProgram(NULL),
	m_versionOGL(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL),
	m_pOpenGLRenderingContext(pOpenGLRenderingContext),
	m_pCamera(NULL)
{
	RESULT r = R_PASS;

	CRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
	CRM(PrepareScene(), "Failed to prepare GL Scene");

	Validate();
	return;
Error:
	Invalidate();
	return;
}

OpenGLImp::~OpenGLImp() {
	m_OpenGLExtensions.glDeleteProgram(m_idOpenGLProgram);
}

RESULT OpenGLImp::InitializeOpenGLVersion() {
	// For all versions
	char* pszVersion = (char*)glGetString(GL_VERSION); // Ver = "3.2.0"
	int vMajor = 0, vMinor = 0, vDblMinor = 0;	// TODO: use minor?
	DEBUG_LINEOUT("OpenGL Version %s", pszVersion);

	vMajor = pszVersion[0] - '0';
	vMinor = pszVersion[2] - '0';
	vDblMinor = pszVersion[4] - '0';

	// GL 3.x+
	if (vMajor >= 3) {
		glGetIntegerv(GL_MAJOR_VERSION, &vMajor); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, &vMinor); // minor = 2
	}

	m_versionOGL.SetVersion(vMajor, vMinor);

	// GLSL
	// "1.50 NVIDIA via Cg compiler"
	pszVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); 
	DEBUG_LINEOUT("OpenGL GLSL Version %s", pszVersion);

	vMajor = pszVersion[0] - '0';
	vMinor = pszVersion[2] - '0';
	vDblMinor = pszVersion[4] - '0';

	m_versionGLSL.SetVersion(vMajor, vMinor);

	return R_PASS;
}

RESULT OpenGLImp::CreateGLProgram() {
	RESULT r = R_PASS;

	CBM((m_idOpenGLProgram == NULL), "Cannot CreateGLProgram if program id not null");

	m_idOpenGLProgram = m_OpenGLExtensions.glCreateProgram();
	CBM((m_idOpenGLProgram != 0), "Failed to create program id");

	GLboolean fIsProg = m_OpenGLExtensions.glIsProgram(m_idOpenGLProgram);
	CBM(fIsProg, "Failed to create program");

	DEBUG_LINEOUT("Created GL program ID %d", m_idOpenGLProgram);

Error:
	return r;
}

RESULT OpenGLImp::InitializeGLContext() {
	RESULT r = R_PASS;

	CRM(m_pOpenGLRenderingContext->InitializeRenderingContext(), "Failed to initialize oglrc");
	CR(InitializeOpenGLVersion());
	//CBM((m_versionMajor >= 3 || (m_versionMajor == 3 && m_versionMinor >= 2)), "OpenGL 3.2 + Not Supported");
	CBM((m_versionOGL >= 3.2f), "OpengL 3.2+ Not Supported");

	//CR(InitializeShadersFolder());

	// Should be called after context is created and made current
	ACRM(m_OpenGLExtensions.InitializeExtensions(), "Failed to initialize extensions");
	
	// Lets create the 3.2+ context
	CRM(m_pOpenGLRenderingContext->InitializeRenderingContext(m_versionOGL), "Failed to initialize oglrc");
	
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
	GLenum glerr = GL_NO_ERROR;

	OpenGLShader *&pOGLShader = (OpenGLShader *&)(this->m_pVertexShader);

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

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//numShaders = param;

	m_OpenGLExtensions.glAttachShader(m_idOpenGLProgram, pOpenGLShader->GetShaderID());

	CRM(CheckGLError(), "AttachShader failed with GL log:%s", pOpenGLShader->GetInfoLog());

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//CBM((param = numShaders + 1), "Failed to attach shader, num shaders attached %d", param);

	// Assign the shader to the implementation stage
	pOGLShader = pOpenGLShader;
	DEBUG_LINEOUT("Attached shader %d type 0x%x", pOpenGLShader->GetShaderID(), pOpenGLShader->GetShaderType());

Error:
	return r;
}

RESULT OpenGLImp::EnableVertexPositionAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexPositionAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OpenGLImp::EnableVertexColorAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexColorAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OpenGLImp::BindAttribLocation(unsigned int index, char* pszName) {
	RESULT r = R_PASS;
	DWORD werr;

	CR(glBindAttribLocation(m_idOpenGLProgram, index, pszName));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound attribute %s to index location %d err:0x%x", pszName, index, werr);

Error:
	return r;
}

RESULT OpenGLImp::UseProgram() {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUseProgram(m_idOpenGLProgram);

	CRM(CheckGLError(), "UseProgram failed");

Error:
	return r;
}

char* OpenGLImp::GetInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = NULL;
	int pszInfoLog_n = 4096;
	int charsWritten_n = -1;

	m_OpenGLExtensions.glGetProgramiv(m_idOpenGLProgram, GL_INFO_LOG_LENGTH, &pszInfoLog_n);
	CBM((pszInfoLog_n > 0), "Program Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	memset(pszInfoLog, 0, sizeof(char) * pszInfoLog_n);
	m_OpenGLExtensions.glGetProgramInfoLog(m_idOpenGLProgram, pszInfoLog_n, &charsWritten_n, pszInfoLog);

Error:
	return pszInfoLog;
}

RESULT OpenGLImp::LinkProgram() {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glLinkProgram(m_idOpenGLProgram);
	CRM(CheckGLError(), "glLinkProgram failed");
	
	GLint param = GL_FALSE;

	m_OpenGLExtensions.glGetProgramiv(m_idOpenGLProgram, GL_LINK_STATUS, &param);
	CBM((param == GL_TRUE), "Failed to link GL Program: %s", GetInfoLog());
	
	DEBUG_LINEOUT("Successfully linked program ID %d", m_idOpenGLProgram);

Error:
	return r;
}

// TODO: Might want to check this against the shader and find
// any mismatches? 
RESULT OpenGLImp::PrintVertexAttributes() {
	RESULT r = R_PASS;

	GLint attributes_n;
	CR(glGetProgramInterfaceiv(m_idOpenGLProgram, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &attributes_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	DEBUG_LINEOUT("%d active attributes", attributes_n);
	for (int i = 0; i < attributes_n; i++) {
		GLint results[3];
		CR(glGetProgramResourceiv(m_idOpenGLProgram, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results));

		GLint pszName_n = results[0] + 1;
		char *pszName = new char[pszName_n];
		CR(glGetProgramResourceName(m_idOpenGLProgram, GL_PROGRAM_INPUT, i, pszName_n, NULL, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", results[2], pszName, OpenGLUtility::GetOGLTypeString(results[1]));

		if (pszName != NULL) {
			delete[] pszName;
			pszName = NULL;
		}
	}

Error:
	return r; 
}

RESULT OpenGLImp::PrintActiveUniformVariables() {
	RESULT r = R_PASS;

	GLint variables_n = 0;
	CR(glGetProgramInterfaceiv(m_idOpenGLProgram, GL_UNIFORM, GL_ACTIVE_RESOURCES, &variables_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	DEBUG_LINEOUT("%d active uniform variables", variables_n);
	for (int i = 0; i < variables_n; i++) {
		GLint results[4];
		CR(glGetProgramResourceiv(m_idOpenGLProgram, GL_UNIFORM, i, 4, properties, 4, NULL, results));

		// Skip uniforms in blocks
		if (results[3] != -1) continue; 
		
		GLint pszName_n = results[0] + 1;
		char *pszName = new char[pszName_n];
		CR(glGetProgramResourceName(m_idOpenGLProgram, GL_UNIFORM, i, pszName_n, NULL, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", results[2], pszName, OpenGLUtility::GetOGLTypeString(results[1]));

		if (pszName != NULL) {
			delete[] pszName;
			pszName = NULL;
		}

	}

Error:
	return r;
}

RESULT OpenGLImp::MakeCurrentContext() {
	return m_pOpenGLRenderingContext->MakeCurrentContext();
}

RESULT OpenGLImp::ReleaseCurrentContext() {
	return m_pOpenGLRenderingContext->ReleaseCurrentContext();
}

RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	// Clear Background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	CRM(CreateGLProgram(), "Failed to create GL program");
	CRM(CheckGLError(), "CreateGLProgram failed");

	// Depth testing
	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDepthFunc(GL_LEQUAL);		// Accept fragment if it closer to the camera than the former one

	// Face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// TODO: Should be stuffed into factory arch - return NULL on fail
	// TODO: More complex shader handling - right now statically calling minimal shader
	// TODO: Likely put into factory
	// TODO: Move to GLSL version?
	OGLVertexShader *pVertexShader = new OGLVertexShader(this);
	CRM(CheckGLError(), "Create OpenGL Vertex Shader failed");
	CRM(pVertexShader->InitializeFromFile(L"minimal.vert", m_versionOGL), "Failed to initialize vertex shader from file");
	CR(pVertexShader->BindAttributes());

	OGLFragmentShader *pFragmentShader = new OGLFragmentShader(this);
	CRM(CheckGLError(), "Create OpenGL Fragment Shader failed");
	CRM(pFragmentShader->InitializeFromFile(L"minimal.frag", m_versionOGL), "Failed to initialize fragment shader from file");
	//CR(pFragmentShader->BindAttributes());
	
	// Link OpenGL Program
	// TODO: Fix the error handling here (driver issue?)
	CRM(LinkProgram(), "Failed to link program");
	CRM(UseProgram(), "Failed to use open gl program");

	CR(PrintVertexAttributes());
	CR(PrintActiveUniformVariables());

	// Allocate the camera
	m_pCamera = new stereocamera(point(0.0f, 0.0f, -10.0f), 45.0f, m_pxViewWidth, m_pxViewHeight);

	CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

Error:
	return r;
}

RESULT OpenGLImp::Resize(int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	m_pxViewWidth = pxWidth;
	m_pxViewHeight = pxHeight;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	glViewport(0, 0, (GLsizei)m_pxViewWidth, (GLsizei)m_pxViewHeight);

	m_pCamera->ResizeCamera(m_pxViewWidth, m_pxViewHeight);

Error:
	//CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

	return r;
}

// Assumes Context Current
RESULT OpenGLImp::SetStereoViewTarget(EYE_TYPE eye) {
	RESULT r = R_PASS;

	switch (eye) {
		case EYE_LEFT: {
			glViewport(0, 0, (GLsizei)m_pxViewWidth / 2, (GLsizei)m_pxViewHeight);
		} break;

		case EYE_RIGHT: {
			glViewport((GLsizei)m_pxViewWidth / 2, 0, (GLsizei)m_pxViewWidth / 2, (GLsizei)m_pxViewHeight);
		} break;

	}

	m_pCamera->ResizeCamera(m_pxViewWidth/2, m_pxViewHeight);

	return r;
}

camera * OpenGLImp::GetCamera() {
	return m_pCamera;
}

RESULT OpenGLImp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	/* This has been moved to the camera 
	DEBUG_LINEOUT("Rx kbe %d %d", kbEvent->KeyCode, kbEvent->KeyState);

	switch (kbEvent->KeyCode) {
		case (SK_SCAN_CODE)('A'):
		case SK_LEFT: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
			else
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
	
			if (kbEvent->KeyState)
				m_pCamera->Strafe(0.1f);

		} break;

		case (SK_SCAN_CODE)('D') :
		case SK_RIGHT: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(-0.1f, 0.0f, 0.0f);
			else
				m_pCamera->AddVelocity(0.1f, 0.0f, 0.0f);
			
			if (kbEvent->KeyState)
				m_pCamera->Strafe(-0.1f);
		} break;

		case (SK_SCAN_CODE)('W') :
		case SK_UP: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
			else
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
			
			if (kbEvent->KeyState)
				m_pCamera->MoveForward(0.1f);
		} break;

		case (SK_SCAN_CODE)('S') :
		case SK_DOWN: {
			if (kbEvent->KeyState)
				m_pCamera->AddVelocity(0.0f, 0.0f, -0.1f);
			else
				m_pCamera->AddVelocity(0.0f, 0.0f, 0.1f);
			
			if (kbEvent->KeyState)
				m_pCamera->MoveForward(-0.1f);
		} break;
	}
	*/

	return r;
}

// TODO: Move to camera?
RESULT OpenGLImp::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	SenseMouse::PrintEvent(mEvent);

	float MouseMoveFactor = 0.1f;

	switch (mEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			CR(m_pCamera->RotateCameraByDiffXY(static_cast<camera_precision>(mEvent->dx), 
				static_cast<camera_precision>(mEvent->dy)));
		} break;

		case SENSE_MOUSE_LEFT_BUTTON: {
			// TODO: Do something?
		} break;

		case SENSE_MOUSE_RIGHT_BUTTON: {
			// TODO: Do something?
		} break;
	}

Error:
	return r;
}

inline RESULT OpenGLImp::SendObjectToShader(DimObj *pDimObj) {
	OGLObj *pOGLObj = dynamic_cast<OGLObj*>(pDimObj);
	GLint locationProjectionMatrix = -1, locationViewMatrix = -1, locationModelMatrix = -1, locationModelViewProjectionMatrix = -1;

	// This is done once on the CPU side rather than per-vertex (although this in theory could be better precision) 
	auto matModel = pDimObj->GetModelMatrix();
	
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4Model", &locationModelMatrix);

	if (locationModelMatrix >= 0)
		glUniformMatrix4fv(locationModelMatrix, 1, GL_FALSE, (GLfloat*)(&matModel));

	return pOGLObj->Render();
}

RESULT OpenGLImp::UpdateCamera() {
	RESULT r = R_PASS;

	m_pCamera->UpdateCameraPosition();

	return r;
}

RESULT OpenGLImp::SetCameraMatrix(EYE_TYPE eye) {
	RESULT r = R_PASS;

	auto matVP = m_pCamera->GetProjectionMatrix() * m_pCamera->GetViewMatrix(eye);

	GLint locationViewProjectionMatrix = -1;
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4ViewProjection", &locationViewProjectionMatrix);

	if (locationViewProjectionMatrix >= 0)
		glUniformMatrix4fv(locationViewProjectionMatrix, 1, GL_FALSE, (GLfloat*)(&matVP));

	return r;
}

#include "OGLVolume.h"

// TODO: Other approach 
RESULT OpenGLImp::LoadScene(SceneGraph *pSceneGraph, TimeObj *pTimeObj) {
	RESULT r = R_PASS;

	OGLVolume *pVolume = NULL;
	int num = 20;
	double size = 0.2f;

	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j++) {
			pVolume = new OGLVolume(this, size);
			pVolume->SetRandomColor();
			pVolume->translate(static_cast<point_precision>(i * (size * 2) - (num * size)), 
				static_cast<point_precision>(0.0f),
				static_cast<point_precision>(j * (size * 2) - (num * size)));
			pVolume->UpdateOGLBuffers();
			pTimeObj->RegisterSubscriber(TIME_ELAPSED, pVolume);
			pSceneGraph->PushObject(pVolume);
		}
	}

	return r;
}

RESULT OpenGLImp::Render(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	DimObj *pDimObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Camera Projection Matrix
	SetCameraMatrix(EYE_MONO);

	// Send SceneGraph objects to shader
	pSceneGraph->Reset();
	while((pDimObj = pObjectStore->GetNextObject()) != NULL) {
		SendObjectToShader(pDimObj);
	}
	
	glFlush();

	CheckGLError();
	return r;
}

RESULT OpenGLImp::RenderStereo(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();
	DimObj *pDimObj = NULL;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < 2; i++) {

		EYE_TYPE eye = (i == 0) ? EYE_LEFT : EYE_RIGHT;

		SetStereoViewTarget(eye);
		SetCameraMatrix(eye);

		// Send SceneGraph objects to shader
		pSceneGraph->Reset();
		while ((pDimObj = pObjectStore->GetNextObject()) != NULL) {
			SendObjectToShader(pDimObj);
		}
	}
	
	glFlush();

	return r;
}


RESULT OpenGLImp::ShutdownImplementaiton() {
	RESULT r = R_PASS;

	//CBM((wglDeleteContext(m_hglrc)), "Failed to wglDeleteContext(hglrc)");

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

	if (m_pOpenGLRenderingContext != NULL) {
		delete m_pOpenGLRenderingContext;
		m_pOpenGLRenderingContext = NULL;
	}

	return r;
}

// Open GL / Wrappers

// OpenGL Program

RESULT OpenGLImp::glGetProgramInterfaceiv(GLuint program, GLenum programInterface, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramInterfaceiv(program, programInterface, pname, params);  
	CRM(CheckGLError(), "glGetProgramInterfaceiv failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramResourceiv(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceiv(program, programInterface, index, propCount, props, bufSize, length, params);
	CRM(CheckGLError(), "glGetProgramResourceiv failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetProgramResourceName(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetProgramResourceName(program, programInterface, index, bufSize, length, name);
	CRM(CheckGLError(), "glGetProgramResourceName failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenVertexArrays(GLsizei n, GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenVertexArrays(n, arrays);  //create VAO container and get ID for it
	CRM(CheckGLError(), "glGenVertexArrays failed");

Error:
	return r;
}

// Bind Array to OpenGL context
RESULT OpenGLImp::glBindVertexArray(GLuint gluiArray) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindVertexArray(gluiArray);
	CRM(CheckGLError(), "glBindVertexArray failed");

Error:
	return r;
}

RESULT OpenGLImp::glGenBuffers(GLsizei n, GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGenBuffers(n, buffers);
	CRM(CheckGLError(), "glGenBuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindBuffer(GLenum target, GLuint gluiBuffer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindBuffer(target, gluiBuffer);
	CRM(CheckGLError(), "glBindBuffer failed");

Error:
	return r;
}

RESULT OpenGLImp::glDeleteBuffers(GLsizei n, const GLuint *buffers) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteBuffers(n, buffers);
	CRM(CheckGLError(), "glDeleteBuffers failed");

Error:
	return r;
}

RESULT OpenGLImp::glDeleteVertexArrays(GLsizei n, const GLuint *arrays) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glDeleteVertexArrays(n, arrays);
	CRM(CheckGLError(), "glDeleteVertexArrays failed");

Error:
	return r;
}

RESULT OpenGLImp::glBindAttribLocation(GLuint program, GLuint index, const GLchar *name) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBindAttribLocation(program, index, name);
	CRM(CheckGLError(), "glBindAttribLocation failed");

Error:
	return r;
}

RESULT OpenGLImp::glGetUniformLocation(GLuint program, const GLchar *name, GLint *pLocation) {
	RESULT r = R_PASS;

	*pLocation = m_OpenGLExtensions.glGetUniformLocation(program, name);
	CRM(CheckGLError(), "glGetUniformLocation failed");

	return r;
Error:
	*pLocation = -1;
	return r;
}

RESULT OpenGLImp::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glUniformMatrix4fv(location, count, transpose, value);
	CRM(CheckGLError(), "glUniformMatrix4fv failed");

Error:
	return r;
}

// OpenGL Shaders
RESULT OpenGLImp::CreateShader(GLenum type, GLuint *shaderID) {
	RESULT r = R_PASS;

	*shaderID = m_OpenGLExtensions.glCreateShader(type);
	CRM(CheckGLError(), "glCreateShader failed");

	return r;
Error:
	*shaderID = NULL;
	return r;
}

RESULT OpenGLImp::GetShaderiv(GLuint programID, GLenum pname, GLint *params) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderiv(programID, pname, params);
	CRM(CheckGLError(), "glGetShaderiv failed");

Error:
	return r;
}

RESULT OpenGLImp::GetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glGetShaderInfoLog(shader, bufSize, length, infoLog);
	CRM(CheckGLError(), "glGetShaderInfoLog failed");

Error:
	return r;
}

RESULT OpenGLImp::ShaderSource(GLuint shaderID, GLsizei count, const GLchar *const*string, const GLint *length) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glShaderSource(shaderID, count, string, length);
	CRM(CheckGLError(), "glShaderSource failed");

Error:
	return r;
}

RESULT OpenGLImp::CompileShader(GLuint shaderID) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glCompileShader(shaderID);
	CRM(CheckGLError(), "glCompileShader failed");

Error:
	return r;
}

RESULT OpenGLImp::glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glBufferData(target, size, data, usage);
	CRM(CheckGLError(), "glBufferData failed");

Error:
	return r;
}

RESULT OpenGLImp::glEnableVertexAtrribArray(GLuint index) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glEnableVertexAttribArray(index);
	CRM(CheckGLError(), "glEnableVertexAttribArray failed");

Error:
	return r;
}

RESULT OpenGLImp::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
	RESULT r = R_PASS;

	m_OpenGLExtensions.glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	CRM(CheckGLError(), "glVertexAttribPointer failed");

Error:
	return r;
}