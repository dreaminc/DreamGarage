#include "OGLProgram.h"
#include "OpenGLImp.h"
#include "OGLObj.h"

OGLProgram::OGLProgram(OpenGLImp *pParentImp) :
	m_pParentImp(pParentImp),
	m_OGLProgramIndex(NULL),
	m_pVertexShader(nullptr),
	m_pFragmentShader(nullptr),
	m_versionOGL(0)
{
	// empty
}

OGLProgram::~OGLProgram() {
	ReleaseProgram();
}

RESULT OGLProgram::OGLInitialize() {
	RESULT r = R_PASS;

	CR(CreateProgram());
	CR(IsProgram());

Error:
	return r;
}

RESULT OGLProgram::OGLInitialize(const wchar_t *pszVertexShaderFilename, const wchar_t *pszFragmentShaderFilename, version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders
	CRM(MakeVertexShader(pszVertexShaderFilename), "Failed to create vertex shader");
	CRM(MakeFragmentShader(pszFragmentShaderFilename), "Failed to create fragment shader");

	// Link the program
	CRM(LinkProgram(), "Failed to link program");

	// TODO: This could all be done in one call in the OGLShader honestly
	// Attributes
	// TODO: Tabulate attributes (get them from shader, not from class)
	WCR(m_pVertexShader->GetVertexAttributesFromShader());
	//WCR(m_pVertexShader->GetAttributeLocationsFromShader());
	WCR(m_pVertexShader->BindAttributes());
	//WCR(m_pVertexShader->EnableAttributes());

	//CR(PrintActiveAttributes());

	// TODO: Uniform Variables

	// Uniform Blocks
	WCR(m_pVertexShader->GetUniformLocationsFromShader());
	WCR(m_pVertexShader->BindUniformBlocks());
	WCR(m_pVertexShader->InitializeUniformBlocks());

	// Fragment shader
	//CR(m_pFragmentShader->GetAttributeLocationsFromShader());
	//CR(m_pFragmentShader->BindAttributes());

	WCR(m_pFragmentShader->GetUniformLocationsFromShader());
	WCR(m_pFragmentShader->BindUniformBlocks());
	WCR(m_pFragmentShader->InitializeUniformBlocks());

	CR(PrintActiveUniformVariables());

	// TODO:  Currently using a global material 
	m_pFragmentShader->SetMaterial(&material(160.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));
	m_pFragmentShader->UpdateUniformBlockBuffers();

Error:
	return r;
}

// TODO: Just a pass through, but might make sense to absorb the functionality here
// or stuff it into the OGLShader
RESULT OGLProgram::CreateShader(GLenum type, GLuint *pShaderID) {
	RESULT r = R_PASS;

	CR(m_pParentImp->CreateShader(type, pShaderID));

Error:
	return r;
}

RESULT OGLProgram::IsProgram() {
	return m_pParentImp->IsProgram(m_OGLProgramIndex);
}

RESULT OGLProgram::CreateProgram() {
	RESULT r = R_PASS;

	CBM((m_OGLProgramIndex == NULL), "Cannot CreateGLProgram if program id not null");

	CR(m_pParentImp->CreateProgram(&m_OGLProgramIndex));
	CBM((m_OGLProgramIndex != 0), "Failed to create OGLProgram");

	DEBUG_LINEOUT("CREATED program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::ReleaseProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->DeleteProgram(m_OGLProgramIndex));

	DEBUG_LINEOUT("DELETED program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::LinkProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->LinkProgram(m_OGLProgramIndex));

	GLint param = GL_FALSE;
	CR(m_pParentImp->glGetProgramiv(m_OGLProgramIndex, GL_LINK_STATUS, &param));
	CBM((param == GL_TRUE), "Failed to link GL Program: %s", GetProgramInfoLog());

	DEBUG_LINEOUT("Successfully linked program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::UseProgram() {
	RESULT r = R_PASS;

	CR(m_pParentImp->UseProgram(m_OGLProgramIndex));

	DEBUG_LINEOUT("USING program ID %d", m_OGLProgramIndex);

Error:
	return r;
}

RESULT OGLProgram::BindUniformBlock(GLint uniformBlockIndex, GLint uniformBlockBindingPoint) {
	RESULT r = R_PASS;
	GLenum glerr;
	DWORD werr;

	CR(m_pParentImp->glUniformBlockBinding(m_OGLProgramIndex, uniformBlockIndex, uniformBlockBindingPoint));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound uniform block index %d to binding point %d err:0x%x", uniformBlockIndex, uniformBlockBindingPoint, werr);

Error:
	return r;
}

RESULT OGLProgram::PrintActiveUniformVariables() {
	RESULT r = R_PASS;

	GLint variables_n = 0;
	CR(m_pParentImp->glGetProgramInterfaceiv(m_OGLProgramIndex, GL_UNIFORM, GL_ACTIVE_RESOURCES, &variables_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION, GL_BLOCK_INDEX };

	DEBUG_LINEOUT("%d active uniform variables", variables_n);
	for (int i = 0; i < variables_n; i++) {
		GLint results[4];
		CR(m_pParentImp->glGetProgramResourceiv(m_OGLProgramIndex, GL_UNIFORM, i, 4, properties, 4, NULL, results));

		// Skip uniforms in blocks
		if (results[3] != -1) continue;

		GLint pszName_n = results[0] + 1;
		char *pszName = new char[pszName_n];
		CR(m_pParentImp->glGetProgramResourceName(m_OGLProgramIndex, GL_UNIFORM, i, pszName_n, NULL, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", results[2], pszName, OpenGLUtility::GetOGLTypeString(results[1]));

		if (pszName != NULL) {
			delete[] pszName;
			pszName = NULL;
		}
	}

Error:
	return r;
}

RESULT OGLProgram::BindAttribLocation(GLint index, const char* pszName) {
	RESULT r = R_PASS;
	DWORD werr;

	CR(m_pParentImp->glBindAttribLocation(m_OGLProgramIndex, index, pszName));

	werr = GetLastError();
	DEBUG_LINEOUT("Bound attribute %s to index location %d err:0x%x", pszName, index, werr);

Error:
	return r;
}

char* OGLProgram::GetProgramInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = NULL;
	int pszInfoLog_n = 4096;
	int charsWritten_n = -1;

	CR(m_pParentImp->glGetProgramiv(m_OGLProgramIndex, GL_INFO_LOG_LENGTH, &pszInfoLog_n));
	CBM((pszInfoLog_n > 0), "Program Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	memset(pszInfoLog, 0, sizeof(char) * pszInfoLog_n);
	CR(m_pParentImp->glGetProgramInfoLog(m_OGLProgramIndex, pszInfoLog_n, &charsWritten_n, pszInfoLog));

Error:
	return pszInfoLog;
}

/*
RESULT OGLProgram::MakeShader(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;



Error:
	return r;
}
*/

// TODO: This is repeated functionality - should create MakeShader function 
// that either takes a shader type or uses the extension

RESULT OGLProgram::MakeVertexShader(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	OGLVertexShader *pVertexShader = new OGLVertexShader(this);
	CN(pVertexShader);
	CRM(m_pParentImp->CheckGLError(), "Create OpenGL Vertex Shader failed");

	CRM(pVertexShader->InitializeFromFile(pszFilename, m_versionOGL), "Failed to initialize vertex shader from file");

	CRM(AttachShader(pVertexShader), "Failed to attach vertex shader");

Error:
	return r;
}

RESULT OGLProgram::MakeFragmentShader(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	OGLFragmentShader *pFragmentShader = new OGLFragmentShader(this);
	CN(pFragmentShader);
	CRM(m_pParentImp->CheckGLError(), "Create OpenGL Fragment Shader failed");

	CRM(pFragmentShader->InitializeFromFile(pszFilename, m_versionOGL), "Failed to initialize fragment shader from file");

	CRM(AttachShader(pFragmentShader), "Failed to attach fragment shader");

Error:
	return r;
}

RESULT OGLProgram::RenderObject(DimObj *pDimObj) {
	OGLObj *pOGLObj = dynamic_cast<OGLObj*>(pDimObj);

	// This is done once on the CPU side rather than per-vertex (although this in theory could be better precision) 
	auto matModel = pDimObj->GetModelMatrix();
	m_pVertexShader->SetModelMatrixUniform(matModel);

	/* TODO: This should be replaced with a materials store or OGLMaterial that pre-allocates and swaps binding points (Wait for textures)
	m_pFragmentShader->SetMaterial(pDimObj->GetMaterial());
	m_pFragmentShader->UpdateUniformBlockBuffers();
	//*/

	m_pFragmentShader->SetObjectTextures(pOGLObj);

	return pOGLObj->Render();
}

RESULT OGLProgram::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	CR(m_pVertexShader->SetLights(pLights));
	CR(m_pVertexShader->UpdateUniformBlockBuffers());

Error:
	return r;
}

// TODO: Consolidate?
RESULT OGLProgram::SetStereoCamera(stereocamera *pStereoCamera, EYE_TYPE eye) {
	RESULT r = R_PASS;

	auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix();
	auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
	auto matViewOrientation = pStereoCamera->GetOrientationMatrix();

	WCR(m_pVertexShader->SetViewMatrixUniform(matV));
	WCR(m_pVertexShader->SetProjectionMatrixUniform(matP));
	WCR(m_pVertexShader->SetViewOrientationMatrixUniform(matViewOrientation));
	WCR(m_pVertexShader->SetViewProjectionMatrixUniform(matVP));
	WCR(m_pVertexShader->SetEyePositionUniform(ptEye));

Error:
	return r;
}

RESULT OGLProgram::SetCamera(camera *pCamera) {
	RESULT r = R_PASS;

	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	auto matViewOrientation = pCamera->GetOrientationMatrix();

	WCR(m_pVertexShader->SetViewMatrixUniform(matV));
	WCR(m_pVertexShader->SetProjectionMatrixUniform(matP));
	WCR(m_pVertexShader->SetViewOrientationMatrixUniform(matViewOrientation));
	WCR(m_pVertexShader->SetViewProjectionMatrixUniform(matVP));
	WCR(m_pVertexShader->SetEyePositionUniform(ptEye));

	// TODO: Better handle this
	r = R_PASS;

Error:
	return r;
}

RESULT OGLProgram::AttachShader(OpenGLShader *pOpenGLShader) {
	RESULT r = R_PASS;
	GLenum glerr = GL_NO_ERROR;

	switch (pOpenGLShader->GetShaderType()) {
		case GL_VERTEX_SHADER: {
			this->m_pVertexShader = dynamic_cast<OGLVertexShader*>(pOpenGLShader);
			CN(this->m_pVertexShader);
		} break;

		case GL_FRAGMENT_SHADER: {
			this->m_pFragmentShader = dynamic_cast<OGLFragmentShader*>(pOpenGLShader);
			CN(this->m_pFragmentShader);
		} break;

		default: {
			CBM((0), "Shader type 0x%x cannot be attached", pOpenGLShader->GetShaderType());
		}
	}

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//numShaders = param;

	CRM(m_pParentImp->glAttachShader(m_OGLProgramIndex, pOpenGLShader->GetShaderID()), "AttachShader failed with GL log:%s", pOpenGLShader->GetInfoLog());

	//m_glGetProgramiv(m_idOpenGLProgram, GL_ATTACHED_SHADERS, &param);
	//CBM((param = numShaders + 1), "Failed to attach shader, num shaders attached %d", param);

	// Assign the shader to the implementation stage
	DEBUG_LINEOUT("Attached shader %d type 0x%x", pOpenGLShader->GetShaderID(), pOpenGLShader->GetShaderType());

Error:
	return r;
}

/*
// TODO: Re-design using attribute registration approach
RESULT OGLProgram::EnableVertexPositionAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexPositionAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OGLProgram::EnableVertexColorAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexColorAttribute();
	else
		return R_FAIL;

	return R_PASS;
}

RESULT OGLProgram::EnableVertexNormalAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableVertexNormalAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexUVCoordAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableUVCoordAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexTangentAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableTangentAttribute();
	else
		return R_FAIL;
}

RESULT OGLProgram::EnableVertexBitangentAttribute() {
	if (m_pVertexShader != NULL)
		m_pVertexShader->EnableBitangentAttribute();
	else
		return R_FAIL;
}
*/