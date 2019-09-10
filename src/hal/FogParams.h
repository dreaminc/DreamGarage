#ifndef FOG_PARAMS_H_
#define FOG_PARAMS_H_

#include "core/ehm/EHM.h"

// TODO: Not sure about this being in HAL
// Dream HAL
// dos/src/hal/FogParams.h

#include "core/primitives/color.h"

#include "core/types/DObject.h"

class FogParams : public DObject {
public:
	FogParams();
	FogParams(float startDistance, float endDistance, float density, color fogColor);
	~FogParams();

private:
	// Default lobby params
	float m_startDistance = 50.0f;
	float m_endDistance = 300.0f;
	float m_density = 0.05f;
	float reserved0 = 0.0f;
	color m_fogColor = color(161.0f / 255.0f, 197.0f / 255.0f, 202.0f / 255.0f, 1.0f);
};

#endif // ! FOG_PARAMS_H_
