#ifndef OGLPROGRAM_TEXTURE_BIT_BLIT_H_
#define OGLPROGRAM_TEXTURE_BIT_BLIT_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramTextureBitBlit.h
// OGLProgramTextureBitBlit  is an OGLProgram that encapsulates the OGLProgram 
// for a minimal shader with a color texture that simply takes in a point, color, and UV coord
// and renders it using the usual suspects of required matrices (no lights)

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramTextureBitBlit : public OGLProgram {
public:
	OGLProgramTextureBitBlit(OpenGLImp *pParentImp) :
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

//	Error:
		return r;
	}

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
		auto matProjection = pCamera->GetProjectionMatrix();
		m_pUniformProjectionMatrix->SetUniform(matProjection);
		
		//return R_NOT_IMPLEMENTED;
		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		auto matProjection = pStereoCamera->GetProjectionMatrix(eye);
		if (m_pUniformProjectionMatrix)
			m_pUniformProjectionMatrix->SetUniform(matProjection);
		
		//return R_NOT_IMPLEMENTED;
		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	//OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasTexture;
};

#endif // ! OGLPROGRAM_TEXTURE_BIT_BLIT_H_