#ifndef LIGHT_H_
#define LIGHT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/light.h
// Light Primitive
// Standard light primitive

#include "VirtualObj.h"
#include "color.h"

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
		m_vectorDirection(vector::jVector(-1.0f))
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

	LIGHT_TYPE GetLightType() { return m_type; }
	light_precision GetPower() { return m_power; }

	color GetDiffuseColor() { return m_colorDiffuse; }
	color GetSpecularColor() { return m_colorSpecular; }
	vector GetLightDirection() { return m_vectorDirection; }


private:
	LIGHT_TYPE m_type;		// NOTE: This is treated as an int
	light_precision m_power;
	color m_colorDiffuse;
	color m_colorSpecular;
	vector m_vectorDirection;
};

#endif // ! LIGHT_H_