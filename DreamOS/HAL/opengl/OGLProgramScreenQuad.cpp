#include "OGLProgramScreenQuad.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLQuad.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"

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
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformBackgroundColor), std::string("u_vec4BackgroundColor")));

	m_pScreenQuad = new OGLQuad(m_pParentImp, 1.0f, 1.0f, 1, 1, nullptr, vector::kVector(1.0f)); // , nullptr, vNormal);
	CN(m_pScreenQuad);

	UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);

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

	UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);

	if(m_pOGLFramebuffer != nullptr) 
		BindToFramebuffer(m_pOGLFramebuffer);

	if (m_pOGLFramebufferInput != nullptr) {
		OGLTexture *pTexture = (OGLTexture*)(m_pOGLFramebufferInput->GetTexture());
		
		pTexture->OGLActivateTexture();
		m_pUniformTextureColor->SetUniform(pTexture);
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