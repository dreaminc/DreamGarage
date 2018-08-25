#include "OGLProgramReflection.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

#include "Primitives/matrix/ReflectionMatrix.h"

OGLProgramReflection::OGLProgramReflection(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglreflection"),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramReflection::OGLInitialize() {
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
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformClippingPlane), std::string("u_vec4ClippingPlane")));
	

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

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	// Frame buffer Output
	int pxWidth = m_pParentImp->GetViewport().Width();
	int pxHeight = m_pParentImp->GetViewport().Height();

	//int pxWidth = 256*4;
	//int pxHeight = 256*4;
	m_frameBufferDivisionFactor = 2;

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->SetSampleCount(1));

	CR(m_pOGLFramebuffer->MakeColorAttachment());
	CR(m_pOGLFramebuffer->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE));
	CR(m_pOGLFramebuffer->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(m_pOGLFramebuffer->MakeDepthAttachment());
	CR(m_pOGLFramebuffer->GetDepthAttachment()->OGLInitializeRenderBuffer());
	CR(m_pOGLFramebuffer->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));

Error:
	return r;
}

RESULT OGLProgramReflection::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");
	//CRM(AddSharedShaderFilename(L"lightingCommon.shader"), "Failed to add shared vertex shader code");

	// Vertex
	//CRM(MakeVertexShader(L"standard_clipping.vert"), "Failed to create vertex shader");
	CRM(MakeVertexShader(L"minimalTexture.vert"), "Failed to create vertex shader");

	// Fragment
	//CRM(MakeFragmentShader(L"standard_clipping.frag"), "Failed to create fragment shader");
	CRM(MakeFragmentShader(L"minimalTexture_clipping.frag"), "Failed to create fragment shader");

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
	SetMaterial(&material(1.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));

Error:
	return r;
}

RESULT OGLProgramReflection::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

//RESULT OGLProgramReflection::SetReflectionPlane(plane reflectionPlane) {
//	m_reflectionPlane = reflectionPlane;
//	return R_PASS;
//}

RESULT OGLProgramReflection::SetReflectionObject(VirtualObj *pReflectionObject) {
	RESULT r = R_PASS;

	quad *pQuad = dynamic_cast<quad*>(pReflectionObject);
	CNM(pQuad, "Object not supported for reflection");

	m_pReflectionObject = pQuad;

Error:
	return r;
}

RESULT OGLProgramReflection::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	// Trick to only render at reduced frame rate
	//if (frameID % 10 != 0) {
	//	return R_SKIPPED;
	//}

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr)
		BindToFramebuffer(m_pOGLFramebuffer);

	//glEnable(GL_BLEND);
	
	//glFrontFace(GL_CW);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	//glFrontFace(GL_CCW);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramReflection::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	// Bump
	//SetTextureUniform(pOGLObj->GetOGLTextureBump(), m_pUniformTextureBump, m_pUniformHasTextureBump, 0);

	// Material textures
	//SetTextureUniform(pOGLObj->GetOGLTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient, 2);
	
	if (pOGLObj->GetOGLTextureDiffuse() != nullptr) {
		SetTextureUniform(pOGLObj->GetOGLTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse, 0);
	}

	//SetTextureUniform(pOGLObj->GetOGLTextureSpecular(), m_pUniformTextureSpecular, m_pUniformHasTextureSpecular, 4);

	// bump texture
	// TODO: add bump texture to shader
	//m_pUniformHasTextureBump->SetUniform(pOGLObj->GetOGLTextureBump() != nullptr);

	//	Error:
	return r;
}

RESULT OGLProgramReflection::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramReflection::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramReflection::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();
		
		vector vReflectionPlane = reflectionPlane.GetNormal();
		vReflectionPlane.w() = reflectionPlane.GetDValue();
	
		if (m_pUniformClippingPlane != nullptr)
			m_pUniformClippingPlane->SetUniform(vReflectionPlane);
	}

	return R_PASS;
}

RESULT OGLProgramReflection::SetCameraUniforms(camera *pCamera) {
	RESULT r = R_PASS;

	//auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();		
		
		// Try to eliminate edge artifacts 
		point ptReflectionPlanePosition = reflectionPlane.GetPosition();
		ptReflectionPlanePosition.y() += 0.1f;
		reflectionPlane.SetPlanePosition(ptReflectionPlanePosition);

		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));
	
		matV = matFlip * matV * matReflection;
	}

	auto matVP = matP * matV;

	if(m_pUniformViewMatrix != nullptr)
		m_pUniformViewMatrix->SetUniform(matV);

	if(m_pUniformProjectionMatrix != nullptr)
		m_pUniformProjectionMatrix->SetUniform(matP);

	if(m_pUniformViewProjectionMatrix != nullptr)
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return r;
}

RESULT OGLProgramReflection::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	RESULT r = R_PASS;

	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	//auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();

		// Try to eliminate edge artifacts 
		point ptReflectionPlanePosition = reflectionPlane.GetPosition();
		ptReflectionPlanePosition.y() += 0.1f;
		reflectionPlane.SetPlanePosition(ptReflectionPlanePosition);
	
		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));
	
		matV = matFlip * matV * matReflection;
	}

	auto matVP = matP * matV;

	if (m_pUniformViewMatrix != nullptr)
		m_pUniformViewMatrix->SetUniform(matV);

	if (m_pUniformProjectionMatrix != nullptr)
		m_pUniformProjectionMatrix->SetUniform(matP);

	if (m_pUniformViewProjectionMatrix != nullptr)
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

	return r;
}

RESULT OGLProgramReflection::SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform, int texUnit) {
	RESULT r = R_PASS;
	
	if (pTexture && pTextureUniform != nullptr) {
		if(pBoolUniform != nullptr)
			pBoolUniform->SetUniform(true);

		m_pParentImp->glActiveTexture(GL_TEXTURE0 + texUnit);
		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());

		pTextureUniform->SetUniform(texUnit);
	}
	else {
		if (pBoolUniform != nullptr)
			pBoolUniform->SetUniform(false);
	}

//Error:
	return r;
}