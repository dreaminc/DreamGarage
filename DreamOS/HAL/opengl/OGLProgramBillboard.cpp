#include "OGLProgramBillboard.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "HAL/opengl/OpenGLImp.h"
#include "HAL/opengl/OGLFramebuffer.h"

#include "OGLAttachment.h"

OGLProgramBillboard::OGLProgramBillboard(OpenGLImp *pParentImp, PIPELINE_FLAGS optFlags) :
	OGLProgramMinimal(pParentImp, optFlags)
{
	// empty
}

RESULT OGLProgramBillboard::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewWidth), std::string("u_width")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewHeight), std::string("u_height")));

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

RESULT OGLProgramBillboard::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");
	CRM(AddSharedShaderFilename(L"materialCommon.shader"), "Failed to add shared vertex shader code");

	// Vertex
	CRM(MakeVertexShader(L"billboard.vert"), "Failed to create vertex shader");

	// Geometry
	CRM(MakeGeometryShader(L"billboard.geo"), "Failed to create geometry shader");

	// Fragment
	CRM(MakeFragmentShader(L"billboard.frag"), "Failed to create vertex shader");

	CRM(LinkProgram(), "Failed to link program");

	WCR(GetVertexAttributesFromProgram());
	WCR(BindAttributes());

	CR(GetUniformVariablesFromProgram());

	CR(GetUniformBlocksFromProgram());
	CR(BindUniformBlocks());

	// TODO:  Currently using a global material 
	SetMaterial(&material(60.0f, 1.0f, color(COLOR_WHITE), color(COLOR_WHITE), color(COLOR_WHITE)));

Error:
	return r;
}

RESULT OGLProgramBillboard::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		if (IsPassthru()) {
			m_pOGLFramebuffer->Bind();
		}
		else {
			BindToFramebuffer(m_pOGLFramebuffer);
		}
	}

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	RenderObjectStore(m_pSceneGraph);

	UnbindFramebuffer();

Error:
	return r;
}

RESULT OGLProgramBillboard::SetObjectUniforms(DimObj *pDimObj) {

	if (m_pUniformModelMatrix != nullptr) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}

	return R_PASS;
}
