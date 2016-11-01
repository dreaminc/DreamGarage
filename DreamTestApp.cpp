#include "Logger/Logger.h"
#include "DreamTestApp.h"
#include <string>

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

cylinder *g_pCylinder = nullptr;
volume *g_pVolume = nullptr;

RESULT DreamTestApp::LoadScene() {
	RESULT r = R_PASS;

	// IO
	//RegisterSubscriber((SK_SCAN_CODE)('C'), this);

	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamApp, this);

	AddSkybox();

	light *pLight = AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));

	//pLight->EnableShadows();

	/*
	quad *pBQuad = AddQuad(10.0f, 20.0f, 200, 200);// , pHeightTextureCobble);
	pBQuad->MoveTo(point(0.0f, -1.0f, 0.0f));

	sphere *pSphere = AddSphere(0.5f, 10, 10, color(COLOR_RED));
	pSphere->MoveTo(1.5f, 0.5f, 0.0f);
	
	volume *pVolume = AddVolume(0.5f, false);
	pVolume->MoveTo(-1.5f, 0.5f, 0.0f);
	pVolume->SetWireframe(true);
	*/

	// TODO: sphere AABB
	//sphere *pSphere = AddSphere(0.5f, 10, 10);
	//pSphere->MoveTo(1.5f, 0.5f, 0.0f);

	g_pCylinder = AddCylinder(0.5f, 2.0f, 20, 5);
	//g_pCylinder->MoveTo(0.0f, -1.0f, 0.0f);

	//g_pCylinder->RotateZBy((float)(M_PI_4));

	//g_pVolume = AddVolume(1.0f);
	//g_pVolume->MoveTo(point(0.0f, 0.0f, 1.0f));
	//g_pVolume->RotateZBy((float)(M_PI_4));

	//pCylinder->SetWireframe(true);

	/*
	AddModel(L"\\Models\\the-hague-apartment\\the-hague-apartment-fix.obj",
		nullptr,
		point(0.0f, 0.0f, 0.0f),
		0.1f,
		vector((float)(M_PI_2), 0.0f, 0.0f));
	//*/

	//Error:
	return r;
}

RESULT DreamTestApp::Update(void) {
	RESULT r = R_PASS;

	///*
	if (g_pCylinder != nullptr) {
		g_pCylinder->RotateByDeg(0.01f, 0.00f, 0.01f);
		//g_pCylinder->RotateZBy(0.001f);
		//g_pCylinder->RotateYBy(0.001f);
		//g_pCylinder->RotateXBy(0.001f);
		g_pCylinder->translate(point(0.0001f, 0.0001f, 0.001f));
	}
	//*/

	if (g_pVolume != nullptr) {
		g_pVolume->RotateByDeg(0.01f, 0.01f, 0.01f);
		//g_pVolume->RotateXBy(0.001f);
		//g_pVolume->RotateYBy(0.001f);
		//g_pVolume->RotateZBy(0.001f);
		g_pVolume->translateX(0.0001f);
	}

	//Error:
	return r;
}

RESULT DreamTestApp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;
	/*
	switch (kbEvent->KeyCode) {
	case (SK_SCAN_CODE)('C') : {
	if (kbEvent->KeyState != 0) {
	HUD_OUT("Key 'C' is pressed - switch model");
	SendSwitchHeadMessage();
	}
	}
	}
	*/
	//Error:
	return r;
}

RESULT DreamTestApp::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	HUD_OUT("DreamAPP command");

//Error:
	return r;
}
