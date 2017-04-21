#include "OGLProgramSkyboxScatter.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

OGLProgramSkyboxScatter::OGLProgramSkyboxScatter(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglskyboxscatter")
{
	// empty
}

RESULT OGLProgramSkyboxScatter::OGLInitialize() {
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


Error:
	return r;
}

RESULT OGLProgramSkyboxScatter::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	//TODO: CR(MakeInput("lights"));

	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<ObjectStore>("scenegraph", &m_pSceneGraph, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));

	// Outputs
	CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));

Error:
	return r;
}

RESULT OGLProgramSkyboxScatter::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	ObjectStoreImp *pObjectStore = m_pSceneGraph->GetSceneGraphStore();

	std::vector<light*> *pLights = nullptr;
	pObjectStore->GetLights(pLights);

	skybox *pSkybox = nullptr;
	CR(pObjectStore->GetSkybox(pSkybox));

	if (pSkybox == nullptr)
		return r;

	UseProgram();
	SetLights(pLights);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	// 3D Object / skybox
	//RenderObjectStore(m_pSceneGraph);
	CR(RenderObject(pSkybox));

Error:
	return r;
}

RESULT OGLProgramSkyboxScatter::SetObjectTextures(OGLObj *pOGLObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLProgramSkyboxScatter::SetObjectUniforms(DimObj *pDimObj) {
	auto matModel = pDimObj->GetModelMatrix();
	m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramSkyboxScatter::SetCameraUniforms(camera *pCamera) {

	auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	auto matVP = matP * matV;
	auto matVO = pCamera->GetOrientationMatrix();

	auto pxWidth = pCamera->GetViewWidth();
	auto pxHeight = pCamera->GetViewHeight();

	vector sunDirection = vector(0.0f, m_SunY, 0.5f);
	sunDirection.Normalize();
	//sunY += 0.01f;
	m_theta += m_delta;
	sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, m_theta) * sunDirection;
	sunDirection.Normalize();

	m_pUniformSunDirection->SetUniform(sunDirection);
	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	m_pUniformViewOrientationMatrix->SetUniform(matVO);
	m_pUniformViewWidth->SetUniformInteger(pxWidth);
	m_pUniformViewHeight->SetUniformInteger(pxHeight);

	return R_PASS;
}

RESULT OGLProgramSkyboxScatter::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {

	auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	auto matVP = matP * matV;
	auto matVO = pStereoCamera->GetOrientationMatrix();

	auto pxWidth = (pStereoCamera->GetViewWidth());
	auto pxHeight = (pStereoCamera->GetViewHeight());

	/*
	point sunDirection = point(0.3f, sunY, -0.5f);
	sunY += 0.0002f;
	DEBUG_OUT("%f\n", sunY);
	*/

	vector sunDirection = vector(0.0f, m_SunY, -0.5f);
	sunDirection.Normalize();
	//sunY += 0.01f;
	m_theta += m_delta;
	sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, m_theta) * sunDirection;
	sunDirection.Normalize();

	m_pUniformSunDirection->SetUniform(sunDirection);

	m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	m_pUniformViewOrientationMatrix->SetUniform(matVO);

	m_pUniformViewWidth->SetUniformInteger(pxWidth);
	m_pUniformViewHeight->SetUniformInteger(pxHeight);

	return R_PASS;
}