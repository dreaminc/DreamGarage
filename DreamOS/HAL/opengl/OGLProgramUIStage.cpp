#include "OGLProgramUIStage.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

#include "Primitives/matrix/ProjectionMatrix.h"
#include "Primitives/quad.h"

OGLProgramUIStage::OGLProgramUIStage(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "ogluistage")
{
	// empty
}

RESULT OGLProgramUIStage::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Vertex Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

	// Uniform Variables
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	// Textures
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureColor), std::string("u_hasTextureColor")));

	// Clipping
//	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingProjection), std::string("u_mat4ClippingProjection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingEnabled), std::string("u_clippingEnabled")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformQuadCenter), std::string("u_ptQuadCenter")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformQuadNormal), std::string("u_vQuadNormal")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformQuadWidth), std::string("u_quadWidth")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformptOrigin), std::string("u_ptOrigin")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformvOrigin), std::string("u_vOrigin")));
//	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformDot), std::string("u_dot")));

	// Materials 
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	//CR(InitializeFrameBuffer(GL_DEPTH_COMPONENT16, GL_FLOAT));
	/*
	int pxWidth = m_pParentImp->GetViewport().Width();
	int pxHeight = m_pParentImp->GetViewport().Height();

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->SetSampleCount(4));

	CR(m_pOGLFramebuffer->MakeColorAttachment());
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(m_pOGLFramebuffer->MakeDepthAttachment());
	CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());

	CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));
	//*/

Error:
	return r;
}

RESULT OGLProgramUIStage::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("clippingscenegraph", &m_pClippingSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));

	//TODO: MatrixNode(?)
	//CR(MakeInput<ViewMatrix>("clipping_matrix", &m_clippingView, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Outputs
	//CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));
	CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramUIStage::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);


	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		//BindToFramebuffer(m_pOGLFramebuffer);
		m_pOGLFramebuffer->Bind();	
	}

	glEnable(GL_BLEND);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());
//*
//	m_clippingView = m_pCamera->GetViewMatrix(m_pCamera->GetCameraEye());
//*/
//	m_pUniformClippingProjection->SetUniform(m_clippingProjection * m_clippingView);

	m_pUniformptOrigin->SetUniform(point(m_ptOrigin.x(), m_ptOrigin.y(), m_ptOrigin.z(), 0.0f));
	m_pUniformvOrigin->SetUniform(m_vOrigin);

	m_fClippingEnabled = false;
	m_pUniformClippingEnabled->SetUniform(false);
	RenderObjectStore(m_pSceneGraph);

	m_fClippingEnabled = true;
	m_pUniformClippingEnabled->SetUniform(true);
	RenderObjectStore(m_pClippingSceneGraph);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramUIStage::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetTextureDiffuse()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);

		m_pUniformHasTextureColor->SetUniform(true);
	}
	else {
		m_pUniformHasTextureColor->SetUniform(false);
	}

	//	Error:
	return r;
}

RESULT OGLProgramUIStage::SetClippingViewMatrix(ViewMatrix matView) {
	m_clippingView = matView;
	return R_PASS;
}

RESULT OGLProgramUIStage::SetClippingFrustrum(float left, float right, float top, float bottom, float nearPlane, float farPlane) {
	m_clippingProjection = ProjectionMatrix(left, right, top, bottom, nearPlane, farPlane);
	return R_PASS;
}

RESULT OGLProgramUIStage::SetClippingFrustrum(float width, float height, float nearPlane, float farPlane, float angle) {
	//m_clippingProjection = ProjectionMatrix(1.0f, 0.25f, 0.0f, 10.0f);
	//m_clippingProjection = ProjectionMatrix(1.0f, 0.25f, 0.0f, 5.0f, 15.0f);
	//m_clippingProjection = ProjectionMatrix(0.09f, 0.25f, 0.0f, 5.0f, 120.0f);
	//m_clippingProjection = ProjectionMatrix(1.2f, 0.25f, 0.0f, 5.0f, 15.0f);
	m_clippingProjection = ProjectionMatrix(width, height, nearPlane, farPlane, angle);
	return R_PASS;
}

RESULT OGLProgramUIStage::SetOriginPoint(point ptOrigin) {
	RESULT r = R_PASS;
	m_ptOrigin = ptOrigin;
	//CR(m_pUniformptOrigin->SetUniform(ptOrigin));
//Error:
	return r;
}

RESULT OGLProgramUIStage::SetOriginDirection(vector vOrigin) {
	RESULT r = R_PASS;
	//CR(vOrigin.Normalize());
	m_vOrigin = vOrigin;
	//m_vOrigin = -1.0f * vOrigin;
	//CR(m_pUniformvOrigin->SetUniform(vOrigin));
//Error:
	return r;
}

RESULT OGLProgramUIStage::SetObjectUniforms(DimObj *pDimObj) {
	RESULT r = R_PASS;

	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	//TODO: shader likely breaks when pDimObj is not a quad
	auto pQuad = dynamic_cast<quad*>(pDimObj);
	if (pQuad != nullptr) {
		point ptTest = pQuad->GetOrigin();
		point ptTest2 = pQuad->GetOrigin(true);
		m_pUniformQuadCenter->SetUniform(pQuad->GetOrigin(true));
	}

//Error:
	return r;
}

RESULT OGLProgramUIStage::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramUIStage::SetCameraUniforms(camera *pCamera) {
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramUIStage::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	//m_pUniformptOrigin->SetUniform(pStereoCamera->GetOrigin(true));
	//m_pUniformvOrigin->SetUniform(pStereoCamera->GetLookVector());

	return R_PASS;
}