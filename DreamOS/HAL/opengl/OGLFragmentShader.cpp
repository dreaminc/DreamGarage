#include "OGLFragmentShader.h"
#include "OpenGLImp.h"
#include "OGLTexture.h"
#include "OGLObj.h"

OGLFragmentShader::OGLFragmentShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_FRAGMENT_SHADER)
{
	m_pMaterialBlock = new OGLMaterialBlock(pParentImp);
}

RESULT OGLFragmentShader::BindAttributes() {
	RESULT r = R_PASS;

	CRM(m_pParentImp->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());

Error:
	return r;
}

RESULT OGLFragmentShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	CRM(m_pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire position GL location");

Error:
	return r;
}

RESULT OGLFragmentShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentImp->GetOGLProgramID();

	// Uniforms
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetColorTextureUniformName(), &m_uniformColorTextureIndex), "Failed to acquire color texture uniform GL location");
	CRM(m_pParentImp->glGetUniformLocation(oglProgramID, GetBumpTextureUniformName(), &m_uniformBumpTextureIndex), "Failed to acquire bump texture uniform GL location");

	// Blocks
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

RESULT OGLFragmentShader::SetTexture(OGLTexture *pTexture) {
	RESULT r = R_PASS;

	switch (pTexture->GetTextureType()) {
		case texture::TEXTURE_TYPE::TEXTURE_COLOR: {
			CR(SetColorTextureUniform(pTexture->GetTextureNumber()));
		} break;

		case texture::TEXTURE_TYPE::TEXTURE_BUMP: {
			CR(SetBumpTextureUniform(pTexture->GetTextureNumber()));
		} break;

		default: {
			CB(false);
		} break;
	}

	CR(pTexture->OGLActivateTexture());

Error:
	return r;
}

RESULT OGLFragmentShader::SetColorTextureUniform(GLint textureNumber) {
	return SetUniformInteger(textureNumber, GetColorTextureUniformName());
}

RESULT OGLFragmentShader::SetBumpTextureUniform(GLint textureNumber) {
	return SetUniformInteger(textureNumber, GetBumpTextureUniformName());
}

RESULT OGLFragmentShader::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	texture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetColorTexture()) != nullptr) {
		WCR(SetTexture(reinterpret_cast<OGLTexture*>(pTexture)));
	}

	if ((pTexture = pOGLObj->GetBumpTexture()) != nullptr) {
		WCR(SetTexture(reinterpret_cast<OGLTexture*>(pTexture)));
	}

Error:
	return r;
}

