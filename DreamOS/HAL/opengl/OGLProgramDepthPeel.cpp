#include "OGLProgramDepthPeel.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLQuery.h"

#include "HAL/opengl/GL/glext.h"

OGLProgramDepthPeel::OGLProgramDepthPeel(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "ogldepthpeel")
{
	// empty
}

RESULT OGLProgramDepthPeel::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDepth), std::string("u_textureDepth")));

	UpdateFramebufferToViewport(m_pOGLFramebufferOutputA, GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToViewport(m_pOGLFramebufferOutputB, GL_DEPTH_COMPONENT16, GL_FLOAT);

	m_pOGLQuery = new OGLQuery(m_pParentImp);
	CN(m_pOGLQuery);
	CR(m_pOGLQuery->OGLInitialize());

Error:
	return r;
}


OGLFramebuffer *m_pOGLFramebufferInputA = nullptr;
OGLFramebuffer *m_pOGLFramebufferInputB = nullptr;

OGLFramebuffer *m_pOGLFramebufferOutputA = nullptr;
OGLFramebuffer *m_pOGLFramebufferOutputB = nullptr;

int m_numSamplesProcessed;
int *m_pNumSamplesProcessedLastPass = nullptr;

int m_depth;
int *m_pLastDepth = nullptr;

RESULT OGLProgramDepthPeel::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	//TODO: CR(MakeInput("lights"));

	CR(MakeInput<OGLFramebuffer>("input_framebufferA", &m_pOGLFramebufferInputA));
	CR(MakeInput<OGLFramebuffer>("input_framebufferB", &m_pOGLFramebufferInputB));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebufferA", m_pOGLFramebufferOutputA));
	CR(MakeOutput<OGLFramebuffer>("output_framebufferB", m_pOGLFramebufferOutputB));

	// The render output
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

	// Update buffers as needed
	UpdateFramebufferToViewport(m_pOGLFramebufferOutputA, GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToViewport(m_pOGLFramebufferOutputB, GL_DEPTH_COMPONENT16, GL_FLOAT);

Error:
	return r;
}

RESULT OGLProgramDepthPeel::PreProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	// Update buffers as needed
	UpdateFramebufferToViewport(m_pOGLFramebufferOutputA, GL_DEPTH_COMPONENT16, GL_FLOAT);
	UpdateFramebufferToViewport(m_pOGLFramebufferOutputB, GL_DEPTH_COMPONENT16, GL_FLOAT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr)
		BindToFramebuffer(m_pOGLFramebuffer);

	SetLights(pLights);
	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	OGLFramebuffer *pOGLFramebufferInput = ((m_depth % 2) == 0) ? m_pOGLFramebufferInputA : m_pOGLFramebufferInputB;

	if (pOGLFramebufferInput != nullptr) {
		OGLTexture *pTexture = (OGLTexture*)(pOGLFramebufferInput->GetTexture());

		pTexture->OGLActivateTexture();
		m_pUniformTextureDepth->SetUniform(pTexture);
	}

	m_pOGLQuery->BeginQuery(GL_SAMPLES_PASSED_ARB);

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	m_pOGLQuery->EndQuery(GL_SAMPLES_PASSED_ARB);

	GLuint samples;
	m_pOGLQuery->GetQueryObject(&samples);
	if (samples == 0) {
		Terminate();
	}

	UnbindFramebuffer();

//Error:
	return r;
}

RESULT OGLProgramDepthPeel::ProcessNode(long frameID) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramDepthPeel::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramDepthPeel::SetObjectUniforms(DimObj *pDimObj) {
	if (m_pUniformModelMatrix != nullptr) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}

	return R_PASS;
}

RESULT OGLProgramDepthPeel::SetCameraUniforms(camera *pCamera) {
	if (m_pUniformViewProjectionMatrix != nullptr) {
		auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
		//auto matVP = pCamera->GetProjectionMatrix();
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	return R_PASS;
}

RESULT OGLProgramDepthPeel::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	if (m_pUniformViewProjectionMatrix != nullptr) {
		auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);
		//auto matVP = pStereoCamera->GetProjectionMatrix(eye);
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	return R_PASS;
}