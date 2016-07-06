#ifndef OGLPROGRAM_SKYBOX_SCATTER_H_
#define OGLPROGRAM_SKYBOX_SCATTER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramSkybox.h
// OGLProgramSkybox is an OGLProgram that encapsulates the OGLProgram 
// for a skybox shader

#include "./RESULT/EHM.h"
#include "OGLProgram.h"

class OGLProgramSkyboxScatter : public OGLProgram {
public:
	OGLProgramSkyboxScatter(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp)
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
//		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

//		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewWidth), std::string("u_intViewWidth")));
//		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewHeight), std::string("u_intViewHeight")));



	Error:
		return r;
	}

	RESULT SetObjectTextures(OGLObj *pOGLObj) {
		return R_NOT_IMPLEMENTED;
	}

	RESULT SetObjectUniforms(DimObj *pDimObj) {
		auto matModel = pDimObj->GetModelMatrix();
		m_pUniformModelMatrix->SetUniform(matModel);

		return R_PASS;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
		
		auto matV = pCamera->GetViewMatrix();
		auto matP = pCamera->GetProjectionMatrix();
		auto matVP = matP * matV;
		auto matVO = pCamera->GetOrientationMatrix();

		auto pxWidth = pCamera->GetPXWidth();
		auto pxHeight = pCamera->GetPXHeight();

		if (m_pUniformViewMatrix)
			m_pUniformViewMatrix->SetUniform(matV);
		else
			DEBUG_OUT("View\n");
		
		if (m_pUniformProjectionMatrix)
			m_pUniformProjectionMatrix->SetUniform(matP);
		else
			DEBUG_OUT("Pro\n");
//		m_pUniformViewProjectionMatrix->SetUniform(matVP);
		if (m_pUniformViewOrientationMatrix)
			m_pUniformViewOrientationMatrix->SetUniform(matVO);
		else
			DEBUG_OUT("Ori\n");
/*
		if (m_pUniformViewWidth)
			m_pUniformViewWidth->SetUniformInteger(pxWidth);
		else
			DEBUG_OUT("Width %d\n", pxHeight);
		if (m_pUniformViewHeight)
			m_pUniformViewHeight->SetUniformInteger(pxHeight);
		else
			DEBUG_OUT("Height\n");
*/
		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {

		auto matV = pStereoCamera->GetViewMatrix(eye);
		auto matP = pStereoCamera->GetProjectionMatrix(eye);
		auto matVP = matP * matV;
		auto matVO = pStereoCamera->GetOrientationMatrix();

		auto pxWidth = pStereoCamera->GetPXWidth();
		auto pxHeight = pStereoCamera->GetPXHeight();

		if (m_pUniformViewMatrix)
			m_pUniformViewMatrix->SetUniform(matV);
		else
			DEBUG_OUT("View\n");
		
		if (m_pUniformProjectionMatrix)
			m_pUniformProjectionMatrix->SetUniform(matP);
		else
			DEBUG_OUT("Pro\n");
//		m_pUniformViewProjectionMatrix->SetUniform(matVP);
		if (m_pUniformViewOrientationMatrix)
			m_pUniformViewOrientationMatrix->SetUniform(matVO);
		else
			DEBUG_OUT("Ori\n");
/*
		if (m_pUniformViewWidth)
			m_pUniformViewWidth->SetUniformInteger(pxWidth);
		else
			DEBUG_OUT("Width %d\n", pxWidth);
		if (m_pUniformViewHeight)
			m_pUniformViewHeight->SetUniformInteger(pxHeight);
		else
			DEBUG_OUT("Height %d\n", pxHeight);
*/
		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;
	OGLVertexAttributeColor *m_pVertexAttributeColor;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewMatrix;
	OGLUniformMatrix4 *m_pUniformProjectionMatrix;
//	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
	OGLUniformMatrix4 *m_pUniformViewOrientationMatrix;

//	OGLUniform *m_pUniformViewWidth;
//	OGLUniform *m_pUniformViewHeight;
};
#endif // ! OGLPROGRAM_SKYBOX_SCATTER_H_
