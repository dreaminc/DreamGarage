#include "OGLProgramFlat.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OGLObj.h"
#include "OGLTexture.h"

OGLProgramFlat::OGLProgramFlat(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglflat")
{
	// empty
}

RESULT OGLProgramFlat::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTexture), std::string("u_hasTexture")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfDistanceMap), std::string("u_fDistanceMap")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformBuffer), std::string("u_buffer")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformGamma), std::string("u_gamma")));

Error:
	return r;
}

RESULT OGLProgramFlat::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<FlatContext>("flatcontext", &m_pFlatContext, DCONNECTION_FLAGS::PASSIVE));

	// Note: This is a special case here, most framebuffer inputs will WANT to be active (another shader)
	CR(MakeInput<framebuffer>("framebuffer", (framebuffer**)(&m_pOGLFramebuffer), DCONNECTION_FLAGS::PASSIVE));

Error:
	return r;
}

RESULT OGLProgramFlat::SetFlatFramebuffer(framebuffer *pFramebuffer) {
	m_pOGLFramebuffer = (OGLFramebuffer*)pFramebuffer;
	return R_PASS;
}

RESULT OGLProgramFlat::SetCamera(stereocamera *pCamera) {
	m_pCamera = pCamera;
	return R_PASS;
}

RESULT OGLProgramFlat::SetFlatContext(FlatContext *pFlatContext) {
	m_pFlatContext = pFlatContext;
	return R_PASS;
}

RESULT OGLProgramFlat::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	//SetFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT, m_pOGLFramebuffer->GetWidth(), m_pOGLFramebuffer->GetHeight(), m_pOGLFramebuffer->GetChannels());

	CR(UseProgram());

	CR(BindToFramebuffer(m_pOGLFramebuffer));

	CR(SetStereoCamera(m_pCamera, EYE_MONO));
	CR(RenderObject(m_pFlatContext));

	CR(UnbindFramebuffer());

Error:
	return r;
}

RESULT OGLProgramFlat::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetColorTexture()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);

		m_pUniformHasTexture->SetUniform(true);
	}
	else {
		m_pUniformHasTexture->SetUniform(false);
	}

	return r;
}

RESULT OGLProgramFlat::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	text *pText = dynamic_cast<text*>(pDimObj);

	m_pUniformfDistanceMap->SetUniform(pText != nullptr && pText->GetFont()->HasDistanceMap());
	if (pText != nullptr) {
		float buffer = pText->GetFont()->GetBuffer();
		float gamma = pText->GetFont()->GetGamma();
		m_pUniformBuffer->SetUniformFloat(&buffer);
		m_pUniformGamma->SetUniformFloat(&gamma);
	}

	return R_PASS;
}

RESULT OGLProgramFlat::SetCameraUniforms(camera *pCamera) {
	auto matP = pCamera->GetProjectionMatrix();
	if (m_pUniformProjectionMatrix)
		m_pUniformProjectionMatrix->SetUniform(matP);

	return R_PASS;
}

RESULT OGLProgramFlat::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	if (m_pUniformProjectionMatrix)
		m_pUniformProjectionMatrix->SetUniform(matP);

	return R_PASS;
}
