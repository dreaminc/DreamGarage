#include "OGLProgramToon.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramToon::OGLProgramToon(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "ogltoon"),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramToon::OGLInitialize() {
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
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	// Textures
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformUseColorTexture), std::string("u_fUseColorTexture")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

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

RESULT OGLProgramToon::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders
	
	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"lightingCommon.shader"), "Failed to add shared vertex shader code");

	// Vertex
	CRM(MakeVertexShader(L"toon.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"toon.frag"), "Failed to create fragment shader");

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
	SetMaterial(&material(10.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));

Error:
	return r;
}

RESULT OGLProgramToon::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph));
	//TODO: CR(MakeInput("lights"));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramToon::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT24, GL_INT);
	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();
	
	if (m_pOGLFramebuffer != nullptr) {
		BindToFramebuffer(m_pOGLFramebuffer);

		// for test
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}

	glEnable(GL_DEPTH_TEST);	// Enable depth test
	glDepthFunc(GL_LEQUAL);		// Accept fragment if it closer to the camera than the former one
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramToon::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());

		if (m_pUniformTextureColor != nullptr)
			m_pUniformTextureColor->SetUniform(0);

		if (m_pUniformUseColorTexture != nullptr)
			m_pUniformUseColorTexture->SetUniform(true);
	}
	else {
		if (m_pUniformUseColorTexture != nullptr)
			m_pUniformUseColorTexture->SetUniform(false);
	}

	//	Error:
	return r;
}

RESULT OGLProgramToon::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramToon::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramToon::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramToon::SetCameraUniforms(camera *pCamera) {

	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramToon::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix->SetUniform(matM)
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}