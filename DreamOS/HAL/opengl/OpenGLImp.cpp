#include "OpenGLImp.h"
#include "OGLObj.h"

OpenGLImp::OpenGLImp(OpenGLRenderingContext *pOpenGLRenderingContext) :
	m_idOpenGLProgram(NULL),
	m_versionMinor(0),
	m_versionMajor(0),
	m_versionGLSL(0),
	m_pVertexShader(NULL),
	m_pFragmentShader(NULL),
	m_pOpenGLRenderingContext(pOpenGLRenderingContext),
	m_pCamera(NULL)
{
	ACRM(InitializeGLContext(), "Failed to Initialize OpenGL Context");
	ACRM(PrepareScene(), "Failed to prepare GL Scene");
}

OpenGLImp::~OpenGLImp() {
	m_OpenGLExtensions.glDeleteProgram(m_idOpenGLProgram);
}

RESULT OpenGLImp::InitializeOpenGLVersion() {
	// For all versions
	char* pszVersion = (char*)glGetString(GL_VERSION); // Ver = "3.2.0"
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
	CBM((m_versionMajor >= 3 || (m_versionMajor == 3 && m_versionMinor >= 2)), "OpenGL 3.2 + Not Supported");

	// Should be called after context is created and made current
	ACRM(m_OpenGLExtensions.InitializeExtensions(), "Failed to initialize extensions");
	
	// Lets create the 3.2+ context
	CRM(m_pOpenGLRenderingContext->InitializeRenderingContext(m_versionMajor, m_versionMinor), "Failed to initialize oglrc");
	
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
}

RESULT OpenGLImp::EnableVertexColorAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexColorAttribute();
	else
		return R_FAIL;
}

RESULT OpenGLImp::BindAttribLocation(unsigned int index, char* pszName) {
	RESULT r = R_PASS;
	GLenum glerr;
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

// TODO: Get this outta here
#include "Primitives/ProjectionMatrix.h"

// TODO: Get this outta here
#include "Primitives/TranslationMatrix.h"

// TODO: Get this outta here
#include "Primitives/RotationMatrix.h"

RESULT OpenGLImp::PrepareScene() {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());

	// Clear Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	CRM(CreateGLProgram(), "Failed to create GL program");
	CRM(CheckGLError(), "CreateGLProgram failed");

	// TODO: Should be stuffed into factory arch - return NULL on fail
	// TODO: More complex shader handling - right now statically calling minimal shader
	// TODO: Likely put into factory
	OGLVertexShader *pVertexShader = new OGLVertexShader(this);
	CRM(CheckGLError(), "Create OpenGL Vertex Shader failed");
	CRM(pVertexShader->InitializeFromFile(L"minimal.vert"), "Failed to initialize vertex shader from file");
	CR(pVertexShader->BindAttributes());

	OpenGLShader *pFragmentShader = new OGLFragmentShader(this);
	CRM(CheckGLError(), "Create OpenGL Fragment Shader failed");
	CRM(pFragmentShader->InitializeFromFile(L"minimal.frag"), "Failed to initialize fragment shader from file");
	
	// Link OpenGL Program
	// TODO: Fix the error handling here (driver issue?)
	CRM(LinkProgram(), "Failed to link program");
	CRM(UseProgram(), "Failed to use open gl program");

	CR(PrintVertexAttributes());
	CR(PrintActiveUniformVariables());

	// Allocate the camera
	m_pCamera = new camera(point(0.0f, 0.0f, -10.0f), 45.0f, m_pxViewWidth, m_pxViewHeight);

Error:
	CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

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
	CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

	return r;
}

RESULT OpenGLImp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

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

Error:
	return r;
}

RESULT OpenGLImp::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	SenseMouse::PrintEvent(mEvent);

	float MouseMoveFactor = 0.1f;

	switch (mEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			CR(m_pCamera->RotateCameraByDiffXY(mEvent->dx, mEvent->dy));
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

RESULT OpenGLImp::Render(SceneGraph *pSceneGraph) {
	RESULT r = R_PASS;
	OGLObj *pOGLObj = NULL;
	DimObj *pDimObj = NULL;
	SceneGraphStore *pObjectStore = pSceneGraph->GetSceneGraphStore();

	// TODO: Remove
	static float theta = 0.0;
	theta -= 0.01f;

	auto matModel = RotationMatrix(RotationMatrix::Z_AXIS, theta) * RotationMatrix(RotationMatrix::Y_AXIS, theta) * RotationMatrix(RotationMatrix::X_AXIS, -theta);
	//auto matModel = ;
	//RotationMatrix matModel(0.0f, theta, -theta);

	TranslationMatrix matView(0.0f, 0.0f, theta);
	ProjectionMatrix matProjection(PROJECTION_MATRIX_PERSPECTIVE, m_pxViewWidth, m_pxViewHeight, 1.0f, 100.0f, 45.0f);

	// TODO: fix camera thing !!
	//m_pCamera->UpdateFromKeyboardState((SenseKeyboard*)(m_pWindows64App->m_pWin64Keyboard));
	m_pCamera->UpdatePosition(); 

	//auto matMVP = matProjection * matView * matModel;
	auto matMVP = m_pCamera->GetProjectionMatrix() * m_pCamera->GetViewMatrix() * matModel;
	//auto matMVP = m_pCamera->GetProjectionViewMatrix() * matModel;

	GLint locationProjectionMatrix = -1, locationViewMatrix = -1, locationModelMatrix = -1, locationModelViewProjectionMatrix = -1;

	CR(m_pOpenGLRenderingContext->MakeCurrentContext());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// First test the identity 
	//matModel.identity();
	//matView.identity();
	//matProjection.identity();

	// This is for testing only
	// TODO: Combined MVP or do in the shader?
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4Projection", &locationProjectionMatrix);
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4View", &locationViewMatrix);
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4Model", &locationModelMatrix);
	glGetUniformLocation(m_idOpenGLProgram, "u_mat4ModelViewProjection", &locationModelViewProjectionMatrix);

	if (locationProjectionMatrix >= 0)
		glUniformMatrix4fv(locationProjectionMatrix, 1, GL_FALSE, (GLfloat*)(&m_pCamera->GetProjectionMatrix()));

	if (locationViewMatrix >= 0)
		glUniformMatrix4fv(locationViewMatrix, 1, GL_FALSE, (GLfloat*)(&m_pCamera->GetViewMatrix()));

	if (locationModelMatrix >= 0)
		glUniformMatrix4fv(locationModelMatrix, 1, GL_FALSE, (GLfloat*)(&matModel));

	if (locationModelViewProjectionMatrix >= 0)
		glUniformMatrix4fv(locationModelViewProjectionMatrix, 1, GL_FALSE, (GLfloat*)(&matMVP));


	// Process SceneGraph
	pSceneGraph->Reset();
	while((pDimObj = pObjectStore->GetNextObject()) != NULL) {
		pOGLObj = dynamic_cast<OGLObj*>(pDimObj);
		pOGLObj->Render();
	}

	
	glFlush();

Error:
	CR(m_pOpenGLRenderingContext->ReleaseCurrentContext());

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

Error:
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