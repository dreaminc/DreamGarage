#include "Logger/Logger.h"
#include "DreamTestApp.h"
#include <string>

#include "PhysicsEngine/PhysicsEngineTestSuite.h"

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

	m_pPhysicsEngineTestSuite = std::make_shared<PhysicsEngineTestSuite>(this);
	CN(m_pPhysicsEngineTestSuite);
	CR(m_pPhysicsEngineTestSuite->Initialize());

	AddSkybox();

	light *pLight = AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(-0.2f, -1.0f, -0.5f));
	pLight->EnableShadows();

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
	/*
	double spacing = 2.0f;
	double angleFactor = 0.2f;
	
	quad *pQuad1 = AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(angleFactor, 1.0f, 0.0f));
	pQuad1->SetPosition(point(-spacing, -1.0f, 0.0f));
	pQuad1->SetMass(1.0f);
	pQuad1->SetImmovable(true);
	AddPhysicsObject(pQuad1);

	quad *pQuad2 = AddQuad(1.0f, 1.0f, 1, 1, nullptr, vector(-angleFactor, 1.0f, 0.0f));
	pQuad2->SetPosition(point(spacing, -1.0f, 0.0f));
	pQuad2->SetMass(1.0f);
	pQuad2->SetImmovable(true);
	AddPhysicsObject(pQuad2);

	sphere *pSphere1 = AddSphere(0.25f, 10, 10);
	pSphere1->SetPosition(point(-spacing, 2.0f, 0.0f));
	pSphere1->SetMass(1.0f);
	//pSphere1->SetVelocity(0.0f, -1.0f, 0.0f);
	AddPhysicsObject(pSphere1);
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

	//volume *pVolume = nullptr;
	//sphere *pSphere = nullptr;
	quad *pQuad = nullptr;
	composite *pComposite = nullptr;
	
	// Composite vs plane

	/*
	auto pVolume = AddVolume(5.0, 5.0, 1.0f);
	pVolume->SetPosition(point(0.0f, -3.0f, 0.0f));
	pVolume->SetMass(100000.0f);
	pVolume->SetImmovable(true);
	AddPhysicsObject(pVolume);
	//*/

	/*
	auto pSphere = AddSphere(0.5f, 10, 10);
	pSphere->SetPosition(point(0.0f, 0.0f, 0.0f));
	pSphere->SetMass(1.0f);
	pSphere->SetVelocity(vector(0.2f, 0.0f, 0.0f));
	pSphere->SetRotationalVelocity(vector(0.0f, 0.0f, 2.0f));
	AddPhysicsObject(pSphere);

	/*
	pComposite = AddComposite();

	//pComposite->InitializeOBB();
	//pComposite->InitializeAABB();
	pComposite->InitializeBoundingSphere();

	pComposite->SetMass(1.0f);

	auto pSphere = pComposite->AddSphere(0.25f, 10, 10);
	//pSphere->SetVisible(false);
	pSphere->SetMass(1.0f);
	pSphere->SetPosition(point(-1.0f, 0.0f, 0.0f));
	//pSphere->SetVelocity(vector(0.25f, 0.0f, 0.0f));

	/*
	pSphere = pComposite->AddSphere(0.2f, 10, 10);
	pSphere->SetMass(1.0f);
	//pSphere->SetVelocity(vector(0.0f, 0.25f, 0.0f));

	pSphere = pComposite->AddSphere(0.25f, 10, 10);
	pSphere->SetMass(1.0f);
	pSphere->SetPosition(point(2.0f, 0.0f, 0.0f));
	//pSphere->SetVelocity(vector(-0.25f, 0.0f, 0.0f));
	//*/
	
	/*
	auto pCompositeChild = pComposite->AddComposite();
	pCompositeChild->InitializeOBB();
	//pCompositeChild->InitializeAABB();
	pCompositeChild->SetMass(1.0f);

	pSphere = pCompositeChild->AddSphere(0.1f, 10, 10);
	pSphere->SetMass(1.0f);
	pSphere->SetPosition(point(-0.5f, 0.0f, 0.0f));

	pSphere = pCompositeChild->AddSphere(0.1f, 10, 10);
	pSphere->SetMass(1.0f);
	pSphere->SetPosition(point(0.5f, 0.0f, 0.0f));
	pCompositeChild->SetPosition(point(1.0f, 0.0f, 0.0f));
	//pCompositeChild->SetRotationalVelocity(vector(0.0f, 0.0f, 0.25f));
	//*/
	
	//pComposite->SetPosition(point(1.0f, 0.0f, 0.0f));
	//pComposite->RotateZByDeg(45.0f);
	//pComposite->SetVelocity(point(0.2f, 0.0f, 0.0f));
	//pComposite->SetRotationalVelocity(vector(0.0f, 0.0f, 0.25f));

	//AddPhysicsObject(pComposite);

	/*
	pComposite = AddModel(L"\\Models\\Boar\\boar-obj.obj", nullptr, point(0.0f, 0.0f, 0.0f), 0.15f, vector(0.0f, 0.0f, 0.0f));
	pComposite->SetMass(1.0f);
	AddPhysicsObject(pComposite);
	//*/

	//*/

	// Volume vs Volume edge edge
	// edge to edge
	/*
	pVolume = AddVolume(0.5f);
	pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
	//pVolume->RotateYByDeg(45.0f);
	pVolume->RotateZByDeg(15.0f);
	pVolume->SetMass(1.0f);
	pVolume->SetVelocity(-1.0f, 0.0f, 0.0f);
	AddPhysicsObject(pVolume);

	pVolume = AddVolume(0.5f);
	pVolume->SetPosition(point(2.0f, 0.0f, 0.0f));
	pVolume->SetMass(1.0f);
	pVolume->RotateYByDeg(135.0f);
	//pVolume->RotateZByDeg(45.0f);
	AddPhysicsObject(pVolume);
	*/
	
	// Edge Edge case 2
	/*
	pVolume = AddVolume(1.0f);
	pVolume->SetPosition(point(3.0f, 0.0f, 0.0f));
	pVolume->RotateYByDeg(45.0f);
	pVolume->RotateZByDeg(55.0f);
	pVolume->SetMass(1.0f);
	pVolume->SetVelocity(-0.5f, 0.0f, 0.0f);
	AddPhysicsObject(pVolume);

	pVolume = AddVolume(1.0f);
	pVolume->SetPosition(point(0.0f, 0.0f, 0.0f));
	pVolume->SetMass(1.0f);
	pVolume->RotateYByDeg(145.0f);
	//pVolume->RotateZByDeg(45.0f);
	AddPhysicsObject(pVolume);
	*/

	/*
	pVolume = AddVolume(0.5f);
	pVolume->SetPosition(point(-1.0f, 0.0f, 0.0f));
	//pVolume->RotateYByDeg(45.0f);
	pVolume->RotateZByDeg(35.0f);
	pVolume->SetMass(1.0f);
	AddPhysicsObject(pVolume);

	pVolume = AddVolume(0.5f);
	pVolume->SetPosition(point(-2.0f, 0.0f, 0.0f));
	pVolume->SetMass(1.0f);
	pVolume->RotateYByDeg(45.0f);
	//pVolume->RotateZByDeg(45.0f);
	AddPhysicsObject(pVolume);

	pVolume = AddVolume(0.5f);
	pVolume->SetPosition(point(-3.0f, 0.0f, 0.0f));
	//pVolume->RotateYByDeg(45.0f);
	pVolume->RotateZByDeg(55.0f);
	pVolume->SetMass(1.0f);
	AddPhysicsObject(pVolume);
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

Error:
	return r;
}

RESULT DreamTestApp::Update(void) {
	RESULT r = R_PASS;

	CR(m_pPhysicsEngineTestSuite->UpdateAndRunTests((void*)(this)));

Error:
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
