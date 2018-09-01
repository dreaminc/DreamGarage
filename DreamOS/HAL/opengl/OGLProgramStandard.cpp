#include "OGLProgramStandard.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramStandard::OGLProgramStandard(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglenvironment"),
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformRiverAnimation), std::string("u_fRiverAnimation")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformAREnabled), std::string("u_fAREnabled")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTime), std::string("u_time")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	// Framebuffer Output
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
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Reflection Map
	//CR(MakeInput<OGLFramebuffer>("input_reflection_map", &m_pOGLReflectionFramebuffer));

	// Outputs
	// Treat framebuffer as pass-thru
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

RESULT OGLProgramStandard::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

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

	m_pUniformAREnabled->SetUniform(m_fAREnabled);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

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
	if (pOGLObj->GetOGLTextureBump() != nullptr) {
		SetTextureUniform(pOGLObj->GetOGLTextureBump(), m_pUniformTextureBump, m_pUniformHasTextureBump, 0);
	}

	// Material textures
	//SetTextureUniform(pOGLObj->GetOGLTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient, 2);
	if (pOGLObj->GetOGLTextureDiffuse() != nullptr) {
		SetTextureUniform(pOGLObj->GetOGLTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse, 3);
	}
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

	//auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	//auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramStandard::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto deltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
	m_deltaTime = (float)deltaTime;
	m_deltaTime *= 0.5f;
	m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&m_deltaTime));

	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	//auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	m_pUniformViewMatrix->SetUniform(matV);
	//m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix->SetUniform(matM)
	m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return R_PASS;
}

RESULT OGLProgramStandard::SetRiverAnimation(bool fRiverAnimation) {
	m_pUniformRiverAnimation->SetUniform(fRiverAnimation);

	return R_PASS;
}

RESULT OGLProgramStandard::SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit) {
	RESULT r = R_PASS;
	
	if (pTexture) {
		pBoolUniform->SetUniform(true);

		m_pParentImp->glActiveTexture(GL_TEXTURE0 + texUnit);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());

		pTextureUniform->SetUniform(texUnit);
	}
	else {
		pBoolUniform->SetUniform(false);
	}

//Error:
	return r;
}