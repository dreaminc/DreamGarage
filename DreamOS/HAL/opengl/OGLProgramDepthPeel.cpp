#include "OGLProgramDepthPeel.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLTexture.h"
#include "OGLQuery.h"

#include "HAL/opengl/GL/glext.h"

#include "OGLProgramBlendQuad.h"

//OGLTexture *g_pColorTexture = nullptr;

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

	//InitializeFrameBuffer(m_pOGLFramebufferOutputA, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	m_pOGLFramebufferOutputA = new OGLFramebuffer(m_pParentImp, 1024, 1024, 4);
	CN(m_pOGLFramebufferOutputA);

	CR(m_pOGLFramebufferOutputA->OGLInitialize());
	CR(m_pOGLFramebufferOutputA->Bind());

	// Color attachment
	CR(m_pOGLFramebufferOutputA->MakeColorAttachment());
	CR(m_pOGLFramebufferOutputA->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR));
	CR(m_pOGLFramebufferOutputA->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	// Depth attachment 
	CR(m_pOGLFramebufferOutputA->MakeDepthAttachment());
	CR(m_pOGLFramebufferOutputA->GetDepthAttachment()->MakeOGLDepthTexture(GL_DEPTH_COMPONENT32F, GL_FLOAT, texture::TEXTURE_TYPE::TEXTURE_RECTANGLE));
	CR(m_pOGLFramebufferOutputA->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(m_pOGLFramebufferOutputA->InitializeOGLDrawBuffers(1));

	//InitializeFrameBuffer(m_pOGLFramebufferOutputB, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	m_pOGLFramebufferOutputB = new OGLFramebuffer(m_pParentImp, 1024, 1024, 4);
	CN(m_pOGLFramebufferOutputB);

	CR(m_pOGLFramebufferOutputB->OGLInitialize());
	CR(m_pOGLFramebufferOutputB->Bind());

	// Color attachment
	CR(m_pOGLFramebufferOutputB->MakeColorAttachment());
	CR(m_pOGLFramebufferOutputB->GetColorAttachment()->MakeOGLTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR));
	CR(m_pOGLFramebufferOutputB->GetColorAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	// Depth attachment 
	CR(m_pOGLFramebufferOutputB->MakeDepthAttachment());
	CR(m_pOGLFramebufferOutputB->GetDepthAttachment()->MakeOGLDepthTexture(GL_DEPTH_COMPONENT32F, GL_FLOAT, texture::TEXTURE_TYPE::TEXTURE_RECTANGLE));
	CR(m_pOGLFramebufferOutputB->GetDepthAttachment()->AttachTextureToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT));

	CR(m_pOGLFramebufferOutputB->InitializeOGLDrawBuffers(1));

	//InitializeFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024, 4);

	//g_pColorTexture = (OGLTexture *)m_pParentImp->MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_RECTANGLE);

	m_pOGLQuery = new OGLQuery(m_pParentImp);
	CN(m_pOGLQuery);
	CR(m_pOGLQuery->OGLInitialize());

	// Blend program
	CR(InitializeBlendQuadProgram());

	// Check that our framebuffer is OK
	CR(m_pParentImp->CheckFramebufferStatus(GL_FRAMEBUFFER));

Error:
	return r;
}

RESULT OGLProgramDepthPeel::InitializeBlendQuadProgram() {
	RESULT r = R_PASS;

	m_pOGLProgramBlendQuad = new OGLProgramBlendQuad(m_pParentImp);
	CNM(m_pOGLProgramBlendQuad, "Failed to allocate OGLProgram");

	CRM(m_pOGLProgramBlendQuad->OGLProgram::OGLInitialize(L"blendquad.vert", L"blendquad.frag", m_pParentImp->GetOGLVersion()),
		"Failed to initialize OGL blend quad Program");

	// TODO: Move this somewhere better
	CR(m_pOGLProgramBlendQuad->SetupConnections());

Error:
	return r;
}

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
	CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));	

	// Connect output as pass-thru to internal blend program
	CR(SetOutputPassthru<OGLFramebuffer>("output_framebuffer", m_pOGLProgramBlendQuad->Output("output_framebuffer")));

Error:
	return r;
}

RESULT OGLProgramDepthPeel::PreProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	// Update buffers as needed
	if (m_depth == 0) {
		//UpdateFramebufferToViewport(m_pOGLFramebufferOutputA, GL_DEPTH_COMPONENT16, GL_FLOAT);
		//UpdateFramebufferToViewport(m_pOGLFramebufferOutputB, GL_DEPTH_COMPONENT16, GL_FLOAT);	

		//UpdateFramebufferToViewport(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT);
	}

	// Alternate buffers 
	OGLFramebuffer *pOGLFramebufferInput = ((m_depth % 2) == 0) ? m_pOGLFramebufferInputA : m_pOGLFramebufferInputB;
	OGLFramebuffer *pOGLFramebufferOutput = ((m_depth % 2) == 0) ? m_pOGLFramebufferOutputB : m_pOGLFramebufferOutputA;

	UseProgram();
	
	if (pOGLFramebufferOutput != nullptr) {
		BindToFramebuffer(pOGLFramebufferOutput);
	}

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	if (m_depth == 0) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		// Disable blending and depth testing
		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetLights(pLights);
	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	if (pOGLFramebufferInput != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);
		m_pParentImp->BindTexture(pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureTarget(), pOGLFramebufferInput->GetDepthAttachment()->GetOGLTextureIndex());
		//m_pParentImp->BindTexture(g_pColorTexture->GetOGLTextureTarget(), g_pColorTexture->GetOGLTextureIndex());
		
		m_pUniformTextureDepth->SetUniform(0);
	}

	m_pOGLQuery->BeginQuery(GL_SAMPLES_PASSED_ARB);

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	m_pOGLQuery->EndQuery(GL_SAMPLES_PASSED_ARB);

	// TODO: Blending here

	GLuint samples;
	m_pOGLQuery->GetQueryObject(&samples);

	//if (samples == 0 || m_depth == 1 ) {
	if (samples == 0 || m_depth >= MAX_DEPTH_PEEL_LAYERS) {
		// TODO: This might not be the best way to do this
		// we kind of want a "stack frame" object potentially 
		m_depth = 0;
		Terminate();

		/*
		m_pOGLFramebufferOutputA->SetAndClearViewport(false, true);
		m_pOGLFramebufferOutputB->SetAndClearViewport(false, true);
		//*/
	}
	else {
		// Blend the texture here

		if (m_pOGLProgramBlendQuad != nullptr) {
			//CR(m_pOGLProgramBlendQuad->ProcessNode(frameID));
			m_pOGLProgramBlendQuad->SetInput<OGLFramebuffer>("input_framebuffer", pOGLFramebufferOutput);
			m_pOGLProgramBlendQuad->ProcessNode(frameID);
		}

		m_depth++;
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