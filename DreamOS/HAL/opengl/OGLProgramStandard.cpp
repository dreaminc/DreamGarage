#include "OGLProgramStandard.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"
#include "OGLCubemap.h"

OGLProgramStandard::OGLProgramStandard(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgram(pParentImp, "oglenvironment", optFlags),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramStandard::OGLInitialize() {
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformEyePosition), std::string("u_vec4Eye")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureBump), std::string("u_hasBumpTexture")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureBump), std::string("u_textureBump")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureColor), std::string("u_hasTextureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureAmbient), std::string("u_hasTextureAmbient")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureAmbient), std::string("u_textureAmbient")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureDiffuse), std::string("u_hasTextureDiffuse")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDiffuse), std::string("u_textureDiffuse")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureSpecular), std::string("u_hasTextureSpecular")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureSpecular), std::string("u_textureSpecular")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformCubemapEnvironment), std::string("u_cubemapEnvironment")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasCubemapEnvironment), std::string("u_hasCubemapEnvironment")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformCubemapIrradiance), std::string("u_cubemapIrradiance")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasCubemapIrradiance), std::string("u_hasCubemapIrradiance")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformRiverAnimation), std::string("u_fRiverAnimation")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformAREnabled), std::string("u_fAREnabled")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTime), std::string("u_time")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	// Framebuffer Output
	if (IsPassthru() == false) {
		int pxWidth = m_pParentImp->GetViewport().Width();
		int pxHeight = m_pParentImp->GetViewport().Height();

		m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
		CR(m_pOGLFramebuffer->OGLInitialize());
		CR(m_pOGLFramebuffer->Bind());

		CR(m_pOGLFramebuffer->SetSampleCount(1));

		CR(m_pOGLFramebuffer->MakeColorAttachment());
		CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::type::TEXTURE_2D));
		CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

		CR(m_pOGLFramebuffer->MakeDepthAttachment());
		CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());
		CR(m_pOGLFramebuffer->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));

		CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));
	}

	m_deltaTime = 0.0f;
	m_startTime = std::chrono::high_resolution_clock::now();

Error:
	return r;
}

RESULT OGLProgramStandard::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"lightingCommon.shader"), "Failed to add shared vertex shader code");
	CRM(AddSharedShaderFilename(L"brdfCommon.shader"), "Failed to add shared vertex shader code");

	// Vertex
	CRM(MakeVertexShader(L"standard.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"standard.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramStandard::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	CR(MakeInput<OGLFramebuffer>("input_framebuffer_environment_cubemap", &m_pOGLInputFramebufferEnvironmentCubemap, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer_irradiance_cubemap", &m_pOGLInputFramebufferIrradianceCubemap, PIPELINE_FLAGS::PASSIVE));

	// Reflection Map
	//CR(MakeInput<OGLFramebuffer>("input_reflection_map", &m_pOGLReflectionFramebuffer));

	// Outputs
	// Treat framebuffer as pass-thru
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

RESULT OGLProgramStandard::SetCubemap(cubemap *pCubemap) {

	m_pCubemap = pCubemap;

	return R_PASS;
}

RESULT OGLProgramStandard::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	if (IsPassthru() == false) {
		UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);
	}

	UseProgram();

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

	m_pUniformAREnabled->SetUniform(m_fAREnabled);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// Environment map
	if (m_pOGLInputFramebufferEnvironmentCubemap != nullptr && m_pUniformCubemapEnvironment != nullptr) {
		if (m_pUniformHasCubemapEnvironment != nullptr) {
			m_pUniformHasCubemapEnvironment->SetUniform(true);

			m_pParentImp->glActiveTexture(GL_TEXTURE5);
			m_pParentImp->BindTexture(m_pOGLInputFramebufferEnvironmentCubemap->GetColorAttachment()->GetOGLCubemapTarget(),
										m_pOGLInputFramebufferEnvironmentCubemap->GetColorAttachment()->GetOGLCubemapIndex());

			//m_pUniformCubemapEnvironment->SetUniform(5);
		}
	}
	//else if(m_pCubemap != nullptr && m_pUniformCubemapEnvironment != nullptr) {
	//	OGLCubemap *pOGLCubemap = dynamic_cast<OGLCubemap*>(m_pCubemap);
	//
	//	if (m_pUniformHasCubemapEnvironment != nullptr && pOGLCubemap != nullptr) {
	//		m_pUniformHasCubemapEnvironment->SetUniform(true);
	//
	//		m_pParentImp->glActiveTexture(GL_TEXTURE0);
	//		m_pParentImp->BindTexture(pOGLCubemap->GetOGLTextureTarget(), pOGLCubemap->GetOGLTextureIndex());
	//	
	//		m_pUniformCubemapEnvironment->SetUniform(0);
	//	}
	//}
	else {
		if (m_pUniformHasCubemapEnvironment != nullptr)
			m_pUniformHasCubemapEnvironment->SetUniform(false);
	}

	// Irradiance Map
	if (m_pOGLInputFramebufferIrradianceCubemap != nullptr && m_pUniformCubemapIrradiance != nullptr) {
		if (m_pOGLInputFramebufferIrradianceCubemap != nullptr) {
			if(m_pUniformHasCubemapIrradiance != nullptr)
				m_pUniformHasCubemapIrradiance->SetUniform(true);

			m_pParentImp->glActiveTexture(GL_TEXTURE6);

			m_pParentImp->BindTexture(m_pOGLInputFramebufferIrradianceCubemap->GetColorAttachment()->GetOGLCubemapTarget(),
				m_pOGLInputFramebufferIrradianceCubemap->GetColorAttachment()->GetOGLCubemapIndex());

			//m_pUniformTextureCubemap->SetUniform(0);
		}
		else {
			if (m_pUniformHasCubemapIrradiance != nullptr)
				m_pUniformHasCubemapIrradiance->SetUniform(false);
		}
	}

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramStandard::SetIsAugmented(bool fAugmented) {
	m_fAREnabled = fAugmented;
	return R_PASS;
}

RESULT OGLProgramStandard::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	// Bump
	SetTextureUniform(pOGLObj->GetOGLTextureBump(), m_pUniformTextureBump, m_pUniformHasTextureBump, 0);

	// Material textures
	//SetTextureUniform(pOGLObj->GetOGLTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient, 2);
	SetTextureUniform(pOGLObj->GetOGLTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse, 3);
	//SetTextureUniform(pOGLObj->GetOGLTextureSpecular(), m_pUniformTextureSpecular, m_pUniformHasTextureSpecular, 4);

	//	Error:
	return r;
}

RESULT OGLProgramStandard::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramStandard::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramStandard::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramStandard::SetCameraUniforms(camera *pCamera) {

	if (m_pUniformTime != nullptr) {
		auto deltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
		m_deltaTime = (float)deltaTime;
		m_deltaTime *= 0.5f;
		m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&m_deltaTime));
	}

	//auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	//auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	if (m_pUniformEyePosition != nullptr) {
		m_pUniformEyePosition->SetUniform(m_pCamera->GetPosition(true));
	}

	return R_PASS;
}

RESULT OGLProgramStandard::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	
	if (m_pUniformTime != nullptr) {
		auto deltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
		m_deltaTime = (float)deltaTime;
		m_deltaTime *= 0.5f;
		m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&m_deltaTime));
	}

	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	//auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix->SetUniform(matM)
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	if (m_pUniformEyePosition != nullptr) {
		m_pUniformEyePosition->SetUniform(pStereoCamera->GetEyePosition(eye));
	}

	return R_PASS;
}

RESULT OGLProgramStandard::SetRiverAnimation(bool fRiverAnimation) {
	m_pUniformRiverAnimation->SetUniform(fRiverAnimation);

	return R_PASS;
}

RESULT OGLProgramStandard::SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit) {
	RESULT r = R_PASS;
	
	if (pTexture != nullptr && pTextureUniform != nullptr) {
		if (pBoolUniform != nullptr) {
			pBoolUniform->SetUniform(true);

			m_pParentImp->glActiveTexture(GL_TEXTURE0 + texUnit);
			m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());

			pTextureUniform->SetUniform(texUnit);
		}
	}
	else {
		if (pBoolUniform != nullptr)
			pBoolUniform->SetUniform(false);
	}

//Error:
	return r;
}
