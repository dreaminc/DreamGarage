#include "OpenGLImp.h"
#include "OGLVertexShader.h"

OGLVertexShader::OGLVertexShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_VERTEX_SHADER),
	m_uniformBlockLightsBindingPoint(VERTEX_SHADER_LIGHTING_UNIFORM_BLOCK_BINDING_POINT)
{
	// empty
}

RESULT OGLVertexShader::BindAttributes() {
	RESULT r = R_PASS;

	CRM(m_pParentImp->BindAttribLocation(GetPositionIndex(), (char*)GetPositionAttributeName()), "Failed to bind %s to position attribute", GetPositionAttributeName());
	CRM(m_pParentImp->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());
	CRM(m_pParentImp->BindAttribLocation(GetNormalIndex(), (char*)GetNormalAttributeName()), "Failed to bind %s to normal attribute", GetNormalAttributeName());

	// TODO: Normal and Tex Coord etc

Error:
	return r;
}

RESULT OGLVertexShader::BindUniformBlocks() {
	RESULT r = R_PASS;

	CRM(m_pParentImp->BindUniformBlock(GetLightsUniformBlockIndex(), GetLightsUniformBlockBindingPoint()), "Failed to bind %s to lights uniform block", GetLightsUniformBlockName());

Error:
	return r;
}

RESULT OGLVertexShader::EnableVertexPositionAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray((GLuint)GetPositionIndex());
}

RESULT OGLVertexShader::EnableVertexColorAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetColorIndex());
}

RESULT OGLVertexShader::EnableVertexNormalAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetNormalIndex());
}

RESULT OGLVertexShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetPositionAttributeName(), &m_PositionIndex), "Failed to acquire position GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire color GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetNormalAttributeName(), &m_NormalIndex), "Failed to acquire position GL location");
	
Error:
	return r;
}

RESULT OGLVertexShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetModelMatrixUniformName(), &m_uniformModelMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewProjectionMatrixUniformName(), &m_uniformViewProjectionMatrixIndex), "Failed to acquire projection view matrix uniform GL location");

	CRM(m_pParentImp->glGetUniformBlockIndex(oglProgramID, GetLightsUniformBlockName(), &m_uniformBlockLightsIndex), "Failed to acquire lights uniform block GL location");

Error:
	return r;
}
