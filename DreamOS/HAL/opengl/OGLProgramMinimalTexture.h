#ifndef OGLPROGRAM_MINIMAL_TEXTURE_H_
#define OGLPROGRAM_MINIMAL_TEXTURE_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramMinimalTexture.h
// OGLProgramMinimalTexture  is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader with a color texture that simply takes in a point, color, and UV coord
// and renders it using the usual suspects of required matrices (no lights)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"

#include "OGLTexture.h"

class OGLProgramMinimalTexture : public OGLProgram {
public:
	OGLProgramMinimalTexture(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp)
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		// Vertex Attributes
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

		// Uniform Variables
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));

	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		RESULT r = R_PASS;

		OGLTexture *pTexture = nullptr;

		if ((pTexture = pOGLObj->GetColorTexture()) != nullptr) {
			pTexture->OGLActivateTexture();
			m_pUniformTextureColor->SetUniform(pTexture);
		}

	Error:
		return r;
	}

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
		auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
};

#endif // ! OGLPROGRAM_MINIMAL_TEXTURE_H_