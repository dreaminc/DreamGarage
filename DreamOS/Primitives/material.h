#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/material.h
// Base type for material

#include "color.h"
#include "texture.h"

class material {
public:
	material() :
		m_shine(100.0f),
		m_bump(0.0f),
		m_ambient(0.03f),
		reserved3(0.0f),
		m_colorAmbient(COLOR_WHITE),
		m_colorDiffuse(COLOR_WHITE),
		m_colorSpecular(COLOR_WHITE)
	{
		// empty
	}

	material(float shine, color colorAmbient, color colorDiffuse, color colorSpecular) :
		m_shine(shine),
		m_bump(0.0f),
		m_ambient(0.0f),
		reserved3(0.0f),
		m_colorAmbient(colorAmbient),
		m_colorDiffuse(colorDiffuse),
		m_colorSpecular(colorSpecular)
	{
		// empty
	}

	material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular, float ambient = 0.03f) :
		m_shine(shine),
		m_bump(bump),
		m_ambient(ambient),
		reserved3(0.0f),
		m_colorAmbient(colorAmbient),
		m_colorDiffuse(colorDiffuse),
		m_colorSpecular(colorSpecular)
	{
		// empty
	}

	~material() {
		// empty
	}

	void Set(color colorAmbient, color colorDiffuse, color colorSpecular) {
		m_colorAmbient = colorAmbient;
		m_colorDiffuse = colorDiffuse;
		m_colorSpecular = colorSpecular;
	}

	void SetAmbientIntensity(float ambient) {
		m_ambient = ambient;
	}

private:
	float m_shine;				// specular
	float m_bump;				// bumpiness 
	float m_ambient;
	float reserved3;
	color m_colorAmbient;
	color m_colorDiffuse;
	color m_colorSpecular;
};

#endif // ! MATERIAL_H_