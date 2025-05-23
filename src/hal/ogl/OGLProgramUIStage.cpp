#include "OGLProgramUIStage.h"

#include "OGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

#include "core/matrix/ProjectionMatrix.h"

#include "core/primitives/quad.h"

OGLProgramUIStage::OGLProgramUIStage(OGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "ogluistage", optFlags)
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
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingEnabled), std::string("u_clippingEnabled")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformAR), std::string("u_arEnabled")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformQuadCenter), std::string("u_ptQuadCenter")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformParentModelMatrix), std::string("u_mat4ParentModel")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformptOrigin), std::string("u_ptOrigin")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformvOrigin), std::string("u_vOrigin")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingThreshold), std::string("u_clippingThreshold")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingRate), std::string("u_clippingRate")));

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
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("clippingscenegraph", &m_pClippingSceneGraph, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
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

	VirtualObj *pVirtualObj = nullptr;

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	// Update all of the UI objects

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		//BindToFramebuffer(m_pOGLFramebuffer);
		m_pOGLFramebuffer->Bind();	
	}

	glEnable(GL_BLEND);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	m_pUniformptOrigin->SetUniform(point(m_ptOrigin.x(), m_ptOrigin.y(), m_ptOrigin.z(), 0.0f));
	m_pUniformvOrigin->SetUniform(m_vOrigin);

	m_pUniformClippingThreshold->SetUniform(m_clippingThreshold);
	m_pUniformClippingRate->SetUniform(m_clippingRate);

	m_pUniformAR->SetUniform(m_fIsAugmented);

	if (m_pClippingSceneGraph != nullptr) {
		m_pUniformClippingEnabled->SetUniform(true);
		RenderObjectStore(m_pClippingSceneGraph);

		m_pUniformClippingEnabled->SetUniform(false);
	}

	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

Error:
	return r;
}

RESULT OGLProgramUIStage::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
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

RESULT OGLProgramUIStage::SetOriginPoint(point ptOrigin) {
	RESULT r = R_PASS;

	m_ptOrigin = ptOrigin;

	return r;
}

RESULT OGLProgramUIStage::SetOriginDirection(vector vOrigin) {
	RESULT r = R_PASS;

	m_vOrigin = vOrigin;

	return r;
}

RESULT OGLProgramUIStage::SetIsAugmented(bool fAugmented) {
	m_fIsAugmented = fAugmented;
	return R_PASS;
}

RESULT OGLProgramUIStage::SetClippingThreshold(float clippingThreshold) {
	m_clippingThreshold = clippingThreshold;
	return R_PASS;
}

RESULT OGLProgramUIStage::SetClippingRate(float clippingRate) {
	m_clippingRate = clippingRate;
	return R_PASS;
}

RESULT OGLProgramUIStage::SetObjectUniforms(DimObj *pDimObj) {
	RESULT r = R_PASS;

	// Critical path -
	// DimObj should be a quad, shader may break otherwise
	// not using EHM to check

	DimObj* pParent = pDimObj->GetParent();
	if (pParent != nullptr) {
		auto matModelParent = pParent->GetModelMatrix();
		if (pParent != nullptr) {
			m_pUniformQuadCenter->SetUniform(pParent->GetOrigin(true));
			m_pUniformParentModelMatrix->SetUniform(matModelParent);
		}

		auto matModelChild = pDimObj->VirtualObj::GetModelMatrix();
		auto matModel = matModelParent * matModelChild;
		m_pUniformModelMatrix->SetUniform(matModel);
	}
	else {
		//auto pParentModel = pParent->GetModelMatrix();
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
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

	return R_PASS;
}