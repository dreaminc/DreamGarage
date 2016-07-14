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
		OGLProgram(pParentImp),
		m_pShadowEmitter(nullptr)
	{
		// empty
	}

	RESULT OGLInitialize() {
		RESULT r = R_PASS;

		CR(OGLProgram::OGLInitialize());

		CR(RegisterVertexAttribute(reinterpret_cast<OGLVertexAttribute**>(&m_pVertexAttributePosition), std::string("inV_vec4Position")));

		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformModelMatrix), std::string("u_mat4Model")));
		CR(RegisterUniform(reinterpret_cast<OGLUniform**>(&m_pUniformViewProjectionMatrix), std::string("u_mat4ViewProjection")));

		// Create this as a pure depth map texture
		CR(InitializeDepthToTexture(GL_DEPTH_COMPONENT16, GL_FLOAT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT));

	Error:
		return r;
	}

	RESULT SetLights(std::vector<light*> *pLights) {
		RESULT r = R_PASS;

		// TODO: This will use the first light that has shadows enabled
		for (auto it = pLights->begin(); it != pLights->end(); it++) {
			if ((*it)->IsShadowEmitter()) {
				m_pShadowEmitter = (*it);
			}
		}

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

	matrix<virtual_precision, 4, 4>GetViewProjectionMatrix() {
		matrix<virtual_precision, 4, 4> matVP;
		matVP.identity();

		if (m_pShadowEmitter != nullptr) {
			matVP = m_pShadowEmitter->GetViewProjectionMatrix(30.0f, 30.0f, 0.1f, 1000.0f);
		}
		else {
			matVP = ProjectionMatrix(30.0f, 30.0f, 0.1f, 1000.0f) * RotationMatrix(-M_PI_2, 0.0f, 0.0f) * TranslationMatrix(point(0.0f, 10.0f, 0.0f));
		}

		return matVP;
	}

	RESULT SetCameraUniforms(camera *pCamera) {
		return m_pUniformViewProjectionMatrix->SetUniform(GetViewProjectionMatrix());
	}

	RESULT SetCameraUniforms(stereocamera *pStereoCamera, EYE_TYPE eye) {
		return m_pUniformViewProjectionMatrix->SetUniform(GetViewProjectionMatrix());
	}

	RESULT SetShadowCastingLightSource(light *pLight) {
		m_pShadowEmitter = pLight;
		return R_PASS;
	}

	light *pGetShadowCastingLight() {
		return m_pShadowEmitter;
	}

private:
	OGLVertexAttributePoint *m_pVertexAttributePosition;

	OGLUniformMatrix4 *m_pUniformModelMatrix;
	OGLUniformMatrix4 *m_pUniformViewProjectionMatrix;

private:
	light *m_pShadowEmitter;
};

#endif // ! OGLPROGRAM_SHADOW_DEPTH_H_