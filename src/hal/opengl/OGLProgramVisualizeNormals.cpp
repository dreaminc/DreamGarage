#include "OGLProgramVisualizeNormals.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLFramebuffer.h"

#include "OGLAttachment.h"

OGLProgramVisualizeNormals::OGLProgramVisualizeNormals(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgramMinimal(pParentImp, optFlags)
{
	// empty
}

RESULT OGLProgramVisualizeNormals::OGLInitialize() {
	RESULT r = R_PASS;

	// Create and initialize the base program
	CR(OGLProgram::OGLInitialize());

	// Set up our attributes and blocks

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeNormal), std::string("inV_vec4Normal")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));

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

		CR(m_pOGLFramebuffer->InitializeOGLDrawBuffers(1));
	}

Error:
	return r;
}

RESULT OGLProgramVisualizeNormals::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");

	// Vertex
	CRM(MakeVertexShader(L"visualize_normals.vert"), "Failed to create vertex shader");
	//CRM(MakeVertexShader(L"minimal.vert"), "Failed to create vertex shader");

	// Geometry
	CRM(MakeGeometryShader(L"visualize_normals.geo"), "Failed to create geometry shader");

	// Fragment
	CRM(MakeFragmentShader(L"visualize_normals.frag"), "Failed to create fragment shader");
	//CRM(MakeFragmentShader(L"minimal.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramVisualizeNormals::SetupConnections() {
	RESULT r = R_PASS;

	//// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, PIPELINE_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, PIPELINE_FLAGS::PASSIVE));
	
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

RESULT OGLProgramVisualizeNormals::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	if (m_pParentImp->IsRenderReferenceGeometry() == false) {
		return R_SKIPPED;
	}

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		if (IsPassthru()) {
			m_pOGLFramebuffer->Bind();
		}
		else {
			BindToFramebuffer(m_pOGLFramebuffer);
		}
	}

	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

//Error:
	return r;
}

RESULT OGLProgramVisualizeNormals::SetObjectUniforms(DimObj *pDimObj) {
	if (m_pUniformModelMatrix != nullptr) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}

	return R_PASS;
}

RESULT OGLProgramVisualizeNormals::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

	if (m_pUniformViewProjectionMatrix != nullptr) {
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	if (m_pUniformProjectionMatrix != nullptr) {
		m_pUniformProjectionMatrix->SetUniform(matP);
	}

	if (m_pUniformViewMatrix != nullptr) {
		m_pUniformViewMatrix->SetUniform(matV);
	}

	return R_PASS;
}

RESULT OGLProgramVisualizeNormals::SetCameraUniforms(camera *pCamera) {
	auto matP = pCamera->GetProjectionMatrix();
	auto matV = pCamera->GetViewMatrix();
	auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

	if (m_pUniformViewProjectionMatrix != nullptr) {
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
	}

	if (m_pUniformProjectionMatrix != nullptr) {
		m_pUniformProjectionMatrix->SetUniform(matP);
	}

	if (m_pUniformViewMatrix != nullptr) {
		m_pUniformViewMatrix->SetUniform(matV);
	}

	return R_PASS;
}