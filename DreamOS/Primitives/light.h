#ifndef LIGHT_H_
#define LIGHT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/light.h
// Light Primitive
// Standard light primitive

#include "VirtualObj.h"
#include "color.h"
#include "matrix/ProjectionMatrix.h"

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
	light();
	light(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	~light();

	matrix<virtual_precision, 4, 4> GetViewProjectionMatrix(virtual_precision width, virtual_precision height, virtual_precision nearPlane, virtual_precision farPlane);
	matrix<virtual_precision, 4, 4> GetViewProjectionMatrix(point ptSceneMax, point ptSceneMin, virtual_precision nearPlane, virtual_precision farPlane);

	LIGHT_TYPE GetLightType();
	light_precision GetPower();

	color GetDiffuseColor();
	color GetSpecularColor();
	vector GetLightDirection();

	RESULT EnableShadows();
	RESULT DisableShadows();
	bool IsShadowEmitter();

	RESULT RotateLightDirection(rotation_precision phi, rotation_precision theta, rotation_precision psi);
	RESULT RotateLightDirectionXAxis(rotation_precision theta);
	RESULT RotateLightDirectionYAxis(rotation_precision theta);
	RESULT RotateLightDirectionZAxis(rotation_precision theta);

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