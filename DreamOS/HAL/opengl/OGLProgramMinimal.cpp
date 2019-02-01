#include "OGLProgramMinimal.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramMinimal::OGLProgramMinimal(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "oglminimal", optFlags)
{
	// empty
}

OGLProgramMinimal::OGLProgramMinimal(OpenGLImp *pParentImp, std::string strName, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, strName, optFlags)
{
	// empty
}

RESULT OGLProgramMinimal::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	//InitializeFrameBuffer(GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeFrameBufferWithDepth(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeDepthFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeDepthToTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024);

	// Custom framebuffer output settings
	//CR(InitializeFrameBuffer(GL_DEPTH_COMPONENT24, GL_INT));

	///*
	if (IsPassthru() == false) {
		int pxWidth = m_pParentImp->GetViewport().Width();
		int pxHeight = m_pParentImp->GetViewport().Height();

		m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
		CR(m_pOGLFramebuffer->OGLInitialize());
		CR(m_pOGLFramebuffer->Bind());

		CR(m_pOGLFramebuffer->SetSampleCount(4));

		CR(m_pOGLFramebuffer->MakeColorAttachment());
		CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
		CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

		CR(m_pOGLFramebuffer->MakeDepthAttachment());
		CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());

		CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));
	}
	//*/

Error:
	return r;
}

RESULT OGLProgramMinimal::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Outputs
	if (IsPassthru() == true) {
		CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));
		CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));
	}
	else {
		CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));
	}

Error:
	return r;
}

RESULT OGLProgramMinimal::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT24, GL_INT);
	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);

	if (m_pOGLFramebuffer != nullptr) {
		if (IsPassthru()) {
			m_pOGLFramebuffer->Bind();
		}
		else {
			BindToFramebuffer(m_pOGLFramebuffer);
		}
	}

	glEnable(GL_BLEND);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

//Error:
	return r;
}

RESULT OGLProgramMinimal::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramMinimal::SetObjectUniforms(DimObj *pDimObj) {
	
	// TODO: why is this here twice
	if (m_pUniformModelMatrix != nullptr) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}

	if (m_pUniformModelMatrix != nullptr) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}

	return R_PASS;
}

RESULT OGLProgramMinimal::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramMinimal::SetCameraUniforms(camera *pCamera) {
	if (m_pUniformViewProjectionMatrix != nullptr) {
		auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
		//auto matVP = pCamera->GetProjectionMatrix();
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	return R_PASS;
}

RESULT OGLProgramMinimal::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {

	if (m_pUniformViewProjectionMatrix != nullptr) {
		auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);
		//auto matVP = pStereoCamera->GetProjectionMatrix(eye);
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	return R_PASS;
}