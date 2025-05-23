#include "OGLProgramSSAO.h"

#include "OGLImp.h"
#include "OGLQuad.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLAttachment.h"

#include "core/camera/stereocamera.h"

OGLProgramSSAO::OGLProgramSSAO(OGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "oglssao", optFlags)
{
	// empty
}

OGLProgramSSAO::OGLProgramSSAO(OGLImp *pParentImp, std::string strName, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, strName, optFlags)
{
	// empty
}

RESULT OGLProgramSSAO::OGLInitialize() {
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformWindowWidth), std::string("u_windowWidth")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformWindowHeight), std::string("u_windowHeight")));

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
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	//CR(m_pOGLFramebuffer->MakeDepthAttachment());
	//CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());
	//*/

	//CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));

	CRM(m_pOGLFramebuffer->CheckStatus(), "Frame buffer messed up");

Error:
	return r;
}

RESULT OGLProgramSSAO::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"AACommon.shader"), "Failed to add global shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"ssao.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"ssao.frag"), "Failed to create fragment shader");

	// Link the program
	CRM(LinkProgram(), "Failed to link program");

	WCR(GetVertexAttributesFromProgram());
	WCR(BindAttributes());

	// Uniform Variables
	CR(GetUniformVariablesFromProgram());

	// Uniform Blocks
	CR(GetUniformBlocksFromProgram());
	CR(BindUniformBlocks());

Error:
	return r;
}

RESULT OGLProgramSSAO::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebufferInput));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramSSAO::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	UseProgram();

	// Set up texture bindings
	//m_pUniformColorTexture->SetUniform(0);
	//m_pUniformColorTextureMS->SetUniform(1);

	//glDisable(GL_CULL_FACE);

	// Seems to be killing our texture
	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToCamera(m_pParentImp->GetCamera(), GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	//CRM(m_pOGLFramebuffer->CheckStatus(), "Frame buffer messed up");

	if (m_pOGLFramebuffer != nullptr) {
		BindToFramebuffer(m_pOGLFramebuffer);
	}

	if(m_pUniformWindowWidth != nullptr)
		m_pUniformWindowWidth->SetUniform((float)m_pOGLFramebufferInput->GetWidth());

	if(m_pUniformWindowHeight != nullptr)
		m_pUniformWindowHeight->SetUniform((float)m_pOGLFramebufferInput->GetHeight());

	glDisable(GL_BLEND);

	if (m_pOGLFramebufferInput != nullptr) {
		int sampleCount = m_pOGLFramebufferInput->GetSampleCount();

		if (sampleCount > 1) {
			m_pParentImp->glActiveTexture(GL_TEXTURE1);
			
			m_pUniformColorTextureMS_n->SetUniformInteger(sampleCount);

			if (m_fRenderDepth) {
				m_pParentImp->BindTexture(
					m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), 
					m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex()
				);
			}
			else {
				m_pParentImp->BindTexture(
					m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), 
					m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex()
				);
			}

			//m_pUniformColorTextureMS->SetUniform(1);
			m_pFUniformTextureMS->SetUniform(true);
		}
		else {
			m_pParentImp->glActiveTexture(GL_TEXTURE0);
			
			if (m_fRenderDepth) {
				m_pParentImp->BindTexture(
					m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), 
					m_pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex()
				);
			}
			else {
				m_pParentImp->BindTexture(
					m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureTarget(), 
					m_pOGLFramebufferInput->GetColorAttachment()->GetOGLTextureIndex()
				);
			}
			
			m_pFUniformTextureMS->SetUniform(false);
		}
	}

	m_pScreenQuad->Render();

	UnbindFramebuffer();

Error:
	return r;
}

RESULT OGLProgramSSAO::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramSSAO::SetObjectUniforms(DimObj *pDimObj) {
	return R_PASS;
}

RESULT OGLProgramSSAO::SetCameraUniforms(camera *pCamera) {
	return R_PASS;
}

RESULT OGLProgramSSAO::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	return R_PASS;
}