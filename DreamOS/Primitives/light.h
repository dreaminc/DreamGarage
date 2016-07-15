#ifndef LIGHT_H_
#define LIGHT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/light.h
// Light Primitive
// Standard light primitive

#include "VirtualObj.h"
#include "color.h"
#include "ProjectionMatrix.h"

typedef float light_precision;

typedef enum LightType {
	LIGHT_DIRECITONAL = 0,
	LIGHT_SPOT,
	LIGHT_POINT,
	LIGHT_INVALID = 2147483647	// define to ensure size of enum as 4 bytes
} LIGHT_TYPE;

#define DEFAULT_LIGHT_TYPE LIGHT_POINT
#define DEFAULT_LIGHT_INTENSITY 1.0f

class light : public VirtualObj {
public:
	light() :
		VirtualObj(),
		m_type(DEFAULT_LIGHT_TYPE),
		m_power(DEFAULT_LIGHT_INTENSITY),
		m_colorDiffuse(COLOR_WHITE),
		m_colorSpecular(COLOR_WHITE),
		m_vectorDirection(vector::jVector(-1.0f)),
		m_fShadowEmitter(false),
		m_spotAngle(0.0f)
	{
		// empty
	}

	light(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) :
		VirtualObj(ptOrigin),
		m_type(type),
		m_power(intensity),
		m_colorDiffuse(colorDiffuse),
		m_colorSpecular(colorSpecular),
		m_vectorDirection(vectorDirection)
	{
		// empty
	}

	~light() {
		// empty
	}

	matrix<virtual_precision, 4, 4> GetViewProjectionMatrix(virtual_precision width, virtual_precision height, virtual_precision nearPlane, virtual_precision farPlane) {
		matrix<virtual_precision, 4, 4> matVP;
		matVP.identity();

		if (m_type == LIGHT_DIRECITONAL) {
			matVP = ProjectionMatrix(width, height, nearPlane, farPlane) * RotationMatrix(m_vectorDirection) * TranslationMatrix(m_ptOrigin);
		}
		else if (m_type == LIGHT_SPOT) {
			matVP = ProjectionMatrix(width, height, nearPlane, farPlane, m_spotAngle) * RotationMatrix(m_vectorDirection) * TranslationMatrix(m_ptOrigin);
		}

		return matVP;
	}

	LIGHT_TYPE GetLightType() { return m_type; }
	light_precision GetPower() { return m_power; }

	color GetDiffuseColor() { return m_colorDiffuse; }
	color GetSpecularColor() { return m_colorSpecular; }
	vector GetLightDirection() { return m_vectorDirection; }

	RESULT EnableShadows() { m_fShadowEmitter = true; return R_FAIL; }
	RESULT DisableShadows() { m_fShadowEmitter = false; return R_FAIL; }
	bool IsShadowEmitter() { return m_fShadowEmitter; }

	RESULT RotateLightDirection(rotation_precision phi, rotation_precision theta, rotation_precision psi) {
		m_vectorDirection = RotationMatrix(phi, theta, psi) * m_vectorDirection;
		m_vectorDirection.Normalize();
		return R_PASS;
	}

	RESULT RotateLightDirectionXAxis(rotation_precision theta) {
		m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, theta) * m_vectorDirection;
		m_vectorDirection.Normalize();
		return R_PASS;
	}

	RESULT RotateLightDirectionYAxis(rotation_precision theta) {
		m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::Y_AXIS, theta) * m_vectorDirection;
		m_vectorDirection.Normalize();
		return R_PASS;
	}

	RESULT RotateLightDirectionZAxis(rotation_precision theta) {
		m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::Z_AXIS, theta) * m_vectorDirection;
		m_vectorDirection.Normalize();
		return R_PASS;
	}

private:
	LIGHT_TYPE m_type;		// NOTE: This is treated as an int
	light_precision m_power;
	color m_colorDiffuse;
	color m_colorSpecular;
	vector m_vectorDirection;
	virtual_precision m_spotAngle;

private:
	bool m_fShadowEmitter;
};

#endif // ! LIGHT_H_