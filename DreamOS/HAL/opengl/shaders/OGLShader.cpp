#include "OGLShader.h"
#include "../OpenGLImp.h"
#include "../OGLProgram.h"

#include "Sandbox/SandboxApp.h"
#include "Sandbox/PathManager.h"

OGLShader::OGLShader(OGLProgram *pParentProgram) :
	GLSLObject(pParentProgram),
	//m_pszShaderCode(nullptr),
	m_shaderID(NULL),
	m_shaderType(NULL)
{
	// empty
}

OGLShader::OGLShader(OGLProgram *pParentProgram, GLenum shaderType) :
	//m_pParentImp(pParentImp),
	GLSLObject(pParentProgram),
	m_shaderType(shaderType),
	//m_pszShaderCode(nullptr),
	m_shaderID(NULL)
{
	RESULT r = R_PASS;

	CR(m_pParentProgram->CreateShader(m_shaderType, &m_shaderID));

	Validate();
	return;

Error:
	Invalidate();
	return;
}

OGLShader::~OGLShader(void) {
	//if (m_pszShaderCode != nullptr) {
	//	delete [] m_pszShaderCode;
	//	m_pszShaderCode = nullptr;
	//}

	m_shaderCode.clear();
}

GLuint OGLShader::GetShaderID() {
	return m_shaderID; 
}

GLenum OGLShader::GetShaderType() {
	return m_shaderType; 
}
//const char *OpenGLShader::GetShaderCode() { 
//	return m_pszShaderCode; 
//}
std::vector<std::string> OGLShader::GetShaderCode() {
	return m_shaderCode;
}

RESULT OGLShader::ClearShaderCode() {
	m_shaderCode = std::vector<std::string>();
	return R_PASS;
}

RESULT OGLShader::AddShaderCode(const std::string &strShaderCode) {
	m_shaderCode.push_back(strShaderCode);
	return R_PASS;
}

RESULT OGLShader::AddShaderCode(const char *pszShaderCode) {
	RESULT r = R_PASS;
	
	CNM(pszShaderCode, "Cannot push nullptr shader code");

	CR(AddShaderCode(std::string(pszShaderCode)));
	
Error:
	return r;
}



RESULT OGLShader::LoadShaderCodeFromFile(const wchar_t *pszFilename) {
	RESULT r = R_PASS;

	version tempVersion(0);
	CRM(LoadShaderCodeFromFile(pszFilename, tempVersion), "Failed to load shader code from file");

Error:
	return r;
}

RESULT OGLShader::LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;

	PathManager *pPathManager = PathManager::instance();
	wchar_t *pFilePath = nullptr;

	if (versionFile == 0) {
		CRM(pPathManager->GetFilePath(PATH_SHADERS, pszFilename, pFilePath), "Failed to get path for %S shader", pszFilename);
	}
	else {
		version versionFileExists = 0;
		CRM(pPathManager->GetFileVersionThatExists(PATH_SHADERS, versionFile, pszFilename, &versionFileExists), "Failed to get existing file version");

		CRM(pPathManager->GetFilePathVersion(PATH_SHADERS, versionFileExists, pszFilename, pFilePath),
			"Failed to get path for %S shader version %d.%d", pszFilename, versionFile.major(), versionFile.minor());
	}

	const char *pszShaderCode = FileRead(pFilePath);
	CNM(pszShaderCode, "Failed to read file %S", pFilePath);
	CRM(AddShaderCode(pszShaderCode), "Failed to add shader code from file");

	DEBUG_LINEOUT("Loaded new shader %S", pFilePath);

Error:
	if (pFilePath != nullptr) {
		delete[] pFilePath;
		pFilePath = nullptr;
	}

	return r;
}

RESULT OGLShader::InitializeFromFile(const wchar_t *pszFilename, version versionFile) {
	RESULT r = R_PASS;

	auto sharedShaderFiles = m_pParentProgram->GetSharedShaderFilenames(m_shaderType);

	for (std::wstring strSharedShaderFile : sharedShaderFiles) {
		LoadShaderCodeFromFile(strSharedShaderFile.c_str(), versionFile);
	}

	CRM(LoadShaderCodeFromFile(pszFilename, versionFile), "Failed to load vertex shader code from %S", pszFilename);
	CRM(Compile(), "Failed to compile shader");

Error:
	return r;
}

// Copy over the code - assuming const
RESULT OGLShader::LoadShaderCodeFromString(const char* pszSource) {
	RESULT r = R_PASS;

	//size_t length = strlen(pszSource);
	//m_pszShaderCode = (char*)(new char[sizeof(char) * length]);
	//CNM(m_pszShaderCode, "Failed to allocated %zu bytes for shader code", length);
	//CBM((strcpy_s(m_pszShaderCode, (length * sizeof(char)), pszSource) == 0), "Failed to copy over code string");

	CRM(AddShaderCode(pszSource), "Failed to add shader code from string");

	//return r;
Error:
	//if (m_pszShaderCode != nullptr) {
	//	delete m_pszShaderCode;
	//	m_pszShaderCode = nullptr;
	//}

	return r;
}

RESULT OGLShader::Compile(void) {
	RESULT r = R_PASS;

	int i = 0;
	char **ppszShaderCode = nullptr;
	size_t shaderCount = m_shaderCode.size();

	//CNM(m_pszShaderCode, "Cannot compile NULL code");
	CBM((shaderCount > 0), "Cannot compile empty shader");

	//const char *pszShaderCode = m_shaderCode[0].c_str();
	
	ppszShaderCode = new char*[shaderCount];
	for (std::string strShaderCode : m_shaderCode) {
		size_t strLen = strShaderCode.size() + 1;
		ppszShaderCode[i] = new char[strLen];
		memset(ppszShaderCode[i], 0, strLen);
		memcpy(ppszShaderCode[i], strShaderCode.c_str(), strShaderCode.size());
		i++;
	}

	OpenGLImp *pParentImp = GetParentOGLImplementation();
	CN(pParentImp);

	CR(pParentImp->ShaderSource(m_shaderID, (GLsizei)(shaderCount), ppszShaderCode, nullptr));
	CR(pParentImp->CompileShader(m_shaderID));

	int param;
	CR(pParentImp->GetShaderiv(m_shaderID, GL_COMPILE_STATUS, &param));
	CBM((param == GL_TRUE), "Shader Compile Error: %s", GetInfoLog());

	DEBUG_LINEOUT("Compiled shader type %d with Shader ID %d", m_shaderType, m_shaderID);

Error:
	if (ppszShaderCode != nullptr) {
		
		for (int i = 0; i < shaderCount; i++) {
			if (ppszShaderCode[i] != nullptr) {
				delete [] ppszShaderCode[i];
				ppszShaderCode[i] = nullptr;
			}
		}

		delete[] ppszShaderCode;
		ppszShaderCode = nullptr;
	}

	return r;
}

RESULT OGLShader::PrintInfoLog() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT(GetInfoLog());

	return r;
}

char* OGLShader::GetInfoLog() {
	RESULT r = R_PASS;

	char *pszInfoLog = nullptr;
	int pszInfoLog_n = -1;
	int charsWritten_n = -1;

	OpenGLImp *pParentImp = GetParentOGLImplementation();

	CR(pParentImp->GetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &pszInfoLog_n));

	CBM((pszInfoLog_n > 0), "Shader Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	CR(pParentImp->GetShaderInfoLog(m_shaderID, pszInfoLog_n, &charsWritten_n, pszInfoLog));

Error:
	return pszInfoLog;
}

// TODO: Fix arch, this call returns new memory
char* OGLShader::FileRead(wchar_t *pszFileName) {
	RESULT r = R_PASS;
	errno_t err;
	char *pszFileContent = nullptr;
	FILE *pFile = nullptr;

	CNM(pszFileName, "Filename cannot be NULL");
	
	err = _wfopen_s(&pFile, pszFileName, L"r");
	
	// TODO: print out with unicode support.
	CNM(pFile, "Failed to open file");

	size_t pFile_n = -1;
	err = fseek(pFile, 0, SEEK_END);
	pFile_n = ftell(pFile);
	rewind(pFile);		

	CBM((pFile_n > 0), "File %S is empty", pszFileName);

	pszFileContent = new char[pFile_n + 1];
	pFile_n = fread(pszFileContent, sizeof(char), pFile_n, pFile);
	pszFileContent[pFile_n] = '\0';

Error:
	if (pFile != nullptr) {
		fclose(pFile);
		pFile = nullptr;
	}

	return pszFileContent;
}