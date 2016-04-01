#include "OGLFragmentShader.h"
#include "OpenGLImp.h"

OGLFragmentShader::OGLFragmentShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_FRAGMENT_SHADER)
{
	m_pMaterialBlock = new OGLMaterialBlock(pParentImp);
}

RESULT OGLFragmentShader::GetAttributeLocationsFromShader() {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLFragmentShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pMaterialBlock->UpdateUniformBlockIndexFromShader(GetMaterialUniformBlockName()), "Failed to acquire material uniform block GL location");
	
Error:
	return r;
}

// TODO: Uniform blocks should be OpenGL Program layer
RESULT OGLFragmentShader::BindUniformBlocks() {
	RESULT r = R_PASS;

	CRM(m_pMaterialBlock->BindUniformBlock(), "Failed to bind %s to material uniform block", GetMaterialUniformBlockName());

Error:
	return r;
}

RESULT OGLFragmentShader::InitializeUniformBlocks() {
	RESULT r = R_PASS;

	CR(m_pMaterialBlock->OGLInitialize());

Error:
	return r;
}

RESULT OGLFragmentShader::UpdateUniformBlockBuffers() {
	RESULT r = R_PASS;

	CR(m_pMaterialBlock->UpdateOGLUniformBlockBuffers());

Error:
	return r;
}

RESULT OGLFragmentShader::SetMaterial(material *pMaterial) {
	return m_pMaterialBlock->SetMaterial(pMaterial);
}