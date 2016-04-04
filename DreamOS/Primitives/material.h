#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/material.h
// Base type for material

#include "color.h"

class material {
public:
	material() :
		m_shine(100.0f),
		m_bump(0.0f),
		reserved2(0.0f),
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
		reserved2(0.0f),
		reserved3(0.0f),
		m_colorAmbient(colorAmbient),
		m_colorDiffuse(colorDiffuse),
		m_colorSpecular(colorSpecular)
	{
		// empty
	}

	material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular) :
		m_shine(shine),
		m_bump(bump),
		reserved2(0.0f),
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

private:
	float m_shine;				// specular
	float m_bump;				// bumpiness 
	float reserved2;
	float reserved3;
	color m_colorAmbient;
	color m_colorDiffuse;
	color m_colorSpecular;
};

#endif // ! MATERIAL_H_