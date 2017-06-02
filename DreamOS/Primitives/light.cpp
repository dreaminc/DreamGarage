#include "light.h"

light::light() :
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

light::light(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) :
	VirtualObj(ptOrigin),
	m_type(type),
	m_power(intensity),
	m_colorDiffuse(colorDiffuse),
	m_colorSpecular(colorSpecular),
	m_vectorDirection(vectorDirection)
{
	// empty
}

light::~light() {
	// empty
}

matrix<virtual_precision, 4, 4> light::GetViewProjectionMatrix(virtual_precision width, virtual_precision height, virtual_precision nearPlane, virtual_precision farPlane) {
	matrix<virtual_precision, 4, 4> matVP;
	matVP.identity();

	quaternion qOrientation = quaternion::jQuaternion(0.0f);
	//qOrientation.RotateByVector(m_vectorDirection, 0.0f); //quaternion(0.0f, m_vectorDirection).RotateVector(vector::kVector(-1.0f));

	if (m_type == LIGHT_DIRECITONAL) {
		matVP = ProjectionMatrix(width, height, nearPlane, farPlane);
	}
	else if (m_type == LIGHT_SPOT) {
		matVP = ProjectionMatrix(width, height, nearPlane, farPlane, m_spotAngle);
	}

	matVP = matVP * RotationMatrix(m_vectorDirection, vector::jVector(1.0f)) * TranslationMatrix(GetOrigin());

	return matVP;
}

matrix<virtual_precision, 4, 4> light::GetViewProjectionMatrix(point ptSceneMax, point ptSceneMin, virtual_precision nearPlane, virtual_precision farPlane) {
	matrix<virtual_precision, 4, 4> matVP;
	matVP.identity();

	float borderFactor = 1.1f;

	auto matInvMax = inverse(RotationMatrix(m_vectorDirection, vector::jVector(1.0f)) * TranslationMatrix(GetOrigin()));

	point ptSceneMaxOriented = matInvMax * ptSceneMax;
	point ptSceneMinOriented = matInvMax * ptSceneMin;
	point ptSceneMidOriented = point::midpoint(ptSceneMaxOriented, ptSceneMinOriented);

	vector vSceneMinMaxOriented = ptSceneMaxOriented - ptSceneMinOriented;

	float width = std::fabs(vSceneMinMaxOriented.x()) * borderFactor;
	float height = std::fabs(vSceneMinMaxOriented.y()) * borderFactor;
	float depth = std::fabs(vSceneMinMaxOriented.z()) * borderFactor;

	if (m_type == LIGHT_DIRECITONAL) {
		matVP = ProjectionMatrix(width, height, nearPlane, farPlane);
	}
	else if (m_type == LIGHT_SPOT) {
		matVP = ProjectionMatrix(width, height, nearPlane, farPlane, m_spotAngle);
	}

	matVP = matVP * RotationMatrix(m_vectorDirection, vector::jVector(1.0f)) * TranslationMatrix(GetOrigin());

	return matVP;
}

LIGHT_TYPE light::GetLightType() { 
	return m_type; 
}

light_precision light::GetPower() { 
	return m_power; 
}

color light::GetDiffuseColor() { 
	return m_colorDiffuse; 
}

color light::GetSpecularColor() { 
	return m_colorSpecular; 
}

vector light::GetLightDirection() { 
	return m_vectorDirection; 
}

RESULT light::EnableShadows() { 
	m_fShadowEmitter = true; 
	return R_PASS; 
}

RESULT light::DisableShadows() { 
	m_fShadowEmitter = false; 
	return R_PASS; 
}

bool light::IsShadowEmitter() { 
	return m_fShadowEmitter; 
}

RESULT light::RotateLightDirection(rotation_precision phi, rotation_precision theta, rotation_precision psi) {
	m_vectorDirection = RotationMatrix(phi, theta, psi) * m_vectorDirection;
	m_vectorDirection.Normalize();
	return R_PASS;
}

RESULT light::RotateLightDirectionXAxis(rotation_precision theta) {
	m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::X_AXIS, theta) * m_vectorDirection;
	m_vectorDirection.Normalize();
	return R_PASS;
}

RESULT light::RotateLightDirectionYAxis(rotation_precision theta) {
	m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::Y_AXIS, theta) * m_vectorDirection;
	m_vectorDirection.Normalize();
	return R_PASS;
}

RESULT light::RotateLightDirectionZAxis(rotation_precision theta) {
	m_vectorDirection = RotationMatrix(RotationMatrix::ROTATION_MATRIX_TYPE::Z_AXIS, theta) * m_vectorDirection;
	m_vectorDirection.Normalize();
	return R_PASS;
}