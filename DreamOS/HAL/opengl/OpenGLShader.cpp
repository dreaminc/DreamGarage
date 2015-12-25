#include "OpenGLShader.h"
#include "OpenGLImp.h"

OpenGLShader::OpenGLShader(OpenGLImp *pParentImp, GLenum shaderType) :
	m_pParentImp(pParentImp),
	m_shaderType(shaderType),
	m_pszShaderCode(NULL)
{
	m_shaderID = m_pParentImp->glCreateShader(m_shaderType);
}

OpenGLShader::~OpenGLShader(void) {
	if (m_pszShaderCode != NULL) {
		delete [] m_pszShaderCode;
		m_pszShaderCode = NULL;
	}
}

RESULT OpenGLShader::LoadFromFile(char *pszFilename) {
	RESULT r = R_PASS;
	
	m_pszShaderCode = FileRead(pszFilename);
	CNM(m_pszShaderCode, "Failed to read file %s", pszFilename);

Error:
	return r;
}

RESULT OpenGLShader::Compile(void) {
	RESULT r = R_PASS;

	CNM(m_pszShaderCode, "Cannot compile NULL code");

	const char *pszShaderCode = m_pszShaderCode;

	m_pParentImp->glShaderSource(m_shaderID, 1, &pszShaderCode, NULL);
	m_pParentImp->glCompileShader(m_shaderID);

	int param;
	m_pParentImp->glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &param);

	CBM((param != GL_TRUE), "Shader Compile Error: %s", GetInfoLog());

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

	m_pParentImp->glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &pszInfoLog_n);

	CBM((pszInfoLog_n > 0), "Shader Info Log of zero length");

	pszInfoLog = new char[pszInfoLog_n];
	m_pParentImp->glGetShaderInfoLog(m_shaderID, pszInfoLog_n, &charsWritten_n, pszInfoLog);

Error:
	return pszInfoLog;
}

char* OpenGLShader::FileRead(char *pszFileName) {
	RESULT r = R_PASS;

	char *pszFileContent = NULL;
	FILE *pFile = NULL;

	CNM(pszFileName, "Filename cannot be NULL");
	
	errno_t err = fopen_s(&pFile, pszFileName, "r");
	CNM(pFile, "Failed to open %s", pszFileName);

	int pFile_n = -1;
	fseek(pFile, 0, SEEK_END);
	rewind(pFile);		

	CBM((pFile_n > 0), "File %s is empty", pszFileName);

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