#ifndef DREAM_GARAGE_H_
#define DREAM_GARAGE_H_

#include "RESULT/EHM.h"

// DREAM GARAGE
// DreamGarage.h
// This is the Dream Garage application layer which is derived from DreamOS
// which is the interface to the engine and platform layers for the application

#include "DreamOS/DreamOS.h"

class DreamGarage : public DreamOS {
public:
	DreamGarage() {
		// empty
	}

	~DreamGarage() {
		// empty
	}

	RESULT LoadScene() {
		RESULT r = R_PASS;
		
		// Add lights
		light *pLight = NULL;

		///*
		pLight = new light(LIGHT_POINT, 1.0f, point(0.0f, 3.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
		CR(AddLight(pLight));
		//*/

		///*
		float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.0f;
		pLight = new light(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_BLUE), color(COLOR_BLUE), vector::jVector(-1.0f));
		CR(AddLight(pLight));

		pLight = new light(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_RED), color(COLOR_RED), vector::jVector(-1.0f));
		CR(AddLight(pLight));

		pLight = new light(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_GREEN), color(COLOR_GREEN), vector::jVector(-1.0f));
		CR(AddLight(pLight));
		//*/

	Error:
		return r;
	}

private:

};


#endif	// DREAM_GARAGE_H_