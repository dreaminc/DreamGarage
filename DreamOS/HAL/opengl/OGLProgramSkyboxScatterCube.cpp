#include "OGLProgramSkyboxScatterCube.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"

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

	// Cubemap Framebuffer Output
	int pxWidth = 1024;
	int pxHeight = 1024;

	m_pOGLFramebufferCubemap = new OGLFramebuffer(m_pParentImp, pxWidth, pxHeight, 4);
	CN(m_pOGLFramebufferCubemap);

	CR(m_pOGLFramebufferCubemap->OGLInitialize());
	CR(m_pOGLFramebufferCubemap->Bind());

	CR(m_pOGLFramebufferCubemap->SetSampleCount(1));

	CR(m_pOGLFramebufferCubemap->MakeColorAttachment());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->MakeOGLCubemap());
	CR(m_pOGLFramebufferCubemap->GetColorAttachment()->AttachCubemapToFramebuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0));

	CR(m_pOGLFramebufferCubemap->MakeDepthAttachment());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->OGLInitializeRenderBuffer());
	CR(m_pOGLFramebufferCubemap->GetDepthAttachment()->AttachRenderBufferToFramebuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER));

	CRM(m_pOGLFramebufferCubemap->CheckStatus(), "Frame buffer messed up");

	// TODO: We can create the skybox mesh here and pull it out of scene graph / box or whatever

	// Debugging (this will eventually be rendered to)
	m_pOutputCubemap = m_pParentImp->MakeCubemap(L"LarnacaCastle");
	CN(m_pOutputCubemap);

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

RESULT OGLProgramSkyboxScatterCube::SetReflectionObject(VirtualObj *pReflectionObject) {
	RESULT r = R_PASS;

	quad *pQuad = dynamic_cast<quad*>(pReflectionObject);
	CNM(pQuad, "Object not supported for reflection");

	m_pReflectionObject = pQuad;

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
	//TODO: CR(MakeInput("lights"));

	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));

	// Outputs
	//CR(MakeOutput<OGLFramebuffer>("output_framebuffer_cube", m_pOGLFramebufferCubemap));
	CR(MakeOutput<cubemap>("output_cubemap", m_pOutputCubemap));

Error:
	return r;
}

RESULT OGLProgramSkyboxScatterCube::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	//UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	// TODO: Replace with a static volume here
	skybox *pSkybox = nullptr;
	CR(pObjectStore->GetSkybox(pSkybox));

	if (pSkybox == nullptr)
		return r;

	UseProgram();

	if (m_pOGLFramebuffer != nullptr) {
		BindToFramebuffer(m_pOGLFramebuffer);
		//m_pOGLFramebuffer->Bind();	// NOTE: This will simply bind, BindToFramebuffer will clear
	}

	/*
	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	CR(RenderObject(pSkybox));

	UnbindFramebuffer();
	*/

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

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();

		plane householderReflectionPlane = reflectionPlane;
		householderReflectionPlane.SetPlanePosition(point(0.0f, 0.0f, 0.0f));

		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matHouseholderReflection = ReflectionMatrix(householderReflectionPlane);

		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));

		matV = matFlip * matV * matReflection;
		matVO = matFlip * matVO * matHouseholderReflection;
	}

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

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();

		plane householderReflectionPlane = reflectionPlane;
		householderReflectionPlane.SetPlanePosition(point(0.0f, 0.0f, 0.0f));

		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matHouseholderReflection = ReflectionMatrix(householderReflectionPlane);

		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));

		matV = matFlip * matV * matReflection;
		matVO = matFlip * matVO * matHouseholderReflection;
	}

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
