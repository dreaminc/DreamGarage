#ifndef OGL_VERTEX_SHADER_H_
#define OGL_VERTEX_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLVertexShader.h
// This is a OGL vertex shader object

#include "OGLShader.h"
#include "OGLVertexAttribute.h"

#include <vector>

#define VERTEX_SHADER_POSITION_INDEX 0
#define VERTEX_SHADER_COLOR_INDEX 1
#define VERTEX_SHADER_NORMAL_INDEX 2

class OGLVertexShader : public OGLShader {
public:
	OGLVertexShader(OGLProgram *pParentProgram);
	
	// This is handled by the parent class 
	//~OGLVertexShader(void);

	//RESULT GetAttributeLocationsFromShader();
	//RESULT GetUniformLocationsFromShader();

public:
	/*
	GLint GetEyePositionUniformIndex();
	GLint GetModelMatrixUniformIndex();
	GLint GetViewMatrixUniformIndex();
	GLint GetModelViewMatrixUniformIndex();
	GLint GetViewProjectionMatrixUniformIndex();
	GLint GetNormalMatrixUniformIndex();
	
	// All uniform work should go up into the OGLShader
	RESULT SetEyePositionUniform(point ptEye);
	RESULT SetModelMatrixUniform(matrix<float, 4, 4> matModel);
	RESULT SetViewMatrixUniform(matrix<float, 4, 4> matView);
	RESULT SetProjectionMatrixUniform(matrix<float, 4, 4> matProjection);
	RESULT SetModelViewMatrixUniform(matrix<float, 4, 4> matModelView);
	RESULT SetViewProjectionMatrixUniform(matrix<float, 4, 4> matViewProjection);
	RESULT SetNormalMatrixUniform(matrix<float, 4, 4> matNormal);
	RESULT SetViewOrientationMatrixUniform(matrix<float, 4, 4> matViewOrientaton);
	*/

	/*
	GLint GetLightsUniformBlockBufferIndex();
	GLint GetLightsUniformBlockIndex();
	GLint GetLightsUniformBlockBindingPoint();
	*/

	// TODO: Create OGLAttributes, OGLUniform, OGLUniformBlock objects instead
	//RESULT BindUniformBlocks();
	//RESULT InitializeUniformBlocks();
	//RESULT UpdateUniformBlockBuffers();

	// TODO: [SHADER] This should be redesigned
	/*
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
	RESULT EnableUVCoordAttribute();
	RESULT EnableTangentAttribute();
	RESULT EnableBitangentAttribute();
	*/

private:
	//std::vector<OGLVertexAttribute*> m_vertexAttributes;

	/*
	GLint m_uniformEyePositionIndex;
	GLint m_uniformModelMatrixIndex;
	GLint m_uniformViewMatrixIndex;
	GLint m_uniformProjectionMatrixIndex;
	GLint m_uniformModelViewMatrixIndex;
	GLint m_uniformViewProjectionMatrixIndex;
	GLint m_uniformNormalMatrixIndex;
	GLint m_uniformViewOrientationMatrixIndex;
	*/

	/* Move into OGLUniformBlock - OGLLightsBlock
	GLint m_uniformBlockLightsIndex;
	GLint m_uniformBlockLightsBindingPoint;
	*/
};

#endif // ! OGL_VERTEX_SHADER_H_
