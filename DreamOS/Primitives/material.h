#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/material.h
// Base type for material

#include "color.h"

class material {
public:
	material();
	material(float shine, color colorAmbient, color colorDiffuse, color colorSpecular);
	material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular, float ambient = 0.03f);
	~material();

	RESULT SetColors(color colorAmbient, color colorDiffuse, color colorSpecular);
	RESULT SetDiffuseColor(color colorDiffuse);
	RESULT SetAmbientIntensity(float ambient);

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