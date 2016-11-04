#include "OGLUniformBlock.h"

#include "../OpenGLImp.h"

OGLUniformBlock::OGLUniformBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName) :
	GLSLObject(pParentProgram),
	m_uniformBlockDataSize(dataSize),
	m_pUniformBufferData(nullptr),
	m_pUniformBufferData_n(0),
	m_uniformLocationIndex(uniformLocationIndex),
	m_uniformBlockBindingPoint(uniformLocationIndex)
{
	m_strUniformBlockName.assign(pszName);
}

RESULT OGLUniformBlock::OGLInitialize() {
	RESULT r = R_PASS;

	OpenGLImp *pParentImp = GetParentOGLImplementation();

	CR(pParentImp->MakeCurrentContext());

	// Create Buffer Objects
	CR(pParentImp->glGenBuffers(1, &m_uniformBlockBufferIndex));
	CR(pParentImp->glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBlockBufferIndex));

	// Set the data
	// TODO: Code reuse from UpdateOGLUniformBlockBuffers
	/*
	void *pUniformBufferData = NULL;
	GLsizeiptr pUniformBufferData_n = 0;
	CR(GetUniformBlockBuffer(pUniformBufferData, &pUniformBufferData_n));
	CR(pParentImp->glBufferData(GL_UNIFORM_BUFFER, pUniformBufferData_n, pUniformBufferData, GL_DYNAMIC_DRAW));
	*/

	m_pUniformBufferData_n = m_uniformBlockDataSize;
	m_pUniformBufferData = (void*)malloc(m_pUniformBufferData_n);
	memset(m_pUniformBufferData, 0, m_pUniformBufferData_n);
	CR(pParentImp->glBufferData(GL_UNIFORM_BUFFER, m_pUniformBufferData_n, m_pUniformBufferData, GL_DYNAMIC_DRAW));

	// Bind buffer to binding point
	CR(pParentImp->glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBlockBindingPoint, m_uniformBlockBufferIndex));

	//CR(m_pParentImp->ReleaseCurrentContext());

Error:
	return r;
}

// TODO: Mapping done by child - but allocate the space
// this would no longer be needed 
/*
RESULT OGLUniformBlock::GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n) {
	return R_NOT_IMPLEMENTED;
}
*/

RESULT OGLUniformBlock::BindUniformBlock() {
	RESULT r = R_PASS;

	CR(m_pParentProgram->BindUniformBlock(m_uniformLocationIndex, m_uniformBlockBindingPoint));
	
Error:
	return r;
}

RESULT OGLUniformBlock::ReleaseOGLUniformBlockBuffers() {
	RESULT r = R_PASS;

	OpenGLImp *pParentImp = GetParentOGLImplementation();

	if (pParentImp != nullptr) {
		if (m_uniformBlockBufferIndex != NULL) {
			CR(pParentImp->glDeleteBuffers(1, &m_uniformBlockBufferIndex));
			m_uniformBlockBufferIndex = NULL;
		}
	}

Error:
	return r;
}

RESULT OGLUniformBlock::UpdateOGLUniformBlockBuffers() {
	RESULT r = R_PASS;

	OpenGLImp *pParentImp = GetParentOGLImplementation();

	CR(pParentImp->glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBlockBufferIndex));
	//CR(m_pParentImp->glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBlockBindingPoint, m_uniformBlockBufferIndex));

	// Set the data
	/*
	void *pUniformBufferData = NULL;
	GLsizeiptr pUniformBufferData_n = 0;
	CR(GetUniformBlockBuffer(pUniformBufferData, &pUniformBufferData_n));
	(pParentImp->glBufferData(GL_UNIFORM_BUFFER, pUniformBufferData_n, pUniformBufferData, GL_DYNAMIC_DRAW));
	*/

	CR(pParentImp->glBufferData(GL_UNIFORM_BUFFER, m_pUniformBufferData_n, m_pUniformBufferData, GL_DYNAMIC_DRAW));

	

	//CR(m_pParentImp->glBindBufferBase(GL_UNIFORM_BUFFER, m_uniformBlockBindingPoint, m_uniformBlockBufferIndex));

	//CR(m_pParentImp->glBufferSubData(GL_UNIFORM_BUFFER, 0, pUniformBufferData_n, pUniformBufferData));

	//CR(m_pParentImp->ReleaseCurrentContext());

Error:
	return r;
}

RESULT OGLUniformBlock::UpdateUniformBlockIndexFromShader(const char* pszUniformBlockName) {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	CR(pParentImp->glGetUniformBlockIndex(oglProgramID, pszUniformBlockName, &m_uniformBlockIndex));

Error:
	return r;
}

RESULT OGLUniformBlock::SetBufferIndex(GLint bufferIndex) {
	m_uniformBlockBufferIndex = bufferIndex;
	return R_PASS;
}

GLint OGLUniformBlock::GetBufferIndex() {
	return m_uniformBlockBufferIndex;
}

RESULT OGLUniformBlock::SetBlockIndex(GLint blockIndex) {
	m_uniformBlockIndex = blockIndex;
	return R_PASS;
}

GLint OGLUniformBlock::GetBlockIndex() {
	return m_uniformBlockIndex;
}

RESULT OGLUniformBlock::SetBindingPoint(GLint bindingPointIndex) {
	m_uniformBlockBindingPoint = bindingPointIndex;
	return R_PASS;
}

GLint OGLUniformBlock::GetBindingPoint() {
	return m_uniformBlockBindingPoint;
}