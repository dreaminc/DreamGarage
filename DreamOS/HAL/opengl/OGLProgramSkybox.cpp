#include "OGLProgramSkybox.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"

#include "OpenGLImp.h"
#include "OGLFramebuffer.h"
#include "OGLAttachment.h"
#include "OGLCubemap.h"

#include "Primitives/matrix/ReflectionMatrix.h"

OGLProgramSkybox::OGLProgramSkybox(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglskybox")
{
	// empty
}

RESULT OGLProgramSkybox::OGLInitialize() {
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

	if (m_fPassThru == false) {
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

	// Create skybox volume
	m_pSkybox = m_pParentImp->MakeSkybox();
	CN(m_pSkybox);

Error:
	return r;
}

RESULT OGLProgramSkybox::OGLInitialize(version versionOGL) {
	RESULT r = R_PASS;

	CR(OGLInitialize());

	m_versionOGL = versionOGL;

	// Create and set the shaders

	// Global
	CRM(AddSharedShaderFilename(L"core440.shader"), "Failed to add global shared shader code");

	// Vertex
	CRM(MakeVertexShader(L"skybox.vert"), "Failed to create vertex shader");

	// Fragment
	CRM(MakeFragmentShader(L"skybox.frag"), "Failed to create fragment shader");

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

RESULT OGLProgramSkybox::SetReflectionObject(VirtualObj *pReflectionObject) {
	RESULT r = R_PASS;

	quad *pQuad = dynamic_cast<quad*>(pReflectionObject);
	CNM(pQuad, "Object not supported for reflection");

	m_pReflectionObject = pQuad;

Error:
	return r;
}

RESULT OGLProgramSkybox::SetCubemap(cubemap *pCubemap) {

	m_pCubemap = pCubemap;

	return R_PASS;
}

RESULT OGLProgramSkybox::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<OGLFramebuffer>("input_framebuffer_cubemap", &m_pOGLInputFramebufferCubemap, DCONNECTION_FLAGS::PASSIVE));

	// TODO: Input cube map node

	// Outputs
	if (m_fPassThru == true) {
		CR(MakeInput<OGLFramebuffer>("input_framebuffer", &m_pOGLFramebuffer));
		CR(MakeOutputPassthru<OGLFramebuffer>("output_framebuffer", &m_pOGLFramebuffer));
	}
	else {
		CR(MakeOutput<OGLFramebuffer>("output_framebuffer", m_pOGLFramebuffer));
	}

Error:
	return r;
}

RESULT OGLProgramSkybox::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	UpdateFramebufferToCamera(m_pCamera, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT);

	UseProgram();

	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);

	if (m_pOGLFramebuffer != nullptr) {
		if (m_fPassThru) {
			m_pOGLFramebuffer->Bind();
		}
		else {
			BindToFramebuffer(m_pOGLFramebuffer);
		}
	}

	glEnable(GL_BLEND);

	SetStereoCamera(m_pCamera, m_pCamera->GetCameraEye());

	if (m_pSkybox) {
		RenderObject(m_pSkybox);
	}

	UnbindFramebuffer();

Error:
	return r;
}

RESULT OGLProgramSkybox::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLCubemap *pOGLCubemap = dynamic_cast<OGLCubemap*>(m_pCubemap);

	if (m_pUniformTextureCubemap != nullptr && m_pUniformHasTextureCubemap != nullptr) {
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

			m_pUniformTextureCubemap->SetUniform(0);
			m_pUniformHasTextureCubemap->SetUniform(true);
		}
		else {
			m_pUniformHasTextureCubemap->SetUniform(false);
		}
	}

Error:
	return r;
}

RESULT OGLProgramSkybox::SetObjectUniforms(DimObj *pDimObj) {
	//auto matModel = pDimObj->GetModelMatrix();
	//m_pUniformModelMatrix->SetUniform(matModel);

	return R_PASS;
}

RESULT OGLProgramSkybox::SetCameraUniforms(camera *pCamera) {

	//auto ptEye = pCamera->GetOrigin();
	//auto matV = pCamera->GetViewMatrix();
	auto matP = pCamera->GetProjectionMatrix();
	//auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
	auto matViewOrientation = pCamera->GetOrientationMatrix();

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();

		plane householderReflectionPlane = reflectionPlane;
		householderReflectionPlane.SetPlanePosition(point(0.0f, 0.0f, 0.0f));

		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matHouseholderReflection = ReflectionMatrix(householderReflectionPlane);

		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));

		//matV = matFlip * matV * matReflection;
		matViewOrientation = matFlip * matViewOrientation * matHouseholderReflection;
	}


	//m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	// m_pUniformModelViewMatrix
	//m_pUniformViewProjectionMatrix->SetUniform(matVP);
	m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

	return R_PASS;
}

RESULT OGLProgramSkybox::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	//auto ptEye = pStereoCamera->GetEyePosition(eye);
	//auto matV = pStereoCamera->GetViewMatrix(eye);
	auto matP = pStereoCamera->GetProjectionMatrix(eye);
	//auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
	auto matViewOrientation = pStereoCamera->GetOrientationMatrix();

	if (m_pReflectionObject != nullptr) {
		plane reflectionPlane = dynamic_cast<quad*>(m_pReflectionObject)->GetPlane();

		plane householderReflectionPlane = reflectionPlane;
		householderReflectionPlane.SetPlanePosition(point(0.0f, 0.0f, 0.0f));

		auto matReflection = ReflectionMatrix(reflectionPlane);
		auto matHouseholderReflection = ReflectionMatrix(householderReflectionPlane);

		auto matFlip = ReflectionMatrix(plane(plane::type::XZ));

		//matV = matFlip * matV * matReflection;
		matViewOrientation = matFlip * matViewOrientation * matHouseholderReflection;
	}

	//m_pUniformViewMatrix->SetUniform(matV);
	m_pUniformProjectionMatrix->SetUniform(matP);
	// m_pUniformModelViewMatrix
	//m_pUniformViewProjectionMatrix->SetUniform(matVP);
	m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

	return R_PASS;
}