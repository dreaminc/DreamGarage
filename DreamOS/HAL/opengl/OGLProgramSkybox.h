#ifndef OGLPROGRAM_SKYBOX_H_
#define OGLPROGRAM_SKYBOX_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLProgramSkybox : public OGLProgram {
public:
	OGLProgramSkybox(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp, "oglskybox")
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeNormal), std::string("inV_vec4Normal")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeUVCoord), std::string("inV_vec4UVCoord")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeTangent), std::string("inV_vec4Tangent")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeBitangent), std::string("inV_vec4Bitangent")));

		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelViewMatrix), std::string("u_mat4ModelView")));
		//CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewOrientationMatrix), std::string("u_mat4ViewOrientation")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformTextureCubeMap), std::string("u_textureCubeMap")));

	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		return R_NOT_IMPLEMENTED;
	}

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		//auto matModel = pDimObj->GetModelMatrix();
		//m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
	
		//auto ptEye = pCamera->GetOrigin();
		//auto matV = pCamera->GetViewMatrix();
		auto matP = pCamera->GetProjectionMatrix();
		//auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
		auto matViewOrientation = pCamera->GetOrientationMatrix();

		
		//m_pUniformViewMatrix->SetUniform(matV);
		m_pUniformProjectionMatrix->SetUniform(matP);
		// m_pUniformModelViewMatrix
		//m_pUniformViewProjectionMatrix->SetUniform(matVP);
		m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

		return R_PASS;
	}

	RESULT SetCameraUniforms(std::shared_ptr<stereocamera> pStereoCamera, EYE_TYPE eye) {
		//auto ptEye = pStereoCamera->GetEyePosition(eye);
		//auto matV = pStereoCamera->GetViewMatrix(eye);
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		//auto matVP = pStereoCamera->GetProjectionMatrix() * pStereoCamera->GetViewMatrix(eye);
		auto matViewOrientation = pStereoCamera->GetOrientationMatrix();
		
		//m_pUniformViewMatrix->SetUniform(matV);
		m_pUniformProjectionMatrix->SetUniform(matP);
		// m_pUniformModelViewMatrix
		//m_pUniformViewProjectionMatrix->SetUniform(matVP);
		m_pUniformViewOrientationMatrix->SetUniform(matViewOrientation);

		return R_PASS;
	}

private:
	// TODO: Pretty sure the Skybox doesn't need all of this
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;
	OGLVertexAttributeVector *m_pVertexAttributeNormal;
	OGLVertexAttributeUVCoord *m_pVertexAttributeUVCoord;
	OGLVertexAttributeVector *m_pVertexAttributeTangent;
	OGLVertexAttributeVector *m_pVertexAttributeBitangent;

	//OGLUniformMatrix4 *m_pUniformModelMatrix;
	//OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	//OGLUniformMatrix4 *m_pUniformModelViewMatrix;
	//OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformViewOrientationMatrix;

	OGLUniformSamplerCube *m_pUniformTextureCubeMap;
};

#endif // ! OGLPROGRAM_SKYBOX_H_