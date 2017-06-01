#include "OGLProgramShadowDepth.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"
#include "OGLTexture.h"

OGLProgramShadowDepth::OGLProgramShadowDepth(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglshadowdepth"),
	m_pShadowEmitter(nullptr)
{
	// empty
}

RESULT OGLProgramShadowDepth::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	// Create this as a pure depth map texture
	// TODO: Remove this path?
	//CR(InitializeDepthToTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT));

	m_pOGLFramebuffer = new OGLFramebuffer(m_pParentImp, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 1);
	CN(m_pOGLFramebuffer);

	CR(m_pOGLFramebuffer->OGLInitialize());
	CR(m_pOGLFramebuffer->Bind());

	CR(m_pOGLFramebuffer->SetSampleCount(4));

	CR(m_pOGLFramebuffer->MakeDepthAttachment());		// Note: This will create a new depth buffer
	CR(m_pOGLFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(GL_DEPTH_COMPONENT32, GL_FLOAT));
	//CR(m_pOGLFramebuffer->GetDepthAttachment()->MakeOGLDepthTexture(GL_DEPTH_COMPONENT16, GL_FLOAT));
	
	// Use linear filtering
	CR(m_pOGLFramebuffer->GetDepthAttachment()->GetOGLTexture()->SetTextureParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	CR(m_pOGLFramebuffer->GetDepthAttachment()->GetOGLTexture()->SetTextureParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR));

	// Reference Compare
	CR(m_pOGLFramebuffer->GetDepthAttachment()->GetOGLTexture()->SetTextureParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
	CR(m_pOGLFramebuffer->GetDepthAttachment()->GetOGLTexture()->SetTextureParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS));

	CR(m_pOGLFramebuffer->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));

Error:
	return r;
}

RESULT OGLProgramShadowDepth::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));

	// Outputs
	// TODO: Handle more than one light for shadows
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

/*
m_pOGLProgramShadowDepth->UseProgram();
m_pOGLProgramShadowDepth->BindToDepthBuffer();
CR(m_pOGLProgramShadowDepth->SetCamera(m_pCamera));
CR(m_pOGLProgramShadowDepth->SetLights(pLights));
CR(m_pOGLProgramShadowDepth->RenderSceneGraph(pSceneGraph));
m_pOGLProgramShadowDepth->UnbindFramebuffer();
*/

RESULT OGLProgramShadowDepth::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	// Trick to only render at reduced frame rate
	if (frameID % 3 != 0) {
		return R_SKIPPED;
	}

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		BindToFramebuffer(m_pOGLFramebuffer);
	}

	SetLights(pLights);

	//SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());
	if (m_pShadowEmitter != nullptr) {
		m_pUniformViewProjectionMatrix->SetUniform(GetViewProjectionMatrix());
	}

	// We render the back faces to avoid artifacts 
	
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	//glDisable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glPolygonOffset(1.0f, 1.0f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDisable(GL_POLYGON_OFFSET_FILL);

	UnbindFramebuffer();

	//Error:
	return r;
}

RESULT OGLProgramShadowDepth::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	// TODO: This will use the first light that has shadows enabled
	for (auto it = pLights->begin(); it != pLights->end(); it++) {
		if ((*it)->IsShadowEmitter()) {
			m_pShadowEmitter = (*it);
		}
	}

	//	Error:
	return r;
}

RESULT OGLProgramShadowDepth::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramShadowDepth::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

vector OGLProgramShadowDepth::GetShadowEmitterDirection() {
	vector vShadowEmitterDirection;

	if (m_pShadowEmitter != nullptr) {
		vShadowEmitterDirection = m_pShadowEmitter->GetLightDirection();
	}

	return vShadowEmitterDirection;
}

// TODO: Determine scene size from scene graph
matrix<virtual_precision, 4, 4> OGLProgramShadowDepth::GetViewProjectionMatrix() {
	matrix<virtual_precision, 4, 4> matVP;
	matVP.identity();

	point ptSceneMax, ptSceneMin, ptSceneMid;

	m_pSceneGraph->GetMinMaxPoint(&ptSceneMax, &ptSceneMin);
	ptSceneMid = point::midpoint(ptSceneMax, ptSceneMin);

	if (m_pShadowEmitter != nullptr) {
		matVP = m_pShadowEmitter->GetViewProjectionMatrix(80.0f, 80.0f, 0.1f, 1000.0f);
	}

	return matVP;
}

RESULT OGLProgramShadowDepth::SetCameraUniforms(camera *pCamera) {
	return m_pUniformViewProjectionMatrix->SetUniform(GetViewProjectionMatrix());
}

RESULT OGLProgramShadowDepth::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	return m_pUniformViewProjectionMatrix->SetUniform(GetViewProjectionMatrix());
}

RESULT OGLProgramShadowDepth::SetShadowCastingLightSource(light *pLight) {
	m_pShadowEmitter = pLight;
	return R_PASS;
}

light* OGLProgramShadowDepth::pGetShadowCastingLight() {
	return m_pShadowEmitter;
}