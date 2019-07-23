#include "OGLProgramReferenceGeometry.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLFramebuffer.h"

OGLProgramReferenceGeometry::OGLProgramReferenceGeometry(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgramMinimal(pParentImp, optFlags)
{
	// empty
}

RESULT OGLProgramReferenceGeometry::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

	CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

Error:
	return r;
}

RESULT OGLProgramReferenceGeometry::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));

	// The render output is passed through
	// TODO: Flag?
	CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramReferenceGeometry::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	if (m_pParentImp->IsRenderReferenceGeometry() == false) {
		return R_SKIPPED;
	}

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		//BindToFramebuffer(m_pOGLFramebuffer);
		m_pOGLFramebuffer->Bind();
	}

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// Reference geometry
	RenderObjectStoreBoundingVolumes(m_pSceneGraph);

	UnbindFramebuffer();

//Error:
	return r;
}