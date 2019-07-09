#ifndef SHADER_H_
#define SHADER_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"
#include "Primitives/version.h"

// DREAM OS
// DreamOS/HAL/Shader.h
// The Shader super class is a pure virtual class to capture the general
// interface for a shader class

typedef enum ShaderType {
	SHADER_VERTEX,
	SHADER_TESSELATION,
	SHADER_TESSELATION_EVALUATION,
	SHADER_GEOMETRY,
	SHADER_FRAGMENT,
	SHADER_CUSTOM,
	SHADER_INVALID
} SHADER_TYPE;

class Shader : public valid {
public:
	//virtual RESULT Enable() = 0;
	//virtual RESULT Disable() = 0;

	virtual RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename) = 0;
	virtual RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile) = 0;

	virtual RESULT Compile(void) = 0;

	/*
	virtual RESULT LoadFromString(const char* pszSource) = 0;
	virtual RESULT CreateAndLinkProgram() = 0;
	virtual RESULT AddAttribute(const char *pszAttribute) = 0;
	virtual RESULT AddUniform(const char *pszUniform) = 0;
	virtual RESULT DeleteShader() = 0;
	*/

protected:
	SHADER_TYPE m_shaderType;

private:
	UID m_uid;
};

#endif // ! SHADER_H_
