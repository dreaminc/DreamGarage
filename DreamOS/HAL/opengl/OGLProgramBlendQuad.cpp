#include "OGLProgramBlendQuad.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLQuad.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLAttachment.h"

OGLTexture *g_pColorTexture1 = nullptr;
OGLTexture *g_pColorTexture2 = nullptr;

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
	//InitializeFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, 1024, 1024, 4);
	CN(m_pOGLFramebuffer);

	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	// Color attachment
	CR(m_pOGLFramebuffer->MakeColorAttachment());
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	// Depth attachment 
	CR(m_pOGLFramebuffer->MakeDepthAttachment());
	CR(m_pOGLFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(texture::type::TEXTURE_2D, GL_DEPTH_COMPONENT32F, GL_FLOAT));
	CR(m_pOGLFramebuffer->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));

	g_pColorTexture1 = (OGLTexture *)m_pParentImp->MakeTexture(texture::type::TEXTURE_2D, L"brickwall_color.jpg");
	g_pColorTexture2 = (OGLTexture *)m_pParentImp->MakeTexture(texture::type::TEXTURE_2D, L"brick1_color.jpg");

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

RESULT OGLProgramBlendQuad::ClearFrameBuffer() {
	RESULT r = R_PASS;

	CN(m_pOGLFramebuffer);
	CR(m_pOGLFramebuffer->Bind());

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pass = 0;

Error:
	return r;
}

RESULT OGLProgramBlendQuad::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	UseProgram();

	// Seems to be killing our texture
	UpdateFramebufferToViewport(GL_DEPTH_COMPONENT32F, GL_FLOAT);

	if (m_pOGLFramebuffer != nullptr) {
		m_pOGLFramebuffer->Bind();
	}

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// Use separate blending function
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	m_pParentImp->glBlendEquation(GL_FUNC_ADD);
	m_pParentImp->glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	if (m_pOGLFramebufferInput != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);
	}

	m_pQuad->Render();

	// Disable blending
	glDisable(GL_BLEND);

	UnbindFramebuffer();

	m_pass++;

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
