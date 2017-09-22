#include "OGLProgramBlinnPhongShadow.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramBlinnPhongShadow::OGLProgramBlinnPhongShadow(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglblinnphongshadow"),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramBlinnPhongShadow::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Vertex Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeNormal), std::string("inV_vec4Normal")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec4UVCoord")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeTangent), std::string("inV_vec4Tangent")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeBitangent), std::string("inV_vec4Bitangent")));

	// Uniforms
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformDepthViewProjectionMatrix), std::string("u_mat4DepthVP")));
	
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformShadowEmitterDirection), std::string("u_vec4ShadowEmitterDirection")));

	// Billboard boolean uniforms
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfBillboard), std::string("u_fBillboard")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfScale), std::string("u_fScale")));

	// Object position uniforms
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformObjectCenter), std::string("u_vec4ObjectCenter")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformEyePosition), std::string("u_vec4EyePosition")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	// Depth Map Texture
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDepth), std::string("u_textureDepth")));

	int pxWidth = m_pParentImp->GetViewport().Width();
	int pxHeight = m_pParentImp->GetViewport().Height();

	//pxWidth = 1024;
	//pxHeight = 1024;

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->SetSampleCount(4));

	CR(m_pOGLFramebuffer->MakeColorAttachment());

	/*
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTextureMultisample());
	CR(m_pOGLFramebuffer->SetOGLTextureToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE));
	*/
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	///*
	CR(m_pOGLFramebuffer->MakeDepthAttachment());
	//CR(m_pOGLRenderFramebuffer->GetDepthAttachment()->OGLInitializeRenderBufferMultisample(GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, DEFAULT_OVR_MULTI_SAMPLE));
	CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());
	CR(m_pOGLFramebuffer->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));
	//*/


Error:
	return r;
}

RESULT OGLProgramBlinnPhongShadow::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	CR(MakeInput<OGLFramebuffer>("input_shadowdepth_framebuffer", &m_pInputFramebufferShadowDepth));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramBlinnPhongShadow::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT24, GL_INT);
	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr)
		BindToFramebuffer(m_pOGLFramebuffer);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object 
	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramBlinnPhongShadow::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramBlinnPhongShadow::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramBlinnPhongShadow::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramBlinnPhongShadow::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	m_pUniformObjectCenter->SetUniform(pDimObj->GetOrigin());

	quad *pQuad = dynamic_cast<quad *>(pDimObj);
	m_pUniformfBillboard->SetUniform(pQuad != nullptr && pQuad->IsBillboard());
	m_pUniformfScale->SetUniform(pQuad != nullptr && pQuad->IsScaledBillboard());

	return R_PASS;
}

RESULT OGLProgramBlinnPhongShadow::SetCameraUniforms(camera *pCamera) {

	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	if (m_pInputFramebufferShadowDepth != nullptr) {
		DConnection *pInputDConnection = Connection("input_shadowdepth_framebuffer", CONNECTION_TYPE::INPUT)->GetConnectionEntry(0);
		DNode *pProgramNode = pInputDConnection->GetParentNode();
		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(pProgramNode);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());
			m_pUniformShadowEmitterDirection->SetUniform(pOGLProgramShadowDepth->GetShadowEmitterDirection());
		}
		
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(m_pInputFramebufferShadowDepth->GetDepthAttachment()->GetOGLTextureTarget(), 
								  m_pInputFramebufferShadowDepth->GetDepthAttachment()->GetOGLTextureIndex());

		m_pUniformTextureDepth->SetUniform(0);
	}

	point origin = pCamera->GetOrigin();
	m_pUniformEyePosition->SetUniform(point(origin.x(), origin.y(), origin.z(), 1.0f));

	return R_PASS;
}

RESULT OGLProgramBlinnPhongShadow::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	if (m_pInputFramebufferShadowDepth != nullptr) {
		DConnection *pInputDConnection = Connection("input_shadowdepth_framebuffer", CONNECTION_TYPE::INPUT)->GetConnectionEntry(0);
		DNode *pProgramNode = pInputDConnection->GetParentNode();
		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(pProgramNode);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());
			m_pUniformShadowEmitterDirection->SetUniform(pOGLProgramShadowDepth->GetShadowEmitterDirection());
		}

		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(m_pInputFramebufferShadowDepth->GetDepthAttachment()->GetOGLTextureTarget(),
			m_pInputFramebufferShadowDepth->GetDepthAttachment()->GetOGLTextureIndex());

		m_pUniformTextureDepth->SetUniform(0);
	}

	point origin = pStereoCamera->GetOrigin();
	m_pUniformEyePosition->SetUniform(point(origin.x(), origin.y(), origin.z(), 1.0f));

	return R_PASS;
}