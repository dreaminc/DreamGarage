#ifndef OGL_VERTEX_SHADER_H_
#define OGL_VERTEX_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLVertexShader.h
// This is a OGL vertex shader object

#include "OpenGLShader.h"
#include "OGLLightsBlock.h"

class OpenGLImp;	// Declare OpenGLImp class

#define VERTEX_SHADER_POSITION_INDEX 0
#define VERTEX_SHADER_COLOR_INDEX 1
#define VERTEX_SHADER_NORMAL_INDEX 2

class OGLVertexShader : public OpenGLShader {
public:
	OGLVertexShader(OpenGLImp *pParentImp);
	
	// This is handled by the parent class 
	//~OGLVertexShader(void);

	RESULT GetAttributeLocationsFromShader();
	RESULT GetUniformLocationsFromShader();

public:
	// TODO: These could be generated, even within a macro
	const char *GetPositionAttributeName() { return "inV_vec4Position"; }
	const char *GetColorAttributeName() { return "inV_vec4Color"; }
	const char *GetNormalAttributeName() { return "inV_vec4Normal"; }

	const char *GetModelMatrixUniformName() { return "u_mat4Model";  }
	const char *GetViewProjectionMatrixUniformName() { return "u_mat4ViewProjection"; }

	const char *GetLightsUniformBlockName() { return "ub_LightArray"; }

	GLint GetPositionIndex();
	GLint GetColorIndex();
	GLint GetNormalIndex();
	
	GLint GetModelMatrixUniformIndex();
	GLint GetViewProjectionMatrixUniformIndex();

	GLint GetLightsUniformBlockBufferIndex();
	GLint GetLightsUniformBlockIndex();
	GLint GetLightsUniformBlockBindingPoint();

	// TODO: Create OGLAttributes, OGLUniform, OGLUniformBlock objects instead
	RESULT BindAttributes();
	RESULT BindUniformBlocks();

	RESULT InitializeUniformBlocks();
	RESULT UpdateUniformBlockBuffers();

	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();

	RESULT SetLights(std::vector<light*> *pLights);

private:
	GLint m_PositionIndex;
	GLint m_ColorIndex;
	GLint m_NormalIndex;

	GLint m_uniformModelMatrixIndex;
	GLint m_uniformViewProjectionMatrixIndex;

	/* Move into OGLUniformBlock - OGLLightsBlock
	GLint m_uniformBlockLightsIndex;
	GLint m_uniformBlockLightsBindingPoint;
	*/

	OGLLightsBlock *m_pLightsBlock;
};

#endif // ! OGL_VERTEX_SHADER_H_
