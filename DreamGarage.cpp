#include "DreamGarage.h"
#include <string>

//quad *g_pQuad;

light *g_pLight = nullptr;

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// Add lights

	///*
	AddLight(LIGHT_POINT, 1.0f, point(0.0f, 3.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	//*/


	float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.3f;
	//AddLight(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0f)), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	//AddLight(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0f)), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	//

	///*
	//texture *pBumpTexture = MakeTexture(L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	//texture *pBumpTexture2 = MakeTexture(L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);

	//texture *pColorTexture = MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	//texture *pColorTexture2 = MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	//texture *pColorTextureCobble = MakeTexture(L"cobblestone_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	//texture *pHeightTextureCobble = MakeTexture(L"cobblestone_height.jpg", texture::TEXTURE_TYPE::TEXTURE_HEIGHT);
	//*/

	// TODO: Combine this into one call
	texture *pCubeMap = MakeTexture(L"HornstullsStrand2", texture::TEXTURE_TYPE::TEXTURE_CUBE);
	skybox *pSkybox = AddSkybox();
	pSkybox->SetCubeMapTexture(pCubeMap);

	//quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	//pQuad->SetColorTexture(pColorTextureCobble);
	//pQuad->SetBumpTexture(pBumpTexture);


	//m_pSphere = AddSphere(0.5f, 30, 30, color(COLOR_RED));

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

	/*
	volume *pVolume = AddVolume(1.0f);
	pVolume->translateX(5.0f);
	*/

	/*
	// TODO: All this should go into Model
	std::vector<vertex> v;
	*/
	// TODO: Should move to using path manager
	PathManager* pMgr = PathManager::instance();
	wchar_t*	path;
	pMgr->GetCurrentPath((wchar_t*&)path);
	std::wstring objFile(path);

	/*
	AddModel(objFile, L"\\Models\\face2\\untitled.obj",
	MakeTexture(L"..\\Models\\face2\\faceP.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
	point(-4.5f, -4.8f, 10.0f),
	0.2f,
	0);
	*/

	AddModel(objFile, L"\\Models\\Bear\\bear-obj.obj",
		nullptr,
		point(-4.5f, -4.8f - 2.6f, 0.0f),
		0.1f,
		100.0f);

	AddModel(objFile, L"\\Models\\Boar\\boar-obj.obj",
		nullptr,
		point(-3.0f, -4.2f - 2.5f, 0.0f),
		0.15f,
		4.0f);

	AddModel(objFile, L"\\Models\\Dwarf\\dwarf_2_low.obj",
		//new OGLTexture(this, L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		MakeTexture(L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(0.0f, -4.9f - 2.1f, 0.0f),
		20.0f);

	AddModel(objFile, L"\\Models\\car\\untitled.obj",
		nullptr,
		point(10.0f, -3.7f - 4.0f, -1.0f - 6.0f),
		0.015f,
		80.1f);// ->SetOrientation(quaternion(vector(1.0, 1.0, 1.0)))->RotateZByDeg(90);

	AddModel(objFile, L"\\Models\\toys\\poly.obj",
		MakeTexture(L"..\\Models\\toys\\lego.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(-4.5f, -4.8f - 2.6f, 4.0f),
		1.0f,
		100.0f);

	AddModel(objFile, L"\\Models\\terrain\\untitled.obj",
		MakeTexture(L"..\\Models\\terrain\\floor.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(0.0f, -10.0f, 0.0f),
		7.0f,
		0);

//Error:
	return r;
}

RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;

	/*
	// Update stuff ...
	if (m_pSphere != nullptr) {
		//m_pSphere->translateY(0.0005f);
		//m_pSphere->RotateBy(0.001f, 0.002f, 0.001f);
		m_pSphere->RotateYBy(0.001f);
		for (auto &childObj : m_pSphere->GetChildren()) {
			childObj->RotateYBy(0.001f);
		}
	}

	m_pSphere->translateX(0.001f);

	//g_pLight->RotateLightDirectionYAxis(0.001f);
	//g_pLight->RotateLightDirectionXAxis(0.0005f * 1.3f);
	*/
	//Error:
	return r;
}
