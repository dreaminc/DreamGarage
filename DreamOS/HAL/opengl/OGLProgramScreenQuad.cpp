#include "OGLProgramScreenQuad.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLQuad.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLAttachment.h"

OGLProgramScreenQuad::OGLProgramScreenQuad(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglscreenquad")
{
	// empty
}

RESULT OGLProgramScreenQuad::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	
	// Uniforms 
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformColorTexture), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformColorTextureMS), std::string("u_textureColorMS")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pFUniformTextureMS), std::string("u_fTextureMS")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformColorTextureMS_n), std::string("u_textureColorMS_n")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformBackgroundColor), std::string("u_vec4BackgroundColor")));

	m_pScreenQuad = new OGLQuad(m_pParentImp, 1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f)); // , nullptr, vNormal);
	CN(m_pScreenQuad);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	///*
	int pxWidth = m_pParentImp->GetViewport().Width();
	int pxHeight = m_pParentImp->GetViewport().Height();

	//pxWidth = 1024;
	//pxHeight = 1024;

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->SetSampleCount(1));

	CR(m_pOGLFramebuffer->MakeColorAttachment());
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	//CR(m_pOGLFramebuffer->MakeDepthAttachment());
	//CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());
	//*/

Error:
	return r;
}

RESULT OGLProgramScreenQuad::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebufferInput));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramScreenQuad::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	UseProgram();

	//glDisable(GL_CULL_FACE);

	// Seems to be killing our texture
	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToCamera(m_pParentImp->GetCamera(), GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	if (m_pOGLFramebuffer != nullptr) {
		BindToFramebuffer(m_pOGLFramebuffer);
	}

	glDisable(GL_BLEND);

	if (m_pOGLFramebufferInput != nullptr) {
		int sampleCount = m_pOGLFramebufferInput->GetSampleCount();

		if (sampleCount > 1) {
			m_pParentImp->glActiveTexture(GL_TEXTURE1);
			
			m_pUniformColorTextureMS_n->SetUniformInteger(sampleCount);

			if (m_fRenderDepth) {
				m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex());
			}
			else {
				m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex());
			}

			m_pUniformColorTextureMS->SetUniform(1);
			m_pFUniformTextureMS->SetUniform(true);
		}
		else {
			m_pParentImp->glActiveTexture(GL_TEXTURE0);

			if (m_fRenderDepth) {
				m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex());
			}
			else {
				m_pParentImp->BindTexture(m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex());
			}

			m_pUniformColorTextureMS->SetUniform(0);
			m_pFUniformTextureMS->SetUniform(false);
		}
	}

	m_pScreenQuad->Render();

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramScreenQuad::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramScreenQuad::SetObjectUniforms(DimObj *pDimObj) {
	return R_PASS;
}

RESULT OGLProgramScreenQuad::SetCameraUniforms(camera *pCamera) {
	return R_PASS;
}

RESULT OGLProgramScreenQuad::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	return R_PASS;
}