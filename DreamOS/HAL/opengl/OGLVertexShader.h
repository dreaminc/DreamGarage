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
	const char *GetUVCoordAttributeName() { return "inV_vec2UVCoord"; }
	const char *GetTangentAttributeName() { return "inV_vec4Tangent"; }
	const char *GetBitangentAttributeName() { return "inV_vec4Bitangent"; }

	const char *GetEyePositionUniformName() { return "u_vec4Eye"; }
	const char *GetModelMatrixUniformName() { return "u_mat4Model";  }
	const char *GetViewMatrixUniformName() { return "u_mat4View"; }
	const char *GetModelViewMatrixUniformName() { return "u_mat4ModelView"; }
	const char *GetViewProjectionMatrixUniformName() { return "u_mat4ViewProjection"; }
	const char *GetNormalMatrixUniformName() { return "u_mat4Normal"; }

	const char *GetLightsUniformBlockName() { return "ub_LightArray"; }

	GLint GetPositionIndex();
	GLint GetColorIndex();
	GLint GetNormalIndex();
	GLint GetUVCoordIndex();
	GLint GetTangentIndex();
	GLint GetBitangentIndex();
	
	GLint GetEyePositionUniformIndex();
	GLint GetModelMatrixUniformIndex();
	GLint GetViewMatrixUniformIndex();
	GLint GetModelViewMatrixUniformIndex();
	GLint GetViewProjectionMatrixUniformIndex();
	GLint GetNormalMatrixUniformIndex();

	RESULT SetEyePositionUniform(matrix<float, 4, 1> ptEye);
	RESULT SetModelMatrixUniform(matrix<float, 4, 4> matModel);
	RESULT SetViewMatrixUniform(matrix<float, 4, 4> matView);
	RESULT SetModelViewMatrixUniform(matrix<float, 4, 4> matModelView);
	RESULT SetViewProjectionMatrixUniform(matrix<float, 4, 4> matViewProjection);
	RESULT SetNormalMatrixUniform(matrix<float, 4, 4> matNormal);

	GLint GetLightsUniformBlockBufferIndex();
	GLint GetLightsUniformBlockIndex();
	GLint GetLightsUniformBlockBindingPoint();

	// TODO: Create OGLAttributes, OGLUniform, OGLUniformBlock objects instead
	RESULT BindAttributes();
	RESULT BindUniformBlocks();

	RESULT InitializeUniformBlocks();
	RESULT UpdateUniformBlockBuffers();

	// TODO: [SHADER] This should be redesigned
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
	RESULT EnableUVCoordAttribute();
	RESULT EnableTangentAttribute();
	RESULT EnableBitangentAttribute();

	RESULT SetLights(std::vector<light*> *pLights);

private:
	// All of this is the same across shaders
	// TODO: Push this up into shader class using registration
	GLint m_PositionIndex;
	GLint m_ColorIndex;
	GLint m_NormalIndex;
	GLint m_UVCoordIndex;
	GLint m_TangentIndex;
	GLint m_BitangentIndex;

	GLint m_uniformEyePositionIndex;
	GLint m_uniformModelMatrixIndex;
	GLint m_uniformViewMatrixIndex;
	GLint m_uniformModelViewMatrixIndex;
	GLint m_uniformViewProjectionMatrixIndex;
	GLint m_uniformNormalMatrixIndex;

	/* Move into OGLUniformBlock - OGLLightsBlock
	GLint m_uniformBlockLightsIndex;
	GLint m_uniformBlockLightsBindingPoint;
	*/

	OGLLightsBlock *m_pLightsBlock;
};

#endif // ! OGL_VERTEX_SHADER_H_
