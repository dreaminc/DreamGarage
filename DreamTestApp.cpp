#include "Logger/Logger.h"
#include "DreamTestApp.h"
#include <string>

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

cylinder *g_pCylinder = nullptr;
volume *g_pVolume = nullptr;
volume *g_pVolume2 = nullptr;
sphere *pSphere5 = nullptr;

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

	//g_pCylinder = AddCylinder(0.5f, 2.0f, 20, 5);
	//g_pCylinder->MoveTo(2.0f, 0.0f, 0.0f);

	//g_pCylinder->RotateZBy((float)(M_PI_4));

	/*
	g_pVolume = AddVolume(0.5f, 0.5f, 2.0f);
	g_pVolume->RotateZByDeg(45.0f);
	g_pVolume->MoveTo(point(-2.0f, 0.0f, 0.0f));
	AddPhysicsObject(g_pVolume);
	
	g_pVolume2 = AddVolume(0.5f, 0.5f, 2.0f);
	g_pVolume2->MoveTo(point(2.0f, 0.0f, 0.0f));
	g_pVolume2->RotateZBy((float)(M_PI_4));
	AddPhysicsObject(g_pVolume2);

	RegisterObjectCollision(g_pVolume2);
	//*/

	// TODO: Plane vs Sphere

	// Quad vs Sphere
	///*
	// TODO: Add normal for quad
	quad *pQuad1 = AddQuad(4.0f, 4.0f, 1, 1, nullptr, vector(0.0f, 1.0f, 0.0f));
	pQuad1->SetPosition(point(0.0f, -1.0f, 0.0f));
	pQuad1->SetImmovable(true);
	//AddPhysicsObject(pVolume1);

	sphere *pSphere1 = AddSphere(0.25f, 10, 10);
	pSphere1->SetPosition(point(3.0f, std::sqrt(2.0f)/2.0f, 0.0f));
	pSphere1->SetMass(1.0f);
	pSphere1->SetVelocity(-1.0f, 0.0f, 0.0f);
	//AddPhysicsObject(pSphere1);
	//*/

	// Volume vs Sphere
	/*
	volume *pVolume1 = AddVolume(1.0f);
	pVolume1->SetPosition(point(-1.0f, -0.0f, 0.0f));
	pVolume1->RotateZByDeg(45.0f);
	pVolume1->SetMass(1.0f);
	pVolume1->SetVelocity(1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pVolume1);

	sphere *pSphere1 = AddSphere(0.25f, 10, 10);
	pSphere1->SetPosition(point(3.0f, std::sqrt(2.0f)/2.0f, 0.0f));
	pSphere1->SetMass(1.0f);
	pSphere1->SetVelocity(-1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pSphere1);
	//*/

	// Volume vs Volume
	/*
	volume *pVolume1 = AddVolume(0.5f);
	pVolume1->SetPosition(point(-1.0f, 0.0f, 0.0f));
	pVolume1->SetMass(1.0f);
	pVolume1->SetVelocity(1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pVolume1);

	volume *pVolume2 = AddVolume(0.5f);
	pVolume2->SetPosition(point(1.0f, 0.0f, 0.0f));
	pVolume2->SetMass(1.0f);
	pVolume2->SetVelocity(-1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pVolume2);
	//*/

	/*
	sphere *pSphere1 = AddSphere(0.25f, 10, 10);
	pSphere1->SetPosition(point(1.0f, -0.0f, 0.0f));
	pSphere1->SetMass(1.0f);
	pSphere1->SetVelocity(-1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pSphere1);
	//*/

	// Sphere vs Sphere
	/*
	float radius = 0.1f;
	float padding = 0.01f;
	int num = 6;
	for (int i = 0; i < num; i++) {
		float startY = (((radius * 2.0) + padding) * (num));
		startY /= -2.0f;

		//for (int j = 0; j < (i + 1); j++) {
		for (int j = 0; j < num; j++) {
			sphere *pSphereTemp = AddSphere(radius, 10, 10);
			pSphereTemp->SetPosition(point(((2.0f * radius) + padding) * i, startY + (((radius * 2.0) + padding) * j), 0.0f));
			pSphereTemp->SetMass(1.0f);
			AddPhysicsObject(pSphereTemp);
		}
	}

	sphere *pSphere1 = AddSphere(3.0f, 10, 10);
	pSphere1->SetPosition(point(-7.0f, 0.0f, 0.0f));
	pSphere1->SetMass(10.0f);
	pSphere1->SetVelocity(1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pSphere1);
	//RegisterObjectCollision(pSphere1);

	//*/

	/*
	sphere *pSphereTemp = AddSphere(0.25f, 10, 10);
	pSphereTemp->SetPosition(point(0.0f, 0.0f, 0.0f));
	pSphereTemp->SetMass(1.0f);
	AddPhysicsObject(pSphereTemp);

	pSphereTemp = AddSphere(0.25f, 10, 10);
	pSphereTemp->SetPosition(point(0.5f, 0.0f, 0.0f));
	pSphereTemp->SetMass(1.0f);
	AddPhysicsObject(pSphereTemp);
	//*/

	/*
	pSphere5 = AddSphere(0.25f, 10, 10);
	pSphere5->SetPosition(point(0.0f, 0.0f, 0.0f));
	pSphere5->SetMass(1.0f);
	AddPhysicsObject(pSphere5);
	//RegisterObjectCollision(pSphere4);
	
	sphere *pSphere1 = AddSphere(0.25f, 10, 10);
	pSphere1->SetPosition(point(-1.0f, 0.0f, 0.0f));
	pSphere1->SetMass(1.0f);
	pSphere1->SetVelocity(1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pSphere1);
	//RegisterObjectCollision(pSphere1);

	sphere *pSphere2 = AddSphere(0.25f, 10, 10);
	pSphere2->SetPosition(point(1.0f, -0.0f, 0.0f));
	pSphere2->SetMass(1.0f);
	pSphere2->SetVelocity(-1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pSphere2);
	
	sphere *pSphere3 = AddSphere(0.25f, 10, 10);
	pSphere3->SetPosition(point(0.0f, 1.0f, 0.0f));
	pSphere3->SetMass(1.0f);
	pSphere3->SetVelocity(0.0f, -1.0f, 0.0f);
	AddPhysicsObject(pSphere3);
	
	sphere *pSphere4 = AddSphere(0.25f, 10, 10);
	pSphere4->SetPosition(point(0.0f, -1.0f, 0.0f));
	pSphere4->SetMass(1.0f);
	pSphere4->SetVelocity(0.0f, 1.0f, 0.0f);
	AddPhysicsObject(pSphere4);
	//RegisterObjectCollision(pSphere4);
	
	sphere *pSphere6 = AddSphere(0.25f, 10, 10);
	pSphere6->SetPosition(point(0.0f, 0.0f, 1.0f));
	pSphere6->SetMass(1.0f);
	pSphere6->SetVelocity(0.0f, 0.0f, -1.0f);
	AddPhysicsObject(pSphere6);
	
	sphere *pSphere7 = AddSphere(0.25f, 10, 10);
	pSphere7->SetPosition(point(0.0f, 0.0f, -1.0f));
	pSphere7->SetMass(1.0f);
	pSphere7->SetVelocity(0.0f, 0.0f, 1.0f);
	AddPhysicsObject(pSphere7);
	//RegisterObjectCollision(pSphere4);
	//*/


	//pCylinder->SetWireframe(true);

	/*
	float radius = 0.5f;
	int width = 10;
	int length = 10;
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < length; j++) {
			volume *pVolume = AddVolume(radius);
			pVolume->MoveTo(point(1.0f * radius * i, 0.0f, 1.0f * radius * j));
		}
	}
	*/

	/*
	AddModel(L"\\Models\\the-hague-apartment\\the-hague-apartment-fix.obj",
		nullptr,
		point(0.0f, 0.0f, 0.0f),
		0.1f,
		vector((float)(M_PI_2), 0.0f, 0.0f));
	//*/

	/*
	AddModel(L"\\Models\\warehouse\\industrial_style_interior.obj",
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
		//g_pCylinder->RotateByDeg(0.01f, 0.00f, 0.01f);
		//g_pCylinder->RotateZBy(0.001f);
		//g_pCylinder->RotateYBy(0.001f);
		//g_pCylinder->RotateXBy(0.001f);
		//g_pCylinder->translate(point(0.0005f, 0.000f, 0.000f));
		g_pCylinder->translateX(-0.0002f);
	}
	//*/

	///*
	if (g_pVolume != nullptr) {
		g_pVolume->SetColor(color(COLOR_WHITE));

		//g_pVolume->RotateByDeg(0.01f, 0.01f, 0.01f);
		g_pVolume->RotateXBy(0.003f);
		g_pVolume->RotateYBy(-0.001f);
		g_pVolume->RotateZBy(-0.003f);
		g_pVolume->translateX(0.0003f);
	}

	if (g_pVolume2 != nullptr) {
		g_pVolume2->SetColor(color(COLOR_WHITE));

		//g_pVolume2->RotateByDeg(0.01f, 0.01f, 0.01f);
		g_pVolume2->RotateXBy(0.002f);
		g_pVolume2->RotateYBy(0.001f);
		g_pVolume2->RotateZBy(0.005f);
		g_pVolume2->translateX(-0.0003f);
	}
	//*/

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

RESULT DreamTestApp::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;

	dynamic_cast<DimObj*>(oEvent->m_pCollisionObject)->SetColor(color(COLOR_RED));

	for (auto &pObj : oEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObj);
		pDimObj->SetColor(color(COLOR_PINK));
	}

//Error:
	return r;
}

RESULT DreamTestApp::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	HUD_OUT("DreamAPP command");

//Error:
	return r;
}
