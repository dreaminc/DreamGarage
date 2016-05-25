#include "OpenGLImp.h"
#include "OGLVertexShader.h"

OGLVertexShader::OGLVertexShader(OGLProgram *pParentProgram) :
	OpenGLShader(pParentProgram, GL_VERTEX_SHADER)
{
	// empty
}

RESULT OGLVertexShader::InitializeAttributes() {
	RESULT r = R_PASS;

	CR(GetVertexAttributesFromShader());
	CR(BindAttributes());
	CR(EnableAttributes());

Error:
	return r;
}

RESULT OGLVertexShader::BindAttributes() {
	RESULT r = R_PASS;

	for (auto const& oglVertexAttribute : m_vertexAttributes) {
		CR(oglVertexAttribute->BindAttribute());
	}

Error:
	return r;
}

// TODO: Don't have this hard coded 
RESULT OGLVertexShader::EnableAttributes() {
	RESULT r = R_PASS;

	for (auto const& oglVertexAttribute : m_vertexAttributes) {
		CR(oglVertexAttribute->EnableAttribute());
	}

Error:
	return r;
}


RESULT OGLVertexShader::GetVertexAttributesFromShader() {
	RESULT r = R_PASS;
	
	OpenGLImp *pParentImp = GetParentOGLImplementation();
	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();

	GLint attributes_n;
	CR(pParentImp->glGetProgramInterfaceiv(oglProgramID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &attributes_n));

	GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };

	DEBUG_LINEOUT("%d active attributes", attributes_n);
	for (int i = 0; i < attributes_n; i++) {
		GLint results[3];
		CR(pParentImp->glGetProgramResourceiv(oglProgramID, GL_PROGRAM_INPUT, i, 3, properties, 3, NULL, results));

		GLint pszName_n = results[0] + 1;
		char *pszName = new char[pszName_n];
		CR(pParentImp->glGetProgramResourceName(oglProgramID, GL_PROGRAM_INPUT, i, pszName_n, NULL, pszName));

		DEBUG_LINEOUT("%-5d %s (%s)", results[2], pszName, OpenGLUtility::GetOGLTypeString(results[1]));

		OGLVertexAttribute *pOGLVertexAttribute = new OGLVertexAttribute(m_pParentProgram, pszName, results[2], results[1]);
		m_vertexAttributes.push_back(pOGLVertexAttribute);

		if (pszName != NULL) {
			delete[] pszName;
			pszName = NULL;
		}
	}

Error:
	return r;
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
