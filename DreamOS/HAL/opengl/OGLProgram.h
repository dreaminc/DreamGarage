#ifndef OGL_PROGRAM_H_
#define OGL_PROGRAM_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/opengl/OGLProgram.h
// OpenGL Program class

#include "OpenGLCommon.h"

#include "Primitives/DimObj.h"
#include <vector>

class OpenGLImp;
class OGLVertexAttribute;

class OGLProgram {
public:
	OGLProgram(OpenGLImp *pParentImp);
	~OGLProgram();

	RESULT OGLInitialize();
	RESULT OGLInitialize(const wchar_t *pszVertexShaderFilename, const wchar_t *pszFragmentShaderFilename, version versionFile);
	
	RESULT CreateProgram();
	RESULT ReleaseProgram();
	RESULT LinkProgram();
	RESULT UseProgram();
	RESULT IsProgram();

	// TODO: Is this really vertex only?
	RESULT PrintActiveAttributes();
	RESULT PrintActiveUniformVariables();
	char* GetProgramInfoLog();

	//RESULT MakeShader(const wchar_t *pszFilename, version versionFile);
	RESULT MakeVertexShader(const wchar_t *pszFilename);
	RESULT MakeFragmentShader(const wchar_t *pszFilename);

	inline RESULT RenderObject(DimObj *pDimObj);
	RESULT SetLights(std::vector<light*> *pLights);
	
	RESULT SetCamera(camera *pCamera);
	RESULT SetStereoCamera(stereocamera *pStereoCamera, EYE_TYPE eye);
	
	// Shaders
	RESULT CreateShader(GLenum type, GLuint *pShaderID);

	GLuint GetOGLProgramIndex() { 
		return m_OGLProgramIndex;
	}

	OpenGLImp *GetOGLImp() {
		return m_pParentImp;
	}

public:
	// TODO: [SHADER] This should be baked into Shader
	// TODO: Build Shader Attribute Registration
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
	RESULT EnableVertexUVCoordAttribute();
	RESULT EnableVertexTangentAttribute();
	RESULT EnableVertexBitangentAttribute();

	RESULT AttachShader(OpenGLShader *pOpenGLShader);

private:
	OpenGLImp *m_pParentImp;

	version m_versionOGL;

	GLuint m_OGLProgramIndex;

	// Shaders
	OGLVertexShader *m_pVertexShader;
	OGLFragmentShader *m_pFragmentShader;
	// TODO: Other shaders

	// Vertex Attributes 
	std::vector<OGLVertexAttribute*> m_OGLVertexAttributes;

	// TODO: Uniforms

	// TODO: Pipelines
};

#endif // ! OGL_PROGRAM_H_