#include "OGLProgramSkyboxScatterCube.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"
#include "Primitives/skybox.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

#include "OGLSkybox.h"
#include "OGLCubemap.h"

#include "Primitives/matrix/ReflectionMatrix.h"

OGLProgramSkyboxScatterCube::OGLProgramSkyboxScatterCube(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglskyboxscattercube")
{
	// empty
}

RESULT OGLProgramSkyboxScatterCube::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewOrientationMatrix), std::string("u_mat4ViewOrientation")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewWidth), std::string("u_intViewWidth")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewHeight), std::string("u_intViewHeight")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformSunDirection), std::string("u_vecSunDirection")));

	///*
	// Cubemap Framebuffer Output
	int pxWidth = 1024;
	int pxHeight = 1024;

	m_pOGLFramebufferCubemap = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CN(m_pOGLFramebufferCubemap);

	CR(m_pOGLFramebufferCubemap->OGLInitialize());
	//CR(m_pOGLFramebufferCubemap->Bind());

	CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pOGLFramebufferCubemap->GetFramebufferIndex()));

	CR(m_pOGLFramebufferCubemap->SetSampleCount(1));

	CR(m_pOGLFramebufferCubemap->MakeDepthAttachment());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->OGLInitializeRenderBuffer());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));

	CR(m_pOGLFramebufferCubemap->MakeColorAttachment());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->MakeOGLCubemap());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->AttachCubemapToFramebuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	

	CRM(m_pOGLFramebufferCubemap->CheckStatus(), "Frame buffer messed up");

	// Create skybox volume
	m_pSkybox = m_pParentImp->MakeSkybox();
	CN(m_pSkybox);

	//*/

	//// Debugging (this will eventually be rendered to)
	//m_pOutputCubemap = m_pParentImp->MakeCubemap(L"LarnacaCastle");
	//CN(m_pOutputCubemap);

Error:
	return r;
}

RESULT OGLProgramSkyboxScatterCube::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"skyboxScatter.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"skyboxScatter.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramSkyboxScatterCube::SetSunDirection(vector vSunDirection) {
	RESULT r = R_PASS;
	
	m_sunDirection = vSunDirection;

	return R_PASS;
}

RESULT OGLProgramSkyboxScatterCube::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer_cube", m_pOGLFramebufferCubemap));
	//CR(MakeOutput<cubemap>("output_cubemap", m_pOutputCubemap));

Error:
	return r;
}


RESULT OGLProgramSkyboxScatterCube::SetCameraUniforms(GLenum glCubeMapFace, int pxWidth, int pxHeight) {
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
	m_pUniformViewMatrix->SetUniform(mat4View);
	m_pUniformViewOrientationMatrix->SetUniform(mat4View);

Error:
	return r;
}

RESULT OGLProgramSkyboxScatterCube::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ProjectionMatrix projMatrix;

	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	CN(m_pSkybox);

	static bool fRendered = false;

	if (fRendered)
		return r;

	UseProgram();

	//if (m_pOGLFramebufferCubemap != nullptr) {
	//	BindToFramebuffer(m_pOGLFramebufferCubemap);
	//}

	int pxWidth = m_pOGLFramebufferCubemap->GetWidth();
	int pxHeight = m_pOGLFramebufferCubemap->GetHeight();

	glViewport(0, 0, pxWidth, pxHeight);
	CR(m_pParentImp->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pOGLFramebufferCubemap->GetFramebufferIndex()));

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// uniforms for all passes
	m_pUniformSunDirection->SetUniform(m_sunDirection);
	m_pUniformViewWidth->SetUniformInteger(pxWidth);
	m_pUniformViewHeight->SetUniformInteger(pxHeight);

	projMatrix = ProjectionMatrix(PROJECTION_MATRIX_PERSPECTIVE,
		static_cast<projection_precision>(pxWidth),
		static_cast<projection_precision>(pxHeight),
		static_cast<projection_precision>(DEFAULT_NEAR_PLANE),
		static_cast<projection_precision>(DEFAULT_FAR_PLANE),
		static_cast<projection_precision>(90.0f));
	m_pUniformProjectionMatrix->SetUniform(projMatrix);

	//OGLSkybox *pOGLSkybox = dynamic_cast<OGLSkybox*>(m_pSkybox);
	//if (pOGLSkybox != nullptr) {
	if(m_pSkybox != nullptr) {
		for (int i = 0; i < NUM_CUBE_MAP_TEXTURES; i++) {
			
			GLenum glCubeMapFace = OGLCubemap::GetGLCubeMapEnums(i);

			CR(m_pOGLFramebufferCubemap->SetOGLCubemapToFramebuffer2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glCubeMapFace));

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

RESULT OGLProgramSkyboxScatterCube::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramSkyboxScatterCube::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramSkyboxScatterCube::SetCameraUniforms(camera *pCamera) {

	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();	
	auto matVO = pCamera->GetOrientationMatrix();

	auto matVP = matP * matV;

	//auto pxWidth = pCamera->GetViewWidth();
	//auto pxHeight = pCamera->GetViewHeight();

	int pxWidth = m_pOGLFramebuffer->GetWidth();
	int pxHeight = m_pOGLFramebuffer->GetHeight();

	/*
	m_sunDirection = vector(0.0f, m_SunY, 0.5f);
	m_sunDirection.Normalize();
	//sunY += 0.01f;
	m_theta += m_delta;
	m_sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, m_theta) * sunDirection;
	m_sunDirection.Normalize();
	//*/

	m_pUniformSunDirection->SetUniform(m_sunDirection);
	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	m_pUniformViewOrientationMatrix->SetUniform(matVO);
	m_pUniformViewWidth->SetUniformInteger(pxWidth);
	m_pUniformViewHeight->SetUniformInteger(pxHeight);

	return R_PASS;
}

RESULT OGLProgramSkyboxScatterCube::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {

	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVO = pStereoCamera->GetOrientationMatrix();

	//auto pxWidth = (pStereoCamera->GetViewWidth());
	//auto pxHeight = (pStereoCamera->GetViewHeight());

	int pxWidth = m_pOGLFramebuffer->GetWidth();
	int pxHeight = m_pOGLFramebuffer->GetHeight();
	
	/*
	point sunDirection = point(0.3f, sunY, -0.5f);
	sunY += 0.0002f;
	DEBUG_OUT("%f\n", sunY);
	*/

	//auto matVP = matP * matV;

	/*
	vector sunDirection = vector(1.0f, m_SunY, -0.4f);
	sunDirection.Normalize();
	//sunY += 0.01f;
	m_theta += m_delta;
	sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, m_theta) * sunDirection;
	sunDirection.Normalize();
	//*/

	m_pUniformSunDirection->SetUniform(m_sunDirection);

	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	m_pUniformViewOrientationMatrix->SetUniform(matVO);

	m_pUniformViewWidth->SetUniformInteger(pxWidth);
	m_pUniformViewHeight->SetUniformInteger(pxHeight);

	return R_PASS;
}
