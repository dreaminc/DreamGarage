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

		///*
		//CN(AddLight(LIGHT_POINT, 1.0f, point(0.0f, 3.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f)));
		//*/

		///*
		float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.0f;
		CN(AddLight(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_BLUE), color(COLOR_BLUE), vector::jVector(-1.0f)));
		CN(AddLight(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_RED), color(COLOR_RED), vector::jVector(-1.0f)));
		CN(AddLight(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_GREEN), color(COLOR_GREEN), vector::jVector(-1.0f)));
		//*/

		texture *pBumpTexture = MakeTexture(L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
		texture *pBumpTexture2 = MakeTexture(L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);
		
		texture *pColorTexture = MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
		texture *pColorTexture2 = MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

		///*
		sphere *pSphere1 = AddSphere(0.5f, 40, 40);
		pSphere1->SetColorTexture(pColorTexture);
		pSphere1->SetBumpTexture(pBumpTexture);

		sphere *pSphere2 = AddSphere(0.5f, 40, 40);
		pSphere2->SetColorTexture(pColorTexture2);
		pSphere2->SetBumpTexture(pBumpTexture2);
		
		pSphere2->translateX(5.0f);
		//*/

		/*
		volume *pVolume = AddVolume(1.0f);
		pVolume->translateX(5.0f);
		*/



	Error:
		return r;
	}

private:

};


#endif	// DREAM_GARAGE_H_