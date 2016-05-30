#ifndef OGLPROGRAM_BLINN_PHONG_H_
#define OGLPROGRAM_BLINN_PHONG_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLProgramBlinnPhong : public OGLProgram {
public:
	OGLProgramBlinnPhong(OpenGLImp *pParentImp) :
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

		// Uniform Blocks
		CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pLightsBlock), std::string("ub_Lights")));
		//CR(RegisterUniformBlock(reinterpret_cast<OGLUniformBlock**>(&m_pMaterialsBlock), std::string("ub_material")));

	Error:
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
		auto matP = pStereoCamera->GetProjectionMatrix();
		auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);

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

	// Uniform Blocks
	OGLLightsBlock *m_pLightsBlock;
	OGLMaterialBlock *m_pMaterialsBlock;
};

#endif // ! OGLPROGRAM_BLINN_PHONG_H_