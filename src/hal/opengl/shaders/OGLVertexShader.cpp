#include "OGLVertexShader.h"

#include "../OpenGLImp.h"

OGLVertexShader::OGLVertexShader(OGLProgram *pParentProgram) :
	OGLShader(pParentProgram, GL_VERTEX_SHADER)
{
	// empty
}

/*
RESULT OGLVertexShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	//GLuint oglProgramID = m_pParentImp->GetOGLProgramID();
	OpenGLImp *pParentImp = GetParentOGLImplementation();
	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();

	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetPositionAttributeName(), &m_PositionIndex), "Failed to acquire position GL location");
	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire color GL location");
	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetNormalAttributeName(), &m_NormalIndex), "Failed to acquire normal GL location");
	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetUVCoordAttributeName(), &m_UVCoordIndex), "Failed to acquire uv coord GL location");
	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetTangentAttributeName(), &m_TangentIndex), "Failed to acquire tangent GL location");
	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetBitangentAttributeName(), &m_BitangentIndex), "Failed to acquire bitangent GL location");
	
Error:
	return r;
}
*/

// TODO: Don't have this hard coded 
/*/
RESULT OGLVertexShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	//GLuint oglProgramID = m_pParentImp->GetOGLProgramID();
	OpenGLImp *pParentImp = GetParentOGLImplementation();
	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();

	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetModelMatrixUniformName(), &m_uniformModelMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetViewMatrixUniformName(), &m_uniformViewMatrixIndex), "Failed to acquire view matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetProjectionMatrixUniformName(), &m_uniformProjectionMatrixIndex), "Failed to acquire view matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetModelViewMatrixUniformName(), &m_uniformModelViewMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetViewProjectionMatrixUniformName(), &m_uniformViewProjectionMatrixIndex), "Failed to acquire projection view matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetNormalMatrixUniformName(), &m_uniformNormalMatrixIndex), "Failed to acquire normal matrix uniform GL location");
	CRM(pParentImp->glGetUniformLocation(oglProgramID, GetViewOrientationMatrixUniformName(), &m_uniformViewOrientationMatrixIndex), "Failed to acquire view orientation quaternion uniform GL location");

	//CRM(m_pParentImp->glGetUniformBlockIndex(oglProgramID, GetLightsUniformBlockName(), &m_uniformBlockLightsIndex), "Failed to acquire lights uniform block GL location");
	CRM(m_pLightsBlock->UpdateUniformBlockIndexFromShader(GetLightsUniformBlockName()), "Failed to acquire lights uniform block GL location");

Error:
	return r;
}
*/

/*
GLint OGLVertexShader::GetEyePositionUniformIndex() {
	return m_uniformEyePositionIndex;
}

GLint OGLVertexShader::GetModelMatrixUniformIndex() {
	return m_uniformModelMatrixIndex;
}

GLint OGLVertexShader::GetViewMatrixUniformIndex() {
	return m_uniformViewMatrixIndex;
}

GLint OGLVertexShader::GetModelViewMatrixUniformIndex() {
	return m_uniformModelViewMatrixIndex;
}

GLint OGLVertexShader::GetViewProjectionMatrixUniformIndex() {
	return m_uniformViewProjectionMatrixIndex;
}

GLint OGLVertexShader::GetNormalMatrixUniformIndex() {
	return m_uniformNormalMatrixIndex;
}
*/

/*
GLint OGLVertexShader::GetLightsUniformBlockBufferIndex() {
	//return m_uniformBlockLightsIndex;
	return m_pLightsBlock->GetBufferIndex();
}

GLint OGLVertexShader::GetLightsUniformBlockIndex() {
	//return m_uniformBlockLightsIndex;
	return m_pLightsBlock->GetBlockIndex();
}

GLint OGLVertexShader::GetLightsUniformBlockBindingPoint() {
	//return m_uniformBlockLightsBindingPoint;
	return m_pLightsBlock->GetBindingPoint();
}
*/
