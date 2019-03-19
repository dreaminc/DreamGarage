#ifndef FOGPARAMS_H_
#define FOGPARAMS_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/material.h
// Base type for material

#include "color.h"

class fogparams {
public:
	fogparams();
	fogparams(float startDistance, float endDistance, float density, color fogColor);
	~fogparams();

private:
	// default lobby params
	float m_startDistance = 50.0f;
	float m_endDistance = 300.0f;
	float m_density = 0.05f;
	float reserved0 = 0.0f;
	color m_fogColor = color(161.0f / 255.0f, 197.0f / 255.0f, 202.0f / 255.0f, 1.0f);
};

#endif // ! MATERIAL_H_
