#include "OpenGLImp.h"
#include "OGLVertexShader.h"

OGLVertexShader::OGLVertexShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_VERTEX_SHADER)
{
	m_pLightsBlock = new OGLLightsBlock(pParentImp);
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

	CRM(m_pLightsBlock->BindUniformBlock(), "Failed to bind %s to lights uniform block", GetLightsUniformBlockName());

Error:
	return r;
}

RESULT OGLVertexShader::InitializeUniformBlocks() {
	RESULT r = R_PASS;

	CR(m_pLightsBlock->OGLInitialize());

Error:
	return r;
}

RESULT OGLVertexShader::UpdateUniformBlockBuffers() {
	RESULT r = R_PASS;

	CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());

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
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewMatrixUniformName(), &m_uniformViewMatrixIndex), "Failed to acquire view matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetModelViewMatrixUniformName(), &m_uniformModelViewMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewProjectionMatrixUniformName(), &m_uniformViewProjectionMatrixIndex), "Failed to acquire projection view matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetNormalMatrixUniformName(), &m_uniformNormalMatrixIndex), "Failed to acquire normal matrix uniform GL location");

	//CRM(m_pParentImp->glGetUniformBlockIndex(oglProgramID, GetLightsUniformBlockName(), &m_uniformBlockLightsIndex), "Failed to acquire lights uniform block GL location");
	CRM(m_pLightsBlock->UpdateUniformBlockIndexFromShader(GetLightsUniformBlockName()), "Failed to acquire lights uniform block GL location");

Error:
	return r;
}

RESULT OGLVertexShader::SetLights(std::vector<light*> *pLights) {
	return m_pLightsBlock->SetLights(pLights);
}

GLint OGLVertexShader::GetPositionIndex() {
	//return VERTEX_SHADER_POSITION_INDEX;
	return m_PositionIndex;
}

GLint OGLVertexShader::GetColorIndex() {
	//return VERTEX_SHADER_COLOR_INDEX;
	return m_ColorIndex;
}

GLint OGLVertexShader::GetNormalIndex() {
	//return VERTEX_SHADER_NORMAL_INDEX;
	return m_NormalIndex;
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

// Set Matrix Functions
RESULT OGLVertexShader::SetModelMatrixUniform(matrix<float, 4, 4> matModel) {
	return Set44MAtrixUniform(matModel, GetModelMatrixUniformName());
}

RESULT OGLVertexShader::SetViewMatrixUniform(matrix<float, 4, 4> matView) {
	return Set44MAtrixUniform(matView, GetViewMatrixUniformName());
}

RESULT OGLVertexShader::SetModelViewMatrixUniform(matrix<float, 4, 4> matModelView) {
	return Set44MAtrixUniform(matModelView, GetModelViewMatrixUniformName());
}

RESULT OGLVertexShader::SetViewProjectionMatrixUniform(matrix<float, 4, 4> matViewProjection) {
	return Set44MAtrixUniform(matViewProjection, GetViewProjectionMatrixUniformName());
}

RESULT OGLVertexShader::SetNormalMatrixUniform(matrix<float, 4, 4> matNormal) {
	return Set44MAtrixUniform(matNormal, GetNormalMatrixUniformName());
}
