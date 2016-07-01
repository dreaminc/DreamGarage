#include "DreamGarage.h"

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// Add lights

	//AddLight(LIGHT_POINT, 1.0f, point(0.0f, 3.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	
	float lightHeight = 5.0f, lightSpace = 5.0f, lightIntensity = 1.0f;
	AddLight(LIGHT_POINT, lightIntensity, point(lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_BLUE), color(COLOR_BLUE), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0)), color(COLOR_RED), color(COLOR_RED), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, point(0.0f, lightHeight, lightSpace), color(COLOR_GREEN), color(COLOR_GREEN), vector::jVector(-1.0f));
	

	texture *pBumpTexture = MakeTexture(L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	texture *pBumpTexture2 = MakeTexture(L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);

	texture *pColorTexture = MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture2 = MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	// TODO: Combine this into one call
	texture *pCubeMap = MakeTexture(L"HornstullsStrand2", texture::TEXTURE_TYPE::TEXTURE_CUBE);
	skybox *pSkybox = AddSkybox();
	pSkybox->SetCubeMapTexture(pCubeMap);

	/*
	model* pModel = AddModel(L"chainsaw_free.obj");
	pModel->SetColorTexture(pColorTexture);
	pModel->SetBumpTexture(pBumpTexture);
	*/

	/*
	m_pSphere = AddSphere(1.5f, 40, 40);
	m_pSphere->SetColorTexture(pColorTexture);
	m_pSphere->SetBumpTexture(pBumpTexture);

	std::shared_ptr<sphere> pSphere2(MakeSphere(0.5f, 40, 40));
	pSphere2->SetColorTexture(pColorTexture2);
	pSphere2->SetBumpTexture(pBumpTexture2);
	pSphere2->translateX(3.0f);
	m_pSphere->AddChild(pSphere2);


	std::shared_ptr<sphere> pSphere3(MakeSphere(0.25f, 40, 40));
	pSphere3->SetColorTexture(pColorTexture2);
	pSphere3->SetBumpTexture(pBumpTexture2);
	pSphere3->translateX(2.0f);
	pSphere2->AddChild(pSphere3);
	*/

	//*/

	/*
	volume *pVolume = AddVolume(1.0f);
	pVolume->translateX(5.0f);
	*/


Error:
	return r;
}

RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;

	// Update stuff ...

	if (m_pSphere != nullptr) {
		m_pSphere->translateX(0.01f);
		m_pSphere->RotateBy(0.01f, 0.02f, 0.01f);
	}

Error:
	return r;
}