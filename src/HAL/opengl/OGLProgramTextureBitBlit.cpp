#include "OGLProgramTextureBitBlit.h"

OGLProgramTextureBitBlit::OGLProgramTextureBitBlit(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "ogltexturebitblit", optFlags)
{
	// empty
}

RESULT OGLProgramTextureBitBlit::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Vertex Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

	// Uniform Variables
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTexture), std::string("u_hasTexture")));

Error:
	return r;
}

RESULT OGLProgramTextureBitBlit::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramTextureBitBlit::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
	
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);

		m_pUniformHasTexture->SetUniform(true);
	}
	else {
		m_pUniformHasTexture->SetUniform(false);
	}

	//	Error:
	return r;
}

RESULT OGLProgramTextureBitBlit::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramTextureBitBlit::SetCameraUniforms(camera *pCamera) {
	m_pUniformProjectionMatrix->SetUniform(pCamera->GetProjectionMatrix());

	return R_PASS;
}

RESULT OGLProgramTextureBitBlit::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	m_pUniformProjectionMatrix->SetUniform(pStereoCamera->GetProjectionMatrix(eye));

	return R_PASS;
}