#include "OGLProgramMinimalTexture.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramMinimalTexture::OGLProgramMinimalTexture(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglminimaltexture")
{
	// empty
}

RESULT OGLProgramMinimalTexture::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	// Vertex Attributes
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

	// Uniform Variables
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureColor), std::string("u_hasTextureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));

	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	//CR(InitializeFrameBuffer(GL_DEPTH_COMPONENT16, GL_FLOAT));

	///*
	if (m_fPassThru == false) {
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

RESULT OGLProgramMinimalTexture::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"fogCommon.shader"), "Failed to add shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"minimalTexture.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"minimalTexture.frag"), "Failed to create fragment shader");

	// Link the program
	CRM(LinkProgram(), "Failed to link program");

	// TODO: This could all be done in one call in the OGLShader honestly
	// Attributes
	// TODO: Tabulate attributes (get them from shader, not from class)
	WCR(GetVertexAttributesFromProgram());
	WCR(BindAttributes());

	//CR(PrintActiveAttributes());

	// Uniform Variables
	CR(GetUniformVariablesFromProgram());

	// Uniform Blocks
	CR(GetUniformBlocksFromProgram());
	CR(BindUniformBlocks());

	// TODO:  Currently using a global material 
	SetMaterial(&material(60.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));

Error:
	return r;
}

RESULT OGLProgramMinimalTexture::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Outputs
	if (m_fPassThru == true) {
		CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));
		CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));
	}
	else {
		CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));
	}

Error:
	return r;
}

RESULT OGLProgramMinimalTexture::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		if (m_fPassThru) {
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

RESULT OGLProgramMinimalTexture::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
		//pTexture->OGLActivateTexture(0);
		//m_pUniformTextureColor->SetUniform(pTexture);

		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);
		m_pUniformHasTextureColor->SetUniform(true);
	}
	else {
		m_pUniformHasTextureColor->SetUniform(false);
	}

Error:
	return r;
}

RESULT OGLProgramMinimalTexture::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramMinimalTexture::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramMinimalTexture::SetCameraUniforms(camera *pCamera) {
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramMinimalTexture::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}