#include "OGLProgramEnvironmentObjects.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

OGLProgramEnvironmentObjects::OGLProgramEnvironmentObjects(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglenvironment"),
	m_pLightsBlock(nullptr),
	m_pMaterialsBlock(nullptr)
{
	// empty
}

RESULT OGLProgramEnvironmentObjects::OGLInitialize() {
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

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTime), std::string("u_time")));

	// Uniform Blocks
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	m_deltaTime = 0.0f;
	m_startTime = std::chrono::high_resolution_clock::now();

Error:
	return r;
}

RESULT OGLProgramEnvironmentObjects::SetupConnections() {
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

RESULT OGLProgramEnvironmentObjects::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	// For now
	EYE_TYPE eye = EYE_MONO;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	UseProgram();
	SetLights(pLights);

	/*
	// Camera Projection Matrix
	if (m_pHMD != nullptr) {
	m_pCamera->ResizeCamera(m_pHMD->GetEyeWidth(), m_pHMD->GetEyeHeight());
	UseProgram();
	}
	*/

	SetStereoCamera(m_pCamera, eye);

	/*
	if (m_pHMD != nullptr) {
	m_pHMD->SetAndClearRenderSurface(eye);
	}
	else if (eye != lastEye) {
	SetViewTarget(eye);
	lastEye = eye;
	}
	*/

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	//Error:
	return r;
}

RESULT OGLProgramEnvironmentObjects::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	// color texture
	SetTextureUniform(pOGLObj->GetColorTexture(), m_pUniformTextureColor, m_pUniformHasTextureColor);

	// material textures
	SetTextureUniform(pOGLObj->GetTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient);
	SetTextureUniform(pOGLObj->GetTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse);
	SetTextureUniform(pOGLObj->GetTextureSpecular(), m_pUniformTextureSpecular, m_pUniformHasTextureSpecular);

	// bump texture
	// TODO: add bump texture to shader
	m_pUniformHasTextureBump->SetUniform(pOGLObj->GetBumpTexture() != nullptr);

	//	Error:
	return r;
}

RESULT OGLProgramEnvironmentObjects::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	if (m_pLightsBlock != nullptr) {
		CR(m_pLightsBlock->SetLights(pLights));
		CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramEnvironmentObjects::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	if (m_pMaterialsBlock != nullptr) {
		CR(m_pMaterialsBlock->SetMaterial(pMaterial));
		CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
	}

Error:
	return r;
}

RESULT OGLProgramEnvironmentObjects::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramEnvironmentObjects::SetCameraUniforms(camera *pCamera) {

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

RESULT OGLProgramEnvironmentObjects::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {

	auto deltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_startTime).count();
	m_deltaTime = (float)deltaTime;
	m_deltaTime *= 0.5f;
	m_pUniformTime->SetUniformFloat(reinterpret_cast<GLfloat*>(&m_deltaTime));

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

RESULT OGLProgramEnvironmentObjects::SetRiverAnimation(bool fRiverAnimation) {
	m_pUniformRiverAnimation->SetUniform(fRiverAnimation);

	return R_PASS;
}

void OGLProgramEnvironmentObjects::SetTextureUniform(OGLTexture* pTexture, OGLUniformSampler2D* pTextureUniform, OGLUniformBool* pBoolUniform) {
	if (pTexture) {
		pBoolUniform->SetUniform(true);
		pTexture->OGLActivateTexture();
		pTextureUniform->SetUniform(pTexture);
	}
	else {
		pBoolUniform->SetUniform(false);
	}
};