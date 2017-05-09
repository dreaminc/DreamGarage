#include "OGLProgramBlendQuad.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLQuad.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLAttachment.h"

OGLTexture *g_pColorTexture = nullptr;

OGLProgramBlendQuad::OGLProgramBlendQuad(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglblendquad")
{
	// empty
}

RESULT OGLProgramBlendQuad::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	
	// Uniforms 
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformBackgroundColor), std::string("u_vec4BackgroundColor")));

	m_pQuad = new OGLQuad(m_pParentImp, 1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f)); // , nullptr, vNormal);
	CN(m_pQuad);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	InitializeFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	g_pColorTexture = (OGLTexture *)m_pParentImp->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_RECTANGLE);

Error:
	return r;
}

RESULT OGLProgramBlendQuad::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebufferInput));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramBlendQuad::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	UseProgram();

	//glDisable(GL_CULL_FACE);

	// Seems to be killing our texture
	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);

	if(m_pOGLFramebuffer != nullptr) 
		BindToFramebuffer(m_pOGLFramebuffer);

	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use separate blending function
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	m_pParentImp->glBlendEquation(GL_FUNC_ADD);
	m_pParentImp->glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	///*
	if (m_pOGLFramebufferInput != nullptr) {
		if (m_fRenderDepth) {
			// TODO: Might be better to formalize this (units are simply routes mapped to the uniform
			m_pParentImp->glActiveTexture(GL_TEXTURE0);
			m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex());

			m_pUniformTextureColor->SetUniform(0);
		}
		else {
			m_pParentImp->glActiveTexture(GL_TEXTURE0);
			m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex());

			m_pUniformTextureColor->SetUniform(0);
		}
	}
	//*/

	/*
	m_pParentImp->glActiveTexture(GL_TEXTURE0);
	m_pParentImp->BindTexture(g_pColorTexture->GetOGLTextureTarget(), g_pColorTexture->GetOGLTextureIndex());
	m_pUniformTextureColor->SetUniform(0);
	//*/

	m_pQuad->Render();

	// Disable blending
	glDisable(GL_BLEND);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramBlendQuad::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramBlendQuad::SetObjectUniforms(DimObj *pDimObj) {
	return R_PASS;
}

RESULT OGLProgramBlendQuad::SetCameraUniforms(camera *pCamera) {
	return R_PASS;
}

RESULT OGLProgramBlendQuad::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	return R_PASS;
}