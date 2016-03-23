#include "OpenGLShader.h"
#include "OpenGLImp.h"

#include "Sandbox/SandboxApp.h"
#include "Sandbox/PathManager.h"

OpenGLShader::OpenGLShader(OpenGLImp *pParentImp, GLenum shaderType) :
	m_pParentImp(pParentImp),
	m_shaderType(shaderType),
	m_pszShaderCode(NULL),
	m_shaderID(NULL)
{
	RESULT r = R_PASS;

	CR(m_pParentImp->CreateShader(m_shaderType, &m_shaderID));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

OpenGLShader::~OpenGLShader(void) {
	if (m_pszShaderCode != NULL) {
		delete [] m_pszShaderCode;
		m_pszShaderCode = NULL;
	}
}

GLuint OpenGLShader::GetShaderID() {
	return m_shaderID; 
}

GLenum OpenGLShader::GetShaderType() {
	return m_shaderType; 
}
const char *OpenGLShader::GetShaderCode() { 
	return m_pszShaderCode; 
}

RESULT OpenGLShader::LoadShaderCodeFromFile(const wchar_t *pszFilename) {
	version tempVersion(0);
	return LoadShaderCodeFromFile(pszFilename, tempVersion);
}

RESULT OpenGLShader::LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	wchar_t *pFilePath = NULL;

	if (versionFile == 0) {
		CRM(pPathManager->GetFilePath(PATH_SHADERS, pszFilename, pFilePath), "Failed to get path for %S shader", pszFilename);
	}
	else {
		version versionFileExists = 0;
		CRM(pPathManager->GetFileVersionThatExists(PATH_SHADERS, versionFile, pszFilename, &versionFileExists), "Failed to get existing file version");

		CRM(pPathManager->GetFilePathVersion(PATH_SHADERS, versionFileExists, pszFilename, pFilePath),
			"Failed to get path for %S shader version %d.%d", pszFilename, versionFile.major(), versionFile.minor());
	}

	m_pszShaderCode = FileRead(pFilePath);
	CNM(m_pszShaderCode, "Failed to read file %S", pFilePath);

	DEBUG_LINEOUT("Loaded new shader %S", pFilePath);

Error:
	if (pFilePath != NULL) {
		delete[] pFilePath;
		pFilePath = NULL;
	}

	return r;
}

RESULT OpenGLShader::InitializeFromFile(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;

	CRM(LoadShaderCodeFromFile(pszFilename, versionFile), "Failed to load vertex shader code from %S", pszFilename);
	CRM(Compile(), "Failed to compile shader");
	CRM(AttachShader(), "Failed to attach vertex shader");

	// Initialize all of the IDs
	// TODO: This can't be done until after linking
	//CRM(GetAttributeLocationsFromShader(), "Failed to get attribute locations");
	//CRM(GetUniformLocationsFromShader(), "Failed to get uniform locations");

Error:
	return r;
}

// Copy over the code - assuming const
RESULT OpenGLShader::LoadShaderCodeFromString(const char* pszSource) {
	RESULT r = R_PASS;

	int length = strlen(pszSource);
	m_pszShaderCode = (char*)(new char(length * sizeof(char)));
	CNM(m_pszShaderCode, "Failed to allocated %d bytes for shader code", length);

	CBM((strcpy_s(m_pszShaderCode, (length * sizeof(char)), pszSource) == 0), "Failed to copy over code string");

	return r;
Error:
	if (m_pszShaderCode != NULL) {
		delete m_pszShaderCode;
		m_pszShaderCode = NULL;
	}
	return r;
}

RESULT OpenGLShader::Compile(void) {
	RESULT r = R_PASS;

	CNM(m_pszShaderCode, "Cannot compile NULL code");

	const char *pszShaderCode = m_pszShaderCode;

	CR(m_pParentImp->ShaderSource(m_shaderID, 1, &pszShaderCode, NULL));
	CR(m_pParentImp->CompileShader(m_shaderID));

	int param;
	CR(m_pParentImp->GetShaderiv(m_shaderID, GL_COMPILE_STATUS, &param));
	CBM((param == GL_TRUE), "Shader Compile Error: %s", GetInfoLog());

	DEBUG_LINEOUT("Compiled shader type %d with Shader ID %d", m_shaderType, m_shaderID);

Error:
	return r;
}

// TODO: Is this even needed or can it be placed into the init function
RESULT OpenGLShader::AttachShader() {
	RESULT r = R_PASS;

	CRM(m_pParentImp->AttachShader(this), "Failed to attach to parent implementation");

Error:
	return r;
}

RESULT OpenGLShader::PrintInfoLog() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT(GetInfoLog());

Error:
	return r;
}

char* OpenGLShader::GetInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = NULL;
	int pszInfoLog_n = -1;
	int charsWritten_n = -1;

	CR(m_pParentImp->GetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &pszInfoLog_n));

	CBM((pszInfoLog_n > 0), "Shader Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	CR(m_pParentImp->GetShaderInfoLog(m_shaderID, pszInfoLog_n, &charsWritten_n, pszInfoLog));

Error:
	return pszInfoLog;
}

// TODO: Fix arch, this call returns new memory
char* OpenGLShader::FileRead(wchar_t *pszFileName) {
	RESULT r = R_PASS;
	errno_t err;
	char *pszFileContent = NULL;
	FILE *pFile = NULL;

	CNM(pszFileName, "Filename cannot be NULL");
	
	err = _wfopen_s(&pFile, pszFileName, L"r");
	CNM(pFile, "Failed to open %s", pszFileName);

	int pFile_n = -1;
	err = fseek(pFile, 0, SEEK_END);
	pFile_n = ftell(pFile);
	rewind(pFile);		

	CBM((pFile_n > 0), "File %S is empty", pszFileName);

	pszFileContent = new char[pFile_n + 1];
	pFile_n = fread(pszFileContent, sizeof(char), pFile_n, pFile);
	pszFileContent[pFile_n] = '\0';

Error:
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}

	return pszFileContent;
}