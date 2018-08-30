#include "OGLFragmentShader.h"

#include "../OGLProgram.h"
#include "../OGLTexture.h"
#include "../OGLObj.h"

OGLFragmentShader::OGLFragmentShader(OGLProgram *pParentProgram) :
	OGLShader(pParentProgram, GL_FRAGMENT_SHADER)
{
	// empty
}

RESULT OGLFragmentShader::BindAttributes() {
	RESULT r = R_PASS;

	CRM(m_pParentProgram->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());

Error:
	return r;
}

RESULT OGLFragmentShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	CRM(pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire position GL location");

Error:
	return r;
}

/*
RESULT OGLFragmentShader::GetUniformLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	// Uniforms
	WCRM(pParentImp->glGetUniformLocation(oglProgramID, GetColorTextureUniformName(), &m_uniformColorTextureIndex), "Failed to acquire color texture uniform GL location");
	WCRM(pParentImp->glGetUniformLocation(oglProgramID, GetBumpTextureUniformName(), &m_uniformBumpTextureIndex), "Failed to acquire bump texture uniform GL location");

	// Blocks
	WCRM(m_pMaterialBlock->UpdateUniformBlockIndexFromShader(GetMaterialUniformBlockName()), "Failed to acquire material uniform block GL location");
	
Error:
	return r;
}
*/

/*
// TODO: Uniform blocks should be OpenGL Program layer
RESULT OGLFragmentShader::BindUniformBlocks() {
	RESULT r = R_PASS;

	WCRM(m_pMaterialBlock->BindUniformBlock(), "Failed to bind %s to material uniform block", GetMaterialUniformBlockName());

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
*/

// TODO: Remove all of the binding stuff?
RESULT OGLFragmentShader::SetTexture(OGLTexture *pTexture) {

	/*
	switch (pTexture->GetTextureType()) {

		RESULT r = R_PASS;

		//case texture::type::TEXTURE_DIFFUSE: {
		//	//CR(SetColorTextureUniform(pTexture->GetTextureNumber()));
		//	CR(SetColorTextureUniform(pTexture->GetOGLTextureIndex()));
		//} break;
		//
		//case texture::type::TEXTURE_BUMP: {
		//	//CR(SetBumpTextureUniform(pTexture->GetTextureNumber()));
		//	CR(SetBumpTextureUniform(pTexture->GetOGLTextureIndex()));
		//} break;

		default: {
			CB(false);
		} break;
	}
	*/

	//CR(pTexture->OGLActivateTexture());

Error:
	return R_DEPRECATED;
}


RESULT OGLFragmentShader::SetColorTextureUniform(GLint textureNumber) {
	//return SetUniformInteger(textureNumber, GetColorTextureUniformName());
	return R_NOT_IMPLEMENTED;
}

RESULT OGLFragmentShader::SetBumpTextureUniform(GLint textureNumber) {
	//return SetUniformInteger(textureNumber, GetBumpTextureUniformName());
	return R_NOT_IMPLEMENTED;
}

RESULT OGLFragmentShader::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
		WCR(SetTexture(pTexture));
	}

	if ((pTexture = pOGLObj->GetOGLTextureBump()) != nullptr) {
		WCR(SetTexture(pTexture));
	}

//Error:
	return r;
}

