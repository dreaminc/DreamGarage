#ifndef OGLPROGRAM_BLINN_PHONG_TEXTURE_SHADOW_H_
#define OGLPROGRAM_BLINN_PHONG_TEXTURE_SHADOW_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramBlinnPhongTexture.h
// Blinn Phong Texture OGL shader program - this uses the blinn phong
// shading model along with one color texture

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "OGLObj.h"
#include "OGLTexture.h"

class OGLProgramBlinnPhongTextureShadow : public OGLProgram {
public:
	OGLProgramBlinnPhongTextureShadow(OpenGLImp *pParentImp) :
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
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformDepthViewProjectionMatrix), std::string("u_mat4DepthVP")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureBump), std::string("u_hasTextureBump")));
		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureBump), std::string("u_textureBump")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureColor), std::string("u_hasTextureColor")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureColor), std::string("u_textureColor")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureDepth), std::string("u_hasTextureDepth")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDepth), std::string("u_textureDepth")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureAmbient), std::string("u_hasTextureAmbient")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureAmbient), std::string("u_textureAmbient")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureDiffuse), std::string("u_hasTextureDiffuse")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDiffuse), std::string("u_textureDiffuse")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformHasTextureSpecular), std::string("u_hasTextureSpecular")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureSpecular), std::string("u_textureSpecular")));

		// Uniform Blocks
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		RESULT r = R_PASS;

		// color texture
		SetTextureUniform(pOGLObj->GetColorTexture(), m_pUniformTextureColor, m_pUniformHasTextureColor);

		// material textures
		SetTextureUniform(pOGLObj->GetTextureAmbient(), m_pUniformTextureAmbient, m_pUniformHasTextureAmbient);
		SetTextureUniform(pOGLObj->GetTextureDiffuse(), m_pUniformTextureDiffuse, m_pUniformHasTextureDiffuse);
		SetTextureUniform(pOGLObj->GetTextureSpecular(), m_pUniformTextureSpecular, m_pUniformHasTextureSpecular);

		// bump texture
		// TODO: add bump texture to shader
		m_pUniformHasTextureBump->SetUniform(pOGLObj->GetBumpTexture() != nullptr);

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

		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(m_pOGLProgramDepth);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformHasTextureDepth->SetUniform(true);
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());

			// TODO: Might be better to formalize this (units are simply routes mapped to the uniform
			pOGLProgramShadowDepth->SetDepthTexture(1);
			m_pUniformTextureDepth->SetUniform(1);
		}
		else
		{
			m_pUniformHasTextureDepth->SetUniform(false);
		}

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

		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(m_pOGLProgramDepth);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());

			// TODO: Might be better to formalize this (units are simply routes mapped to the uniform
			pOGLProgramShadowDepth->SetDepthTexture(1);
			m_pUniformTextureDepth->SetUniform(1);
		}

		return R_PASS;
	}

private:

	void SetTextureUniform(OGLTexture* pTexture,
		OGLUniformSampler2D* pTextureUniform,
		OGLUniformBool* pBoolUniform) {
		if (pTexture) {
			pBoolUniform->SetUniform(true);
			pTexture->OGLActivateTexture();
			pTextureUniform->SetUniform(pTexture);
		}
		else
		{
			pBoolUniform->SetUniform(false);
		}
	};

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
	OGLUniformMatrix4 *m_pUniformDepthViewProjectionMatrix;

	OGLUniformBool *m_pUniformHasTextureBump;

	OGLUniformBool *m_pUniformHasTextureColor;
	OGLUniformSampler2D *m_pUniformTextureColor;
	OGLUniformBool *m_pUniformHasTextureDepth;
	OGLUniformSampler2D *m_pUniformTextureDepth;

	OGLUniformBool *m_pUniformHasTextureAmbient;
	OGLUniformSampler2D *m_pUniformTextureAmbient;
	OGLUniformBool *m_pUniformHasTextureDiffuse;
	OGLUniformSampler2D *m_pUniformTextureDiffuse;
	OGLUniformBool *m_pUniformHasTextureSpecular;
	OGLUniformSampler2D *m_pUniformTextureSpecular;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_SHADOW_H_