#include "DreamGarage.h"
#include <string>

//quad *g_pQuad;

light *g_pLight = nullptr;

#include "Cloud/CloudController.h"
#include "Cloud/Message/UpdateHeadMessage.h"
#include "Cloud/Message/UpdateHandMessage.h"

// TODO: Should this go into the DreamOS side?
RESULT DreamGarage::InitializeCloudControllerCallbacks() {
	RESULT r = R_PASS;

//	CloudController::HandleHeadUpdateMessageCallback fnHeadUpdateMessageCallback = static_cast<CloudController::HandleHeadUpdateMessageCallback>(std::bind(&DreamGarage::HandleUpdateHeadMessage, this, std::placeholders::_1, std::placeholders::_2));

	CR(RegisterHeadUpdateMessageCallback(std::bind(&DreamGarage::HandleUpdateHeadMessage, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterHandUpdateMessageCallback(std::bind(&DreamGarage::HandleUpdateHandMessage, this, std::placeholders::_1, std::placeholders::_2)));

Error:
	return r;
}

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// TODO: This should go into an "initialize" function
	InitializeCloudControllerCallbacks();

	// Add lights

	///*
	//AddLight(LIGHT_POINT, 1.0f, point(0.0f, 5.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	// TODO: Special lane for global light

	//vector lightdir = vector(0.0f, 1.0f, -0.5f);
	vector lightdir = vector(0.7f, -0.5f, -0.6f);
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
/*


	quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	pQuad->MoveTo(point(0.0f, -1.5f, 0.0f));
	pQuad->SetColorTexture(pColorTextureCobble);
	pQuad->translateY(-2.0f);
	*/
	//pQuad->SetBumpTexture(pBumpTexture);

	//quad *pQuad = AddQuad(10.0f, 15.0f, 200, 200, pHeightTextureCobble);
	//pQuad->SetColorTexture(pColorTextureCobble);

	HUD_OUT("Hello World");

	//pQuad->SetBumpTexture(pBumpTexture);
	
	/*
	quad *pQuad = AddQuad(1.0f, 1.0f, 10, 10);
	pQuad->MoveTo(0.0f, 0.0f, 0.0f);
	pQuad->SetBillboard(true);

	quad *pQuad2 = AddQuad(1.0f, 1.0f, 10, 10);
	pQuad2->MoveTo(3.0f, 1.0f, 3.0f);
	pQuad2->SetScaledBillboard(true);
	pQuad2->SetBillboard(true);
	
	//tQuad->SetBillboard(true);
	//*/

/*
	m_pSphere = AddSphere(0.5f, 30, 30, color(COLOR_RED));
	m_pSphere->MoveTo(0.0f, 2.0f, 0.0f);

	//quad *pQuad = AddQuad(10.0f, 10.0f, 100, 100);
	//pQuad->MoveTo(0.0f, -1.0f, 0.0f);

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


	// TODO: Replace with model
	m_pPeerUser = AddVolume(0.25f, 0.5f, 1.0f);
	//m_pPeerUser = AddVolume(1.0f);

	//m_pSphere = AddSphere(1.0f, 30, 30, color(COLOR_RED));

	/*

	///*
	std::shared_ptr<sphere> pSphere2(MakeSphere(0.5f, 40, 40, color(COLOR_BLUE)));
	pSphere2->SetColorTexture(pColorTexture2);
	pSphere2->SetBumpTexture(pBumpTexture2);
	pSphere2->translateX(3.0f);
	m_pSphere->AddChild(pSphere2);


	std::shared_ptr<sphere> pSphere3(MakeSphere(0.25f, 40, 40, color(COLOR_GREEN)));
	pSphere3->SetColorTexture(pColorTexture2);
	pSphere3->SetBumpTexture(pBumpTexture2);
	pSphere3->translateX(1.0f);
	pSphere2->AddChild(pSphere3);
	//*/
	
	/*
	volume *pVolume = AddVolume(0.5f);
	pVolume->MoveTo(0.0f, 0.0f, 0.0f);
	*/

	// TODO: All this should go into Model
	std::vector<vertex> v;

	// TODO: Should move to using path manager
	PathManager* pMgr = PathManager::instance();
	wchar_t*	path;
	pMgr->GetCurrentPath((wchar_t*&)path);
	std::wstring objFile(path);

	/*
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

	
	*/

	/*
	AddModel(objFile, L"\\Models\\table\\untitled.obj",
		nullptr,
		point(0.0, 13.0, 0.0),
		10.0,
		3.14f);
	//*/


//Error:
	return r;
}

RESULT DreamGarage::SendHeadPosition() {
	RESULT r = R_PASS;

	point ptPosition = GetCameraPosition();
	quaternion qOrientation = GetCameraOrientation();

	CR(SendUpdateHeadMessage(NULL, ptPosition, qOrientation));

Error:
	return r;
}




// 30 FPS update
#define UPDATE_COUNT_THROTTLE 90	
#define UPDATE_COUNT_MS ((1000.0f) / UPDATE_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastUpdateTime = std::chrono::system_clock::now();

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
	//*/

	//m_pSphere->translateX(0.001f);

	// TODO: Switch to message queue that runs on own thread
	// for now just throttle it down
	//g_updateCount++;
	//if (g_updateCount != 0 && (g_updateCount % UPDATE_COUNT_THROTTLE) == 0) {

	std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
	if(std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastUpdateTime).count() > UPDATE_COUNT_MS) {
		SendHeadPosition();
		g_lastUpdateTime = timeNow;
	}

	//g_pLight->RotateLightDirectionYAxis(0.001f);
	//g_pLight->RotateLightDirectionXAxis(0.0005f * 1.3f);

	//Error:
	return r;
}

// Cloud Controller
RESULT DreamGarage::HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage) {
	RESULT r = R_PASS;

	CN(pUpdateHeadMessage);

	//DEBUG_LINEOUT("HandleUpdateHeadMessage");
	//pUpdateHeadMessage->PrintMessage();

	//m_pSphere->SetPosition(pUpdateHeadMessage->GetPosition());

	m_pPeerUser->SetPosition(pUpdateHeadMessage->GetPosition());
	m_pPeerUser->SetOrientation(pUpdateHeadMessage->GetOrientation().GetReverse());

Error:
	return r;
}

RESULT DreamGarage::HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage) {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("HandleUpdateHandMessage");
	pUpdateHandMessage->PrintMessage();

//Error:
	return r;
}
