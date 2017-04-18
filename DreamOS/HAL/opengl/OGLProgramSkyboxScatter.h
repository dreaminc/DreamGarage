#ifndef OGLPROGRAM_SKYBOX_SCATTER_H_
#define OGLPROGRAM_SKYBOX_SCATTER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

#include "Primitives/matrix/RotationMatrix.h"

class OGLProgramSkyboxScatter : public OGLProgram {
public:
	OGLProgramSkyboxScatter(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp, "oglskyboxscatter")
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));
		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributeColor), std::string("inV_vec4Color")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewMatrix), std::string("u_mat4View")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformProjectionMatrix), std::string("u_mat4Projection")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewOrientationMatrix), std::string("u_mat4ViewOrientation")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewWidth), std::string("u_intViewWidth")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewHeight), std::string("u_intViewHeight")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformSunDirection), std::string("u_vecSunDirection")));


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

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	float sunY = 1.0f;
	float theta = 0.0f;
	//float delta = 0.00005f;
	float delta = 0.0f;

	RESULT SetCameraUniforms(camera *pCamera) {
		
		auto matV = pCamera->GetViewMatrix();
		auto matP = pCamera->GetProjectionMatrix();
		auto matVP = matP * matV;
		auto matVO = pCamera->GetOrientationMatrix();

		auto pxWidth = pCamera->GetViewWidth();
		auto pxHeight = pCamera->GetViewHeight();

		vector sunDirection = vector(0.0f, sunY, 0.5f);
		sunDirection.Normalize();
		//sunY += 0.01f;
		theta += delta;
		sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, theta) * sunDirection;
		sunDirection.Normalize();

		m_pUniformSunDirection->SetUniform(sunDirection);
		m_pUniformViewMatrix->SetUniform(matV);
		m_pUniformProjectionMatrix->SetUniform(matP);
		m_pUniformViewOrientationMatrix->SetUniform(matVO);
		m_pUniformViewWidth->SetUniformInteger(pxWidth);
		m_pUniformViewHeight->SetUniformInteger(pxHeight);

		return R_PASS;
	}

	RESULT SetCameraUniforms(std::shared_ptr<stereocamera> pStereoCamera, EYE_TYPE eye) {

		auto matV = pStereoCamera->GetViewMatrix(eye);
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		auto matVP = matP * matV;
		auto matVO = pStereoCamera->GetOrientationMatrix();

		auto pxWidth = (pStereoCamera->GetViewWidth());
		auto pxHeight = (pStereoCamera->GetViewHeight());

		/*
		point sunDirection = point(0.3f, sunY, -0.5f);
		sunY += 0.0002f;
		DEBUG_OUT("%f\n", sunY);
		*/

		vector sunDirection = vector(0.0f, sunY, -0.5f);
		sunDirection.Normalize();
		//sunY += 0.01f;
		theta += delta;
		sunDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, theta) * sunDirection;
		sunDirection.Normalize();

		m_pUniformSunDirection->SetUniform(sunDirection);

		m_pUniformViewMatrix->SetUniform(matV);
		m_pUniformProjectionMatrix->SetUniform(matP);
		m_pUniformViewOrientationMatrix->SetUniform(matVO);

		m_pUniformViewWidth->SetUniformInteger(pxWidth);
		m_pUniformViewHeight->SetUniformInteger(pxHeight);

		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformViewOrientationMatrix;

	OGLUniform *m_pUniformViewWidth;
	OGLUniform *m_pUniformViewHeight;
	OGLUniformVector *m_pUniformSunDirection;
};
#endif // ! OGLPROGRAM_SKYBOX_SCATTER_H_
