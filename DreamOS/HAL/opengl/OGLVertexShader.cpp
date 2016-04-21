#include "OpenGLImp.h"
#include "OGLVertexShader.h"

OGLVertexShader::OGLVertexShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_VERTEX_SHADER)
{
	m_pLightsBlock = new OGLLightsBlock(pParentImp);
}

RESULT OGLVertexShader::BindAttributes() {
	RESULT r = R_PASS;
	
	WCRM(m_pParentImp->BindAttribLocation(GetNormalIndex(), (char*)GetNormalAttributeName()), "Failed to bind %s to normal attribute", GetNormalAttributeName());
	WCRM(m_pParentImp->BindAttribLocation(GetUVCoordIndex(), (char*)GetUVCoordAttributeName()), "Failed to bind %s to uv coord attribute", GetUVCoordAttributeName());
	WCRM(m_pParentImp->BindAttribLocation(GetTangentIndex(), (char*)GetTangentAttributeName()), "Failed to bind %s to tangent attribute", GetTangentAttributeName());
	WCRM(m_pParentImp->BindAttribLocation(GetBitangentIndex(), (char*)GetBitangentAttributeName()), "Failed to bind %s to bitangent attribute", GetBitangentAttributeName());

	WCRM(m_pParentImp->BindAttribLocation(GetPositionIndex(), (char*)GetPositionAttributeName()), "Failed to bind %s to position attribute", GetPositionAttributeName());
	WCRM(m_pParentImp->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());

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

RESULT OGLVertexShader::EnableUVCoordAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetUVCoordIndex());
}

RESULT OGLVertexShader::EnableTangentAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetTangentIndex());
}

RESULT OGLVertexShader::EnableBitangentAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetBitangentIndex());
}

RESULT OGLVertexShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetPositionAttributeName(), &m_PositionIndex), "Failed to acquire position GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire color GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetNormalAttributeName(), &m_NormalIndex), "Failed to acquire normal GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetUVCoordAttributeName(), &m_UVCoordIndex), "Failed to acquire uv coord GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetTangentAttributeName(), &m_TangentIndex), "Failed to acquire tangent GL location");
	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetBitangentAttributeName(), &m_BitangentIndex), "Failed to acquire bitangent GL location");
	
Error:
	return r;
}

RESULT OGLVertexShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetModelMatrixUniformName(), &m_uniformModelMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewMatrixUniformName(), &m_uniformViewMatrixIndex), "Failed to acquire view matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetProjectionMatrixUniformName(), &m_uniformProjectionMatrixIndex), "Failed to acquire view matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetModelViewMatrixUniformName(), &m_uniformModelViewMatrixIndex), "Failed to acquire model matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewProjectionMatrixUniformName(), &m_uniformViewProjectionMatrixIndex), "Failed to acquire projection view matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetNormalMatrixUniformName(), &m_uniformNormalMatrixIndex), "Failed to acquire normal matrix uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetViewOrientationMatrixUniformName(), &m_uniformViewOrientationMatrixIndex), "Failed to acquire view orientation quaternion uniform GL location");

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

GLint OGLVertexShader::GetUVCoordIndex() {
	return m_UVCoordIndex;
}

GLint OGLVertexShader::GetTangentIndex() {
	return m_TangentIndex;
}

GLint OGLVertexShader::GetBitangentIndex() {
	return m_BitangentIndex;
}

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
// TODO: This should be generalized 
RESULT OGLVertexShader::SetEyePositionUniform(point ptEye) {
	return SetPointUniform(ptEye, GetEyePositionUniformName());
}

RESULT OGLVertexShader::SetModelMatrixUniform(matrix<float, 4, 4> matModel) {
	return Set44MatrixUniform(matModel, GetModelMatrixUniformName());
}

RESULT OGLVertexShader::SetViewMatrixUniform(matrix<float, 4, 4> matView) {
	return Set44MatrixUniform(matView, GetViewMatrixUniformName());
}

RESULT OGLVertexShader::SetProjectionMatrixUniform(matrix<float, 4, 4> matProjection) {
	return Set44MatrixUniform(matProjection, GetProjectionMatrixUniformName());
}

RESULT OGLVertexShader::SetModelViewMatrixUniform(matrix<float, 4, 4> matModelView) {
	return Set44MatrixUniform(matModelView, GetModelViewMatrixUniformName());
}

RESULT OGLVertexShader::SetViewProjectionMatrixUniform(matrix<float, 4, 4> matViewProjection) {
	return Set44MatrixUniform(matViewProjection, GetViewProjectionMatrixUniformName());
}

RESULT OGLVertexShader::SetNormalMatrixUniform(matrix<float, 4, 4> matNormal) {
	return Set44MatrixUniform(matNormal, GetNormalMatrixUniformName());
}

RESULT OGLVertexShader::SetViewOrientationMatrixUniform(matrix<float, 4, 4> matViewOrientaton) {
	return Set44MatrixUniform(matViewOrientaton, GetViewOrientationMatrixUniformName());
}
