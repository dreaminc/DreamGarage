#include "OGLProgramBlinnPhongTextureBumpDisplacement.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramBlinnPhongTextureBumpDisplacement::OGLProgramBlinnPhongTextureBumpDisplacement(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "oglblinnphongtexturebumpdisplacement", optFlags),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::OGLInitialize() {
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformEye), std::string("u_vec4Eye")));
	

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasColorTexture), std::string("u_fHasColorTexture")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureBump), std::string("u_textureBump")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasBumpTexture), std::string("u_fHasBumpTexture")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDisplacement), std::string("u_textureDisplacement")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasDisplacementTexture), std::string("u_fHasDisplacementTexture")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTime), std::string("u_time")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	// Framebuffer Output
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
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
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

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"lightingCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"parallaxCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"noiseCommon.shader"), "Failed to add shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"blinnPhongTextureBumpDisplacement.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"blinnPhongTextureBumpDisplacement.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetupConnections() {
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

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::ProcessNode(long frameID) {
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

	if (m_pUniformTime != nullptr) {
		static std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
		float msTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - timeStart).count();
		m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&msTime));
	}

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		
		if (m_pUniformTextureColor != nullptr)
			m_pUniformTextureColor->SetUniform(0);

		m_pUniformHasColorTexture->SetUniform(true);
	}
	else {
		m_pUniformHasColorTexture->SetUniform(false);
	}

	if ((pTexture = pOGLObj->GetOGLTextureBump()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE1);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		
		if(m_pUniformTextureBump != nullptr) 
			m_pUniformTextureBump->SetUniform(1);

		if(m_pUniformHasBumpTexture != nullptr)
			m_pUniformHasBumpTexture->SetUniform(true);
	}
	else {
		m_pUniformHasBumpTexture->SetUniform(false);
	}

	if ((pTexture = pOGLObj->GetOGLTextureDisplacement()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE2);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
			
		m_pUniformTextureDisplacement->SetUniform(2);

		m_pUniformHasDisplacementTexture->SetUniform(true);
	}
	else {
		m_pUniformHasDisplacementTexture->SetUniform(false);
	}

Error:
	return r;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetCameraUniforms(camera *pCamera) {

	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	if (m_pUniformViewMatrix != nullptr)
		m_pUniformViewMatrix->SetUniform(matV);

	//m_pUniformProjectionMatrix->SetUniform(matP);

	m_pUniformEye->SetUniform(ptEye);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramBlinnPhongTextureBumpDisplacement::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	if(m_pUniformViewMatrix!= nullptr)
		m_pUniformViewMatrix->SetUniform(matV);

	//m_pUniformProjectionMatrix->SetUniform(matP);
	m_pUniformEye->SetUniform(ptEye);
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}