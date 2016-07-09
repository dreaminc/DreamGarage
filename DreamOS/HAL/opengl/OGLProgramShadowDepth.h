#ifndef OGLPROGRAM_SHADOW_DEPTH_H_
#define OGLPROGRAM_SHADOW_DEPTH_H_

// Dream OS
// DreamOS/HAL/opengl/OGLProgramShadowDepth.h
// OGLProgramShadowDepth is an OGLProgram that encapsulates the OGLProgram 
// for a shader used to simply capture a depth map as set by a given light

#include "./RESULT/EHM.h"
#include "OGLProgram.h"
#include "Primitives/ProjectionMatrix.h"
#include "Primitives/ViewMatrix.h"

#define SHADOW_MAP_WIDTH 1024
#define SHADOW_MAP_HEIGHT 1024

class OGLProgramShadowDepth : public OGLProgram {
public:
	OGLProgramShadowDepth(OpenGLImp *pParentImp) :
		OGLProgram(pParentImp)
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

		//CR(InitializeFrameBuffer(SHADOW_MAP_WIDTH, SHADOW_MAP_WIDTH, SHADOW_MAP_CHANNELS));

		// TODO: This is not the right way to create a pure depth map, but this is for now
		CR(InitializeFrameBuffer(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 1));

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
		//auto matVP = pCamera->GetProjectionMatrix() * pCamera->GetViewMatrix();
		//auto matVP = ProjectionMatrix(PROJECTION_MATRIX_ORTHOGRAPHIC, 10.0f, 10.0f, 0.1f, 1000.0f, 0.0f) * pCamera->GetViewMatrix();
		auto matVP = ProjectionMatrix(PROJECTION_MATRIX_ORTHOGRAPHIC, 10.0f, 10.0f, 0.1f, 1000.0f, 0.0f) * ViewMatrix(point(0.0f, -10.0f, 0.0f), -M_PI/2.0f, 0.0f, 0.0f);
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		//auto matVP = pStereoCamera->GetProjectionMatrix(eye) * pStereoCamera->GetViewMatrix(eye);
		//auto matVP = ProjectionMatrix(PROJECTION_MATRIX_ORTHOGRAPHIC, 10.0f, 10.0f, 0.1f, 1000.0f, 0.0f) * TranslationMatrix(0.0f, 5.0f, 0.0f);
		auto matVP = ProjectionMatrix(PROJECTION_MATRIX_ORTHOGRAPHIC, 10.0f, 10.0f, 0.1f, 1000.0f, 0.0f) * TranslationMatrix(0.0f, 5.0f, 0.0f) * RotationMatrix(vector::jVector(-1.0f), 0.0f);
		m_pUniformViewProjectionMatrix->SetUniform(matVP);

		return R_PASS;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;
};

#endif // ! OGLPROGRAM_SHADOW_DEPTH_H_