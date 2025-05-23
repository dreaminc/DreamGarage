#ifndef OGLPROGRAM_BLINN_PHONG_SHADOW_H_
#define OGLPROGRAM_BLINN_PHONG_SHADOW_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLProgramBlinnPhongShadow.h

// OGLProgramBlinnPhongShadow is an OGL Program that utilizes the Blinn Phong
// algorithm and also employs shadow mapping

#include "OGLProgram.h"
#include "OGLImp.h"
#include "OGLProgramShadowDepth.h"

#include "core/matrix/BiasMatrix.h"

class OGLProgramBlinnPhongShadow : public OGLProgram {
public:
	OGLProgramBlinnPhongShadow(OGLImp *pParentImp, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE) :
		OGLProgram(pParentImp, "oglblinnphongshadow", optFlags),
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
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformDepthViewProjectionMatrix), std::string("u_mat4DepthVP")));

		// Billboard boolean uniforms
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfBillboard), std::string("u_fBillboard")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformfScale), std::string("u_fScale")));

		// Object position uniforms
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformObjectCenter), std::string("u_vec4ObjectCenter")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformEyePosition), std::string("u_vec4EyePosition")));

		// Uniform Blocks
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

		// Depth Map Texture
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureDepth), std::string("u_textureDepth")));

	Error:
		return r;
	}

	virtual RESULT SetupConnections() override {
		// TODO: do it
		return R_NOT_IMPLEMENTED;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		return R_NOT_IMPLEMENTED;
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

		m_pUniformObjectCenter->SetUniform(pDimObj->GetOrigin());

		quad *pQuad = dynamic_cast<quad *>(pDimObj);
		m_pUniformfBillboard->SetUniform(pQuad != nullptr && pQuad->IsBillboard());
		m_pUniformfScale->SetUniform(pQuad != nullptr && pQuad->IsScaledBillboard());

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {

		auto ptEye = pCamera->GetOrigin();
		auto matV = pCamera->GetViewMatrix();
		auto matP = pCamera->GetProjectionMatrix();
		auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();

		m_pUniformViewMatrix->SetUniform(matV);
		//m_pUniformModelViewMatrix
		m_pUniformViewProjectionMatrix->SetUniform(matVP);
		
		/*
		// TODO: this
		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(m_pOGLProgramDepth);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());

			// TODO: Might be better to formalize this (units are simply routes mapped to the uniform
			pOGLProgramShadowDepth->SetDepthTexture(0);
			m_pUniformTextureDepth->SetUniform(0);			
		}
		*/

		point origin = pCamera->GetOrigin();
		m_pUniformEyePosition->SetUniform(point(origin.x(), origin.y(), origin.z(), 1.0f));

		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera* pStereoCamera, EYE_TYPE eye) {
		auto ptEye = pStereoCamera->GetEyePosition(eye);
		auto matV = pStereoCamera->GetViewMatrix(eye);
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);

		m_pUniformViewMatrix->SetUniform(matV);
		//m_pUniformModelViewMatrix->SetUniform(matM)
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		point origin = pStereoCamera->GetOrigin();
		m_pUniformEyePosition->SetUniform(point(origin.x(), origin.y(), origin.z(), 1.0f));

		/*
		// TODO: this
		OGLProgramShadowDepth *pOGLProgramShadowDepth = dynamic_cast<OGLProgramShadowDepth*>(m_pOGLProgramDepth);
		if (pOGLProgramShadowDepth != nullptr) {
			m_pUniformDepthViewProjectionMatrix->SetUniform(pOGLProgramShadowDepth->GetViewProjectionMatrix());

			// TODO: Might be better to formalize this (units are simply routes mapped to the uniform
			pOGLProgramShadowDepth->SetDepthTexture(0);
			m_pUniformTextureDepth->SetUniform(0);
		}
		*/

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
	OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformDepthViewProjectionMatrix;

	OGLUniformPoint *m_pUniformObjectCenter;
	OGLUniformPoint *m_pUniformEyePosition;

	// Booleans
	OGLUniformBool *m_pUniformfBillboard;
	OGLUniformBool *m_pUniformfScale;

	// Textures
	OGLUniformSampler2D *m_pUniformTextureDepth;

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_SHADOW_H_
