#include "OGLProgramCubemapConvolution.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"
#include "OGLCubemap.h"

OGLProgramCubemapConvolution::OGLProgramCubemapConvolution(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglskybox")
{
	// empty
}

RESULT OGLProgramCubemapConvolution::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
	//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewOrientationMatrix), std::string("u_mat4ViewOrientation")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureCubemap), std::string("u_hasTextureCubemap")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureCubemap), std::string("u_textureCubeMap")));

	///*
	// Cubemap Framebuffer Output
	int pxWidth = 1024;
	int pxHeight = 1024;

	m_pOGLFramebufferCubemap = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CN(m_pOGLFramebufferCubemap);

	CR(m_pOGLFramebufferCubemap->OGLInitialize());
	//CR(m_pOGLFramebufferCubemap->Bind());

	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_pOGLFramebufferCubemap->GetFramebufferIndex()));

	CR(m_pOGLFramebufferCubemap->SetSampleCount(1));

	CR(m_pOGLFramebufferCubemap->MakeDepthAttachment());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->OGLInitializeRenderBuffer());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));

	CR(m_pOGLFramebufferCubemap->MakeColorAttachment());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->MakeOGLCubemap());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->AttachCubemapToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(m_pOGLFramebufferCubemap->InitializeOGLDrawBuffers(1));

	CRM(m_pOGLFramebufferCubemap->CheckStatus(), "Frame buffer messed up");

	// Create skybox volume
	m_pSkybox = m_pParentImp->MakeSkybox();
	CN(m_pSkybox);

Error:
	return r;
}

RESULT OGLProgramCubemapConvolution::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"cubemapConvolution.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"cubemapConvolution.frag"), "Failed to create fragment shader");

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

Error:
	return r;
}

RESULT OGLProgramCubemapConvolution::SetCubemap(cubemap *pCubemap) {

	m_pCubemap = pCubemap;

	return R_PASS;
}

RESULT OGLProgramCubemapConvolution::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer_cubemap", &m_pOGLInputFramebufferCubemap, DCONNECTION_FLAGS::PASSIVE));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer_cube", m_pOGLFramebufferCubemap));

Error:
	return r;
}

// TODO: Duplicate code from scatterCube
RESULT OGLProgramCubemapConvolution::SetCameraUniforms(GLenum glCubeMapFace, int pxWidth, int pxHeight) {
	RESULT r = R_PASS;

	matrix<float, 4, 4> mat4View;

	mat4View.identity(1.0f);

	switch (glCubeMapFace) {
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X: {
		mat4View = RotationMatrix(RotationMatrix::Z_AXIS, (float)(M_PI)) * mat4View;
		mat4View = RotationMatrix(RotationMatrix::Y_AXIS, (float)(-M_PI / 2.0f)) * mat4View;
	} break;

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: {
		mat4View = RotationMatrix(RotationMatrix::Z_AXIS, (float)(M_PI)) * mat4View;
		mat4View = RotationMatrix(RotationMatrix::Y_AXIS, (float)(M_PI / 2.0f)) * mat4View;
	} break;

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: {
		mat4View = BasisMatrix(BasisMatrix::NEG_Y);
	} break;

	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: {
		mat4View = BasisMatrix(BasisMatrix::POS_Y);
	} break;

	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: {
		mat4View = RotationMatrix(RotationMatrix::Z_AXIS, (float)(M_PI)) * mat4View;
	} break;

	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: {
		mat4View = RotationMatrix(RotationMatrix::Z_AXIS, (float)(M_PI)) * mat4View;
		mat4View = RotationMatrix(RotationMatrix::Y_AXIS, (float)(M_PI)) * mat4View;
	} break;
	}

	// No translation in orientation
	//m_pUniformViewMatrix->SetUniform(mat4View);
	m_pUniformViewOrientationMatrix->SetUniform(mat4View);

Error:
	return r;
}

RESULT OGLProgramCubemapConvolution::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ProjectionMatrix projMatrix;

	CN(m_pSkybox);

	static bool fRendered = false;
	if (fRendered)
		return r;

	UseProgram();

	int pxWidth = m_pOGLFramebufferCubemap->GetWidth();
	int pxHeight = m_pOGLFramebufferCubemap->GetHeight();

	glViewport(0, 0, pxWidth, pxHeight);
	CR(m_pParentImp->glBindFramebuffer(GL_FRAMEBUFFER, m_pOGLFramebufferCubemap->GetFramebufferIndex()));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projMatrix = ProjectionMatrix(PROJECTION_MATRIX_PERSPECTIVE,
		static_cast<projection_precision>(pxWidth),
		static_cast<projection_precision>(pxHeight),
		static_cast<projection_precision>(DEFAULT_NEAR_PLANE),
		static_cast<projection_precision>(DEFAULT_FAR_PLANE),
		static_cast<projection_precision>(90.0f));
	m_pUniformProjectionMatrix->SetUniform(projMatrix);

	if (m_pSkybox != nullptr) {
		for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {

			GLenum glCubeMapFace = OGLCubemap::GetGLCubeMapEnums(i);

			CR(m_pOGLFramebufferCubemap->SetOGLCubemapToFramebuffer2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glCubeMapFace));

			CR(SetCameraUniforms(glCubeMapFace, pxWidth, pxHeight));

			// Render the skybox with above params
			//pOGLSkybox->Render();
			RenderObject(m_pSkybox);
		}
	}

	fRendered = true;

	UnbindFramebuffer();
	//*/

Error:
	return r;
}

RESULT OGLProgramCubemapConvolution::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLCubemap *pOGLCubemap = dynamic_cast<OGLCubemap*>(m_pCubemap);

	if (m_pUniformTextureCubemap != nullptr) {
		//if (pOGLCubemap != nullptr) {
		//	m_pParentImp->glActiveTexture(GL_TEXTURE0);
		//	m_pParentImp->BindTexture(pOGLCubemap->GetOGLTextureTarget(), pOGLCubemap->GetOGLTextureIndex());
		//
		//	m_pUniformTextureCubemap->SetUniform(0);
		//	m_pUniformHasTextureCubemap->SetUniform(true);
		//}
		
		if (m_pOGLInputFramebufferCubemap != nullptr) {
			m_pParentImp->glActiveTexture(GL_TEXTURE0);
			m_pParentImp->BindTexture(m_pOGLInputFramebufferCubemap->GetColorAttachment()->GetOGLCubemapTarget(), 
									  m_pOGLInputFramebufferCubemap->GetColorAttachment()->GetOGLCubemapIndex());

			//m_pUniformTextureCubemap->SetUniform(0);
			if(m_pUniformHasTextureCubemap != nullptr)
				m_pUniformHasTextureCubemap->SetUniform(true);
		}
		else {
			if (m_pUniformHasTextureCubemap != nullptr)
				m_pUniformHasTextureCubemap->SetUniform(false);
		}
	}

Error:
	return r;
}

RESULT OGLProgramCubemapConvolution::SetObjectUniforms(DimObj *pDimObj) {
	//auto matModel = pDimObj->GetModelMatrix();
	//m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramCubemapConvolution::SetCameraUniforms(camera *pCamera) {

	//auto ptEye = pCamera->GetOrigin();
	//auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	//auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	auto matViewOrientation = pCamera->GetOrientationMatrix();

	//m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	// m_pUniformModelViewMatrix
	//m_pUniformViewProjectionMatrix->SetUniform(matVP);
	m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

	return R_PASS;
}

RESULT OGLProgramCubemapConvolution::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	//auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	//auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
	auto matViewOrientation = pStereoCamera->GetOrientationMatrix();

	//m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	// m_pUniformModelViewMatrix
	//m_pUniformViewProjectionMatrix->SetUniform(matVP);
	m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

	return R_PASS;
}