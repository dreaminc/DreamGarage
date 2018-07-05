#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/material.h
// Base type for material

#include "color.h"

#define DEFAULT_MATERIAL_AMBIENT_LEVEL 0.1f
#define DEFUALT_MATERIAL_SHINE 100.0f

class material {
public:
	material();
	material(float shine, color colorAmbient, color colorDiffuse, color colorSpecular);
	material(float shine, float bump, color colorAmbient, color colorDiffuse, color colorSpecular, float ambient = DEFAULT_MATERIAL_AMBIENT_LEVEL);
	~material();

	RESULT SetColors(color colorAmbient, color colorDiffuse, color colorSpecular);
	RESULT SetAmbientColor(color colorAmbient);
	RESULT SetDiffuseColor(color colorDiffuse);
	RESULT SetSpecularColor(color colorSpecular);

	RESULT SetAmbientIntensity(float ambient);
	color GetDiffuseColor();

	RESULT SetShininess(float shine);
	RESULT SetBumpiness(float bumpiness);

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