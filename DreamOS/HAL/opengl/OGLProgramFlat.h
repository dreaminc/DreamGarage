#ifndef OGLPROGRAM_FLAT_H_
#define OGLPROGRAM_FLAT_H_

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLProgramFlat : public OGLProgram {
public:
	OGLProgramFlat(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp)
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec2UVCoord")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTexture), std::string("u_hasTexture")));

	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		RESULT r = R_PASS;

		OGLTexture *pTexture = nullptr;

		if ((pTexture = pOGLObj->GetColorTexture()) != nullptr) {
			pTexture->OGLActivateTexture();
			m_pUniformTextureColor->SetUniform(pTexture);
			m_pUniformHasTexture->SetUniform(true);
		}
		else
		{
			m_pUniformHasTexture->SetUniform(false);
		}

		return r;
	}

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
		auto matP = pCamera->GetProjectionMatrix();
		if (m_pUniformProjectionMatrix)
			m_pUniformProjectionMatrix->SetUniform(matP);

		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		if (m_pUniformProjectionMatrix)
			m_pUniformProjectionMatrix->SetUniform(matP);

		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasTexture;
};

#endif // ! OGLPROGRAM_FLAT_H_
