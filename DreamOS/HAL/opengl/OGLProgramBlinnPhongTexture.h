#ifndef OGLPROGRAM_BLINN_PHONG_TEXTURE_H_
#define OGLPROGRAM_BLINN_PHONG_TEXTURE_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramBlinnPhongTexture.h
// Blinn Phong Texture OGL shader program - this uses the blinn phong
// shading model along with one color texture

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramBlinnPhongTexture : public OGLProgram {
public:
	OGLProgramBlinnPhongTexture(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp),
		m_pLightsBlock(nullptr),
		m_pMaterialsBlock(nullptr)
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		// Vertex Attributes
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeNormal), std::string("inV_vec4Normal")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec4UVCoord")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeTangent), std::string("inV_vec4Tangent")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeBitangent), std::string("inV_vec4Bitangent")));

		// Uniforms
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureChannels), std::string("u_intTextureChannels")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformUseColorTexture), std::string("u_fUseColorTexture")));

		// Uniform Blocks
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		RESULT r = R_PASS;

		OGLTexture *pTexture = nullptr;

		if ((pTexture = pOGLObj->GetColorTexture()) != nullptr) {
			pTexture->OGLActivateTexture();
			m_pUniformTextureColor->SetUniform(pTexture);
			m_pUniformUseColorTexture->SetUniform(true);
			if (m_pUniformTextureChannels)
				m_pUniformTextureChannels->SetUniformInteger(pTexture->GetChannels());
		}
		else {
			m_pUniformUseColorTexture->SetUniform(false);
			if (m_pUniformTextureChannels)
				m_pUniformTextureChannels->SetUniformInteger(0);
		}

//	Error:
		return r;
	}

	RESULT SetLights(std::vector<light*> *pLights) {
		RESULT r = R_PASS;

		if (m_pLightsBlock != nullptr) {
			CR(m_pLightsBlock->SetLights(pLights));
			CR(m_pLightsBlock->UpdateOGLUniformBlockBuffers());
		}

	Error:
		return r;
	}

	RESULT SetMaterial(material *pMaterial) {
		RESULT r = R_PASS;

		if (m_pMaterialsBlock != nullptr) {
			CR(m_pMaterialsBlock->SetMaterial(pMaterial));
			CR(m_pMaterialsBlock->UpdateOGLUniformBlockBuffers());
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

		auto ptEye = pCamera->GetOrigin();
		auto matV = pCamera->GetViewMatrix();
		auto matP = pCamera->GetProjectionMatrix();
		auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

		m_pUniformViewMatrix->SetUniform(matV);
		//m_pUniformProjectionMatrix->SetUniform(matP);
		//m_pUniformModelViewMatrix
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		auto ptEye = pStereoCamera->GetEyePosition(eye);
		auto matV = pStereoCamera->GetViewMatrix(eye);
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

		m_pUniformViewMatrix->SetUniform(matV);
		//m_pUniformProjectionMatrix->SetUniform(matP);
		//m_pUniformModelViewMatrix->SetUniform(matM)
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

private:
	// Vertex Attribute
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeVector *m_pVertexAttributeNormal;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;
	OGLVertexAttributeVector *m_pVertexAttributeTangent;
	OGLVertexAttributeVector *m_pVertexAttributeBitangent;

	// Uniforms
	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewMatrix;
	//OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniform *m_pUniformTextureChannels;

	OGLUniformBool *m_pUniformUseColorTexture;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_H_