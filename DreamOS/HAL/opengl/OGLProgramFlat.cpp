#include "OGLProgramFlat.h"

#include "Scene/ObjectStoreImp.h"
#include "Scene/ObjectStore.h"

#include "Primitives/stereocamera.h"
#include "Primitives/font.h"

#include "OGLObj.h"
#include "OGLTexture.h"

OGLProgramFlat::OGLProgramFlat(OpenGLImp *pParentImp) :
	OGLProgram(pParentImp, "oglflat")
{
	// empty
}

RESULT OGLProgramFlat::OGLInitialize() {
	RESULT r = R_PASS;

	CR(OGLProgram::OGLInitialize());

	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
	CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasColorTexture), std::string("u_hasColorTexture")));

	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfDistanceMap), std::string("u_fDistanceMap")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformBuffer), std::string("u_buffer")));
	CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformGamma), std::string("u_gamma")));

Error:
	return r;
}

RESULT OGLProgramFlat::SetupConnections() {
	RESULT r = R_PASS;

	// Inputs
	CR(MakeInput<stereocamera>("camera", &m_pCamera, DCONNECTION_FLAGS::PASSIVE));
	CR(MakeInput<FlatContext>("flatcontext", &m_pFlatContext, DCONNECTION_FLAGS::PASSIVE));

	// Note: This is a special case here, most framebuffer inputs will WANT to be active (another shader)
	CR(MakeInput<framebuffer>("framebuffer", (framebuffer**)(&m_pOGLFramebuffer), DCONNECTION_FLAGS::PASSIVE));

Error:
	return r;
}

RESULT OGLProgramFlat::SetFlatFramebuffer(framebuffer *pFramebuffer) {
	m_pOGLFramebuffer = (OGLFramebuffer*)pFramebuffer;
	return R_PASS;
}

RESULT OGLProgramFlat::SetCamera(stereocamera *pCamera) {
	m_pCamera = pCamera;
	return R_PASS;
}

RESULT OGLProgramFlat::SetFlatContext(FlatContext *pFlatContext) {
	m_pFlatContext = pFlatContext;
	return R_PASS;
}

RESULT OGLProgramFlat::ProcessNode(long frameID) {
	RESULT r = R_PASS;

	CR(UseProgram());

	CR(BindToFramebuffer(m_pOGLFramebuffer));

	CR(SetStereoCamera(m_pCamera, EYE_MONO));
	CR(RenderObject(m_pFlatContext));

	CR(UnbindFramebuffer());

Error:
	return r;
}

RESULT OGLProgramFlat::RenderFlatContext(FlatContext *pFlatContext) {
	RESULT r = R_PASS;

	CR(UseProgram());

	CR(BindToFramebuffer(m_pOGLFramebuffer));
	
	{
		float left = pFlatContext->GetLeft(false);
		float right = pFlatContext->GetRight(false);

		float top = pFlatContext->GetTop(false);
		float bottom = pFlatContext->GetBottom(false);

		float nearPlane = -1000.0f;
		float farPlane = 1000.0f;

		// TODO: Why the negative one?
		//matrix<float, 4,4> matP = ProjectionMatrix::MakeOrtho(left, right, top * -1.0f, bottom * -1.0f, nearPlane, farPlane);
		matrix<float, 4,4> matP = ProjectionMatrix::MakeOrthoYAxis(left, right, top * -1.0f, bottom * -1.0f, nearPlane, farPlane);
		//matrix<float, 4,4> matP = ProjectionMatrix::MakeOrthoYAxis(left, right, top, bottom, nearPlane, farPlane);
		//auto matFlatContextMM = pFlatContext->GetModelMatrix();
		//auto qOrientation = pFlatContext->GetOrientation(false);
		//qOrientation.Reverse();
		//float tempX = qOrientation.x();
		//qOrientation.x() = qOrientation.z();
		//qOrientation.z() = tempX;

		//matrix<virtual_precision, 4, 4> matRotation = inverse(matFlatContextMM);
		//matrix<virtual_precision, 4, 4> matRotation = inverse(RotationMatrix(qOrientation));
		//matrix<virtual_precision, 4, 4> tempXRow;
/*
		tempXRow.element(0, 0) = matRotation.element(0, 0);
		tempXRow.element(0, 1) = matRotation.element(0, 1);
		tempXRow.element(0, 2) = matRotation.element(0, 2);

		matRotation.element(0, 0) = matRotation.element(1, 0);
		matRotation.element(0, 1) = matRotation.element(1, 1);
		matRotation.element(0, 2) = matRotation.element(1, 2);

		matRotation.element(1, 0) = tempXRow.element(0, 0);
		matRotation.element(1, 1) = tempXRow.element(0, 1);
		matRotation.element(1, 2) = tempXRow.element(0, 2);
		//*/

		//matRotation[0] = tempZRow;
		//float tempX = matRotation[0]
		//auto matRotation = pFlatContext->GetOrientationMatrix();
		//matP = matRotation * matP;
	//	matP = matP * matRotation;
		//if (!m_pFlatContext->UseVirtualModelMatrix()) {
		//	matP = matFlatContextMM * matP;
		//}

		m_pUniformProjectionMatrix->SetUniform(matP);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		//if (pFlatContext->HasChildren()) {
		//	CR(RenderChildren(pFlatContext));
		//}
		CR(RenderObject(pFlatContext));
	}

	CR(UnbindFramebuffer());

Error:
	return r;
}

RESULT OGLProgramFlat::SetObjectTextures(OGLObj *pOGLObj) {
	RESULT r = R_PASS;

	OGLTexture *pTexture = nullptr;

	if ((pTexture = pOGLObj->GetOGLTextureDiffuse()) != nullptr) {
		m_pParentImp->glActiveTexture(GL_TEXTURE0);

		m_pParentImp->BindTexture(pTexture->GetOGLTextureTarget(), pTexture->GetOGLTextureIndex());
		m_pUniformTextureColor->SetUniform(0);
		
		m_pUniformfDistanceMap->SetUniform(pTexture->IsDistanceMapped());
		m_pUniformHasColorTexture->SetUniform(true);
	}
	else {
		m_pUniformHasColorTexture->SetUniform(false);
		m_pUniformfDistanceMap->SetUniform(false);
	}

	return r;
}

RESULT OGLProgramFlat::SetObjectUniforms(DimObj *pDimObj) {
	if (m_pFlatContext->UseVirtualModelMatrix()) { 
		auto matModel = pDimObj->VirtualObj::GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
	}
	else {
//		auto matModel = pDimObj->VirtualObj::GetModelMatrix();
		//auto matModel = pDimObj->GetFlatModelMatrix();
		auto matModel = pDimObj->GetFlatModelMatrix();
		//auto matModel = (TranslationMatrix(pDimObj->VirtualObj::GetOrigin()) * ScalingMatrix(pDimObj->VirtualObj::GetScale()));
		//auto matModel = (TranslationMatrix(pDimObj->GetOrigin()) * RotationMatrix(pDimObj->VirtualObj::GetOrientation()) * ScalingMatrix(pDimObj->GetScale()));
		//auto matModel = (TranslationMatrix(pDimObj->VirtualObj::GetOrigin()) * RotationMatrix(pDimObj->GetOrientation()) * ScalingMatrix(pDimObj->VirtualObj::GetScale()));
		//auto matModel = pDimObj->VirtualObj::GetModelMatrix() * m_pFlatContext->VirtualObj::GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);
		//pDimObj->GetRelativeModelMatrix();
	}

	// could do with a flag in DimObj
	/*
	text *pText = dynamic_cast<text*>(pDimObj);
	if (pText != nullptr) {
		/*
		float buffer = pText->GetFont()->GetBuffer();
		float gamma = pText->GetFont()->GetGamma();

		m_pUniformBuffer->SetUniformFloat(&buffer);
		m_pUniformGamma->SetUniformFloat(&gamma);

		//m_pUniformfDistanceMap->SetUniform(pText->GetFont()->HasDistanceMap());
		//matModel = pDimObj->VirtualObj::GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(pDimObj->VirtualObj::GetModelMatrix());
	}
	else {
		//m_pUniformfDistanceMap->SetUniform(true);
		m_pUniformModelMatrix->SetUniform(pDimObj->GetModelMatrix());
	}
	//*/

	return R_PASS;
}

RESULT OGLProgramFlat::SetCameraUniforms(camera *pCamera) {
	auto matP = pCamera->GetProjectionMatrix();

	if (m_pUniformProjectionMatrix) {
		m_pUniformProjectionMatrix->SetUniform(matP);
	}

	return R_PASS;
}

RESULT OGLProgramFlat::SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
	auto matP = pStereoCamera->GetProjectionMatrix(eye);

	if (m_pUniformProjectionMatrix) {
		m_pUniformProjectionMatrix->SetUniform(matP);
	}

	return R_PASS;
}
