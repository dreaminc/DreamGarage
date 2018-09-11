#include "OGLProgramWater.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

#include "Primitives/matrix/ReflectionMatrix.h"

#include <chrono>

OGLProgramWater::OGLProgramWater(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglwater"),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramWater::OGLInitialize() {
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformEye), std::string("u_vec4Eye")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureReflection), std::string("u_hasTextureReflection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureReflection), std::string("u_textureReflection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureRefraction), std::string("u_hasTextureRefraction")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureRefraction), std::string("u_textureRefraction")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureRefractionDepth), std::string("u_hasTextureRefractionDepth")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureRefractionDepth), std::string("u_textureRefractionDepth")));


	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureNormal), std::string("u_hasTextureNormal")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureNormal), std::string("u_textureNormal")));

	OGLUniformBool *m_pUniformHasTextureNormal = nullptr;
	OGLUniformSampler2D *m_pUniformTextureNormal = nullptr;

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTime), std::string("u_time")));

	// Framebuffer Output
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

	CRM(m_pOGLFramebuffer->CheckStatus(), "Frame buffer messed up");

Error:
	return r;
}

RESULT OGLProgramWater::OGLInitialize(version versionOGL) {
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
	CRM(MakeVertexShader(L"water.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"water.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramWater::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	//CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	// Reflection Map
	CR(MakeInput<OGLFramebuffer>("input_reflection_map", &m_pOGLReflectionFramebuffer_in));

	// Refraction Map
	CR(MakeInput<OGLFramebuffer>("input_refraction_map", &m_pOGLRefractionFramebuffer_in));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

	{
		// Make our own lights
		vector vWaterLightDirection = vector(-1.0f, -0.35f, 0.1f);
		//float lightIntensity = 3.5f;
		float lightIntensity = 2.0f;
		auto pLight = m_pParentImp->MakeLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(vWaterLightDirection));
		CN(pLight);

		m_lights.push_back(pLight);
	}

Error:
	return r;
}

RESULT OGLProgramWater::SetPlaneObject(VirtualObj* pReflectionObject) {
	RESULT r = R_PASS;

	quad *pQuad = dynamic_cast<quad*>(pReflectionObject);
	CNM(pQuad, "Non quad not supported in reflection");

	m_pPlaneObject = pReflectionObject;

Error:
	return r;
}

//RESULT OGLProgramWater::SetReflectionPlane(plane reflectionPlane) {
//	m_reflectionPlane = reflectionPlane;
//	return R_PASS;
//}

RESULT OGLProgramWater::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	//ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();
	//std::vector<light*> *pLights = nullptr;
	//pObjectStore->GetLights(pLights);

	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr)
		BindToFramebuffer(m_pOGLFramebuffer);

	glEnable(GL_BLEND);

	SetLights(&m_lights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	//RenderObjectStore(m_pSceneGraph);

	DimObj *pPlaneObj = dynamic_cast<DimObj*>(m_pPlaneObject);
	if (pPlaneObj != nullptr) {
		//if (m_pOGLReflectionFramebuffer_in != nullptr) {
		//	pPlaneObj->SetDiffuseTexture(m_pOGLReflectionFramebuffer_in->GetColorTexture());
		//}
		//
		//if (m_pOGLRefractionFramebuffer_in != nullptr) {
		//	pPlaneObj->SetDiffuseTexture(m_pOGLRefractionFramebuffer_in->GetColorTexture());
		//}

		if (m_pUniformTime != nullptr) {
			static std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now();
			float msTime = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - timeStart).count();
			m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&msTime));
		}

		RenderObject(pPlaneObj);
	}

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramWater::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	// Bump
	//SetTextureUniform(pOGLObj->GetOGLTextureBump(), m_pUniformTextureBump, m_pUniformHasTextureBump, 0);
	
	//// Color texture
	//SetTextureUniform(pOGLObj->GetOGLTextureDiffuse(), m_pUniformTextureColor, m_pUniformHasTextureColor, 1);
	//
	//// Material textures
	//SetTextureUniform(pOGLObj->GetOGLTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient, 2);
	//SetTextureUniform(pOGLObj->GetOGLTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse, 3);
	//SetTextureUniform(pOGLObj->GetOGLTextureSpecular(), m_pUniformTextureSpecular, m_pUniformHasTextureSpecular, 4);
	//
	//// bump texture
	//// TODO: add bump texture to shader
	//m_pUniformHasTextureBump->SetUniform(pOGLObj->GetOGLTextureBump() != nullptr);

	// Reflection Texture
	if (m_pOGLReflectionFramebuffer_in != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(m_pOGLReflectionFramebuffer_in->GetColorAttachment()->GetOGLTextureTarget(),
			m_pOGLReflectionFramebuffer_in->GetColorAttachment()->GetOGLTextureIndex());
		if(m_pUniformTextureReflection != nullptr)
			m_pUniformTextureReflection->SetUniform(0);

		if(m_pUniformHasTextureReflection != nullptr)
			m_pUniformHasTextureReflection->SetUniform(true);
	}
	else {
		if (m_pUniformHasTextureReflection != nullptr)
			m_pUniformHasTextureReflection->SetUniform(false);
	}

	// Refraction Texture & depth map
	if (m_pOGLRefractionFramebuffer_in != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE1);
		m_pParentImp->BindTexture(m_pOGLRefractionFramebuffer_in->GetColorAttachment()->GetOGLTextureTarget(),
			m_pOGLRefractionFramebuffer_in->GetColorAttachment()->GetOGLTextureIndex());
		
		if(m_pUniformTextureRefraction != nullptr)
			m_pUniformTextureRefraction->SetUniform(1);
		
		if(m_pUniformHasTextureRefraction != nullptr)
			m_pUniformHasTextureRefraction->SetUniform(true);

		// Depth

		m_pParentImp->glActiveTexture(GL_TEXTURE2);
		m_pParentImp->BindTexture(m_pOGLRefractionFramebuffer_in->GetDepthAttachment()->GetOGLTextureTarget(),
			m_pOGLRefractionFramebuffer_in->GetDepthAttachment()->GetOGLTextureIndex());
		
		if (m_pUniformTextureRefractionDepth != nullptr)
			m_pUniformTextureRefractionDepth->SetUniform(2);

		if (m_pUniformHasTextureRefractionDepth != nullptr)
			m_pUniformHasTextureRefractionDepth->SetUniform(true);

	}
	else {
		if (m_pUniformHasTextureRefraction != nullptr)
			m_pUniformHasTextureRefraction->SetUniform(false);

		if (m_pUniformHasTextureRefractionDepth != nullptr)
			m_pUniformHasTextureRefractionDepth->SetUniform(false);
	}

	

	// Normal map
	if (pOGLObj->GetOGLTextureBump() != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE3);
		m_pParentImp->BindTexture(pOGLObj->GetOGLTextureBump()->GetOGLTextureTarget(), 
			pOGLObj->GetOGLTextureBump()->GetOGLTextureIndex());

		if(m_pUniformTextureNormal != nullptr)
			m_pUniformTextureNormal->SetUniform(3);

		if(m_pUniformHasTextureNormal != nullptr)
			m_pUniformHasTextureNormal->SetUniform(true);
	}
	else {
		if(m_pUniformHasTextureNormal != nullptr)
			m_pUniformHasTextureNormal->SetUniform(false);
	}

	//	Error:
	return r;
}

RESULT OGLProgramWater::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramWater::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramWater::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	if (m_pPlaneObject != nullptr) {

		plane reflectionPlane = dynamic_cast<quad*>(m_pPlaneObject)->GetPlane();

		auto matReflection = ReflectionMatrix(reflectionPlane);

		//TODO: Get this to work
		//auto matIdentity = matrix<float, 4, 4>();
		//matIdentity.identity(1.0f);
		//auto matFlip = ReflectionMatrix(plane(plane::type::XZ));
		//auto matR = matFlip * (matReflection - matIdentity);

		if (m_pUniformReflectionMatrix != nullptr) {
			m_pUniformReflectionMatrix->SetUniform(matReflection);
		}

		vector vReflectionPlane = reflectionPlane.GetNormal();
		vReflectionPlane.w() = reflectionPlane.GetDValue();

		if (m_pUniformReflectionPlane != nullptr)
			m_pUniformReflectionPlane->SetUniform(vReflectionPlane);
	}

	return R_PASS;
}

RESULT OGLProgramWater::SetCameraUniforms(camera *pCamera) {

	auto ptEye = pCamera->GetOrigin();
	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	m_pUniformViewMatrix->SetUniform(matV);

	if (m_pUniformProjectionMatrix != nullptr)
		m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix
	m_pUniformViewProjectionMatrix->SetUniform(matVP);
	m_pUniformEye->SetUniform(ptEye);

	return R_PASS;
}

RESULT OGLProgramWater::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	//auto ptEye = pStereoCamera->GetPosition(true);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	m_pUniformViewMatrix->SetUniform(matV);

	if(m_pUniformProjectionMatrix != nullptr)
		m_pUniformProjectionMatrix->SetUniform(matP);
	//m_pUniformModelViewMatrix->SetUniform(matM)
	m_pUniformViewProjectionMatrix->SetUniform(matVP);
	//m_pUniformEye->SetUniform(ptEye);

	return R_PASS;
}