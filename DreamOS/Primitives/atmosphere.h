#ifndef ATMOSPHERE_H_
#define ATMOSPHERE_H_

#include "RESULT/EHM.h"

#include <math.h>

// Atmosphere primitive
// Used to describe realistic atmospheric scattering

#include "sphere.h"

#define DEFAULT_RADIUS	1000.0f

// Constants for following scattering functions
#define G_RAYLEIGH		0.0f
#define G_MIE			0.995f
#define K_RAYLEIGH		4.0f
#define K_MIE			0.84f

#define AVG_DENSITY_H	0.25f


class atmosphere : public sphere {

private:

	float Phase(float costheta, float g) {

		float n1 = 3 * (1 - g*g);
		float d1 = 2 * (2 + g*g);
		float n2 = 1 + costheta*costheta;
		float d2 = pow(1 + g*g - 2*g*costheta, 1.5f);

		return (n1 / d1) * (n2 / d2);
	}



	float SunIntensity(float lambda) {
		// TODO: Add non-white suns
		return 1.0f;
	}

};

