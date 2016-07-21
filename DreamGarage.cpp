#include "DreamGarage.h"

//quad *g_pQuad;

light *g_pLight = nullptr;

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// Add lights

	///*
	//AddLight(LIGHT_POINT, 1.0f, point(0.0f, 5.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	// TODO: Special lane for global light

	//vector lightdir = vector(0.0f, 1.0f, -0.5f);
	vector lightdir = vector(0.0f, -1.0f, 0.5f);
	lightdir.Normalize();

	/*
	float lightdistance = 10.0f;
	point lightpoint = -1.0f * lightdir * lightdistance;
	lightpoint.w() = 1.0f;
	//*/

	point lightpoint = point(0.0f, 10.0f, 0.0f);
	g_pLight = AddLight(LIGHT_DIRECITONAL, 1.0f, lightpoint, color(COLOR_WHITE), color(COLOR_WHITE), lightdir);
	g_pLight->EnableShadows();
	//*/

	/*
	float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.0f;
	AddLight(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0f)), color(COLOR_BLUE), color(COLOR_BLUE), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0f)), color(COLOR_RED), color(COLOR_RED), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_GREEN), color(COLOR_GREEN), vector::jVector(-1.0f));
	//*/

	///*
	texture *pBumpTexture = MakeTexture(L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	texture *pBumpTexture2 = MakeTexture(L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);

	texture *pColorTexture = MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture2 = MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	texture *pColorTextureCobble = MakeTexture(L"cobblestone_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pHeightTextureCobble = MakeTexture(L"cobblestone_height.jpg", texture::TEXTURE_TYPE::TEXTURE_HEIGHT);
	//*/

	// TODO: Combine this into one call
	///*
	texture *pCubeMap = MakeTexture(L"HornstullsStrand2", texture::TEXTURE_TYPE::TEXTURE_CUBE);
	skybox *pSkybox = AddSkybox();
	pSkybox->SetCubeMapTexture(pCubeMap);
	//*/

	quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	pQuad->MoveTo(point(0.0f, -1.5f, 0.0f));
	pQuad->SetColorTexture(pColorTextureCobble);
	//pQuad->SetBumpTexture(pBumpTexture);

	//quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	//pQuad->SetColorTexture(pColorTextureCobble);

	//quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	//pQuad->SetColorTexture(pColorTextureCobble);

	HUD_OUT("Hello World");

	//pQuad->SetBumpTexture(pBumpTexture);

	//quad *pQuad = AddQuad(10.0f, 10.0f, 100, 100);
	//pQuad->MoveTo(0.0f, -1.0f, 0.0f);

	m_pSphere = AddSphere(0.5f, 30, 30, color(COLOR_RED));
	m_pSphere->MoveTo(0.0f, 2.0f, 0.0f);

	/*
	sphere *pSphere2 = AddSphere(0.5f, 40, 40);
	pSphere2->MoveTo(0.0f, -1.0f, 0.0f);
	*/

	/*
	model* pModel = AddModel(L"\\Models\\Bear\\bear-obj.obj");
	pModel->SetColorTexture(pColorTexture);
	pModel->SetBumpTexture(pBumpTexture);
	pModel->Scale(0.1f);
	*/

	/*
	m_pSphere = AddSphere(0.5f, 40, 40);
	m_pSphere->SetColorTexture(pColorTexture);
	m_pSphere->SetBumpTexture(pBumpTexture);
	//*/

	/*
	sphere *pSphere2 = AddSphere(0.5f, 40, 40);
	pSphere2->SetColorTexture(pColorTexture2);
	pSphere2->SetBumpTexture(pBumpTexture2);

	pSphere2->translateX(5.0f);
	//*/

	
	volume *pVolume = AddVolume(0.5f);
	pVolume->MoveTo(-1.0f, 1.0f, 0.0f);

	/*
	// TODO: All this should go into Model
	std::vector<vertex> v;

	// TODO: Should move to using path manager
	PathManager* pMgr = PathManager::instance();
	wchar_t*	path;
	pMgr->GetCurrentPath((wchar_t*&)path);
	std::wstring objFile(path);

	AddModel(objFile, L"\\Models\\Bear\\bear-obj.obj",
		nullptr,
		point(-4.5f, -4.8f, 0.0f),
		0.1f,
		1.0f);

	AddModel(objFile, L"\\Models\\Boar\\boar-obj.obj",
		nullptr,
		point(-3.0f, -4.2f, 0.0f),
		0.15f,
		4.0f);

	AddModel(objFile, L"\\Models\\Dwarf\\dwarf_2_low.obj",
		//new OGLTexture(this, L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		MakeTexture(L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(0.0f, -4.9f, 0.0f),
		20.0f);

	AddModel(objFile, L"\\Models\\car\\untitled.obj",
		nullptr,
		point(6.0f, -3.7f, -1.0f),
		0.015f,
		1.0f);

	AddModel(objFile, L"\\Models\\table\\untitled.obj",
		nullptr,
		point(0.0f, -13.0f, 0.0f),
		10.0f,
		3.14f);
	*/


//Error:
	return r;
}

RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;

	// Update stuff ...

	m_pSphere->translateX(0.001f);

	//g_pLight->RotateLightDirectionYAxis(0.001f);
	//g_pLight->RotateLightDirectionXAxis(0.0005f * 1.3f);

//Error:
	return r;
}