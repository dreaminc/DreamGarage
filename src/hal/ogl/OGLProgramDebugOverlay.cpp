#include "OGLProgramDebugOverlay.h"

#include "scene/ObjectStoreImp.h"
#include "scene/ObjectStore.h"

#include "core/camera/stereocamera.h"

#include "OGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

OGLProgramDebugOverlay::OGLProgramDebugOverlay(OGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgramMinimal(pParentImp, "ogldebugoverlay", optFlags)
{
	// empty
}

RESULT OGLProgramDebugOverlay::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	//InitializeFrameBuffer(GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeFrameBufferWithDepth(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeDepthFrameBuffer(m_pOGLFramebuffer, GL_DEPTH_COMPONENT16, GL_FLOAT);
	//InitializeDepthToTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, 1024, 1024);

	// Custom framebuffer output settings
	//CR(InitializeFrameBuffer(GL_DEPTH_COMPONENT24, GL_INT));

	/*
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

	CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));
	//*/

Error:
	return r;
}

RESULT OGLProgramDebugOverlay::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));

	// Outputs
	CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramDebugOverlay::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToViewport(GL_DEPTH_COMPONENT24, GL_INT);
	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	glDisable(GL_DEPTH_TEST);

	if (m_pOGLFramebuffer != nullptr) {
		//BindToFramebuffer(m_pOGLFramebuffer);
		m_pOGLFramebuffer->Bind();
	}
	
	glEnable(GL_BLEND);

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	RenderObjectStore(m_pSceneGraph);

	glEnable(GL_DEPTH_TEST);

	UnbindFramebuffer();

//Error:
	return r;
}
