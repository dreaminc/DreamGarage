#ifndef SHADER_H_
#define SHADER_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/Shader.h

// The Shader super class is a pure virtual class to capture the general
// interface for a shader class

#include "core/types/DObject.h"

typedef enum ShaderType {
	SHADER_VERTEX,
	SHADER_TESSELATION,
	SHADER_TESSELATION_EVALUATION,
	SHADER_GEOMETRY,
	SHADER_FRAGMENT,
	SHADER_CUSTOM,
	SHADER_INVALID
} SHADER_TYPE;

class Shader : public DObject {
public:
	//virtual RESULT Enable() = 0;
	//virtual RESULT Disable() = 0;

	virtual RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename) = 0;
	virtual RESULT LoadShaderCodeFromFile(const wchar_t *pszFilename, version versionFile) = 0;

	virtual RESULT Compile(void) = 0;

protected:
	SHADER_TYPE m_shaderType;
};

#endif // ! SHADER_H_
