#include "DreamGarage.h"
#include "Logger/Logger.h"
#include <string>
#include <array>

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

light *g_pLight = nullptr;
light *g_pLight2 = nullptr;

#include "Cloud/CloudController.h"
#include "Cloud/Message/UpdateHeadMessage.h"
#include "Cloud/Message/UpdateHandMessage.h"
#include "Cloud/Message/AudioDataMessage.h"

#include "DreamGarage/DreamContentView.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramEnvironmentObjects.h"

#include "PhysicsEngine/CollisionManifold.h"

// TODO: Should this go into the DreamOS side?
RESULT DreamGarage::InitializeCloudControllerCallbacks() {
	RESULT r = R_PASS;

//	CloudController::HandleHeadUpdateMessageCallback fnHeadUpdateMessageCallback = static_cast<CloudController::HandleHeadUpdateMessageCallback>(std::bind(&DreamGarage::HandleUpdateHeadMessage, this, std::placeholders::_1, std::placeholders::_2));

	CR(RegisterPeersUpdateCallback(std::bind(&DreamGarage::HandlePeersUpdate, this, std::placeholders::_1)));
	CR(RegisterDataMessageCallback(std::bind(&DreamGarage::HandleDataMessage, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterHeadUpdateMessageCallback(std::bind(&DreamGarage::HandleUpdateHeadMessage, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterHandUpdateMessageCallback(std::bind(&DreamGarage::HandleUpdateHandMessage, this, std::placeholders::_1, std::placeholders::_2)));
	CR(RegisterAudioDataCallback(std::bind(&DreamGarage::HandleAudioData, this, std::placeholders::_1, std::placeholders::_2)));

Error:
	return r;
}

// Rotation testing TODO: move to separate app
/*
volume *pVolume;
composite *pVolume2;
volume *pVolume3;
volume *pVolume4;
std::vector<composite*> bears;
//*/

RESULT DreamGarage::ConfigureSandbox() {
	RESULT r = R_PASS;

	SandboxApp::configuration sandboxconfig;
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fUseLeap = true;
	SetSandboxConfiguration(sandboxconfig);

	//Error:
	return r;
}

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// TODO: This should go into an "initialize" function
	InitializeCloudControllerCallbacks();

	// Keyboard
	RegisterSubscriber(SenseVirtualKey::SVK_ALL, this);
	RegisterSubscriber(SENSE_TYPING_EVENT_TYPE::CHARACTER_TYPING, this);

	// Controller
	RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_MENU_UP, this);
	RegisterSubscriber(SENSE_CONTROLLER_EVENT_TYPE::SENSE_CONTROLLER_TRIGGER_UP, this);

	// Console
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamApp, this);
	
	m_browsers.Init(AddComposite());

	for (auto x : std::array<int, 8>()) {
		user* pNewUser = AddUser();
		pNewUser->SetVisible(false);
		m_usersPool.push_back(pNewUser);
	}
	
	AddSkybox();

	g_pLight2 = AddLight(LIGHT_DIRECITONAL, 2.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
	g_pLight2->EnableShadows();

	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	point sceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector sceneDirection = vector(0.0f, 0.0f, 0.0f);

	AddModel(L"\\Models\\FloatingIsland\\env.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	composite* pRiver = AddModel(L"\\Models\\FloatingIsland\\river.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);
	composite* pClouds = AddModel(L"\\Models\\FloatingIsland\\clouds.obj",
		nullptr,
		sceneOffset,
		sceneScale,
		sceneDirection);

	pClouds->SetMaterialAmbient(0.8f);

	std::shared_ptr<OGLObj> pOGLObj = std::dynamic_pointer_cast<OGLObj>(pRiver->GetChildren()[0]);
	if (pOGLObj != nullptr) {
		pOGLObj->SetOGLProgramPreCallback(
			[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff pre
				OGLProgramEnvironmentObjects *pOGLEnvironmentProgram = dynamic_cast<OGLProgramEnvironmentObjects*>(pOGLProgram);
				if (pOGLEnvironmentProgram != nullptr) {
					pOGLEnvironmentProgram->SetRiverAnimation(true);
				}
				return R_PASS;
			}
		);

		pOGLObj->SetOGLProgramPostCallback(
			[](OGLProgram* pOGLProgram, void *pContext) {
				// Do some stuff post
			
				OGLProgramEnvironmentObjects *pOGLEnvironmentProgram = dynamic_cast<OGLProgramEnvironmentObjects*>(pOGLProgram);
				if (pOGLEnvironmentProgram != nullptr) {
					pOGLEnvironmentProgram->SetRiverAnimation(false);
				}
				return R_PASS;
			}
		);
	}

#ifdef TESTING
// Test Scene
// 
	// Add lights
///*

/*
	AddLight(LIGHT_POINT, lightIntensity, point(-lightSpace, lightHeight, -(lightSpace / 2.0f)), color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
	AddLight(LIGHT_POINT, lightIntensity, ptLight, color(COLOR_WHITE), color(COLOR_WHITE), vector::jVector(-1.0f));
//*/

	// Add textures
///*
	texture *pBumpTexture = MakeTexture(L"brickwall_bump.jpg", texture::TEXTURE_TYPE::TEXTURE_BUMP);
	texture *pBumpTexture2 = MakeTexture(L"crate_bump.png", texture::TEXTURE_TYPE::TEXTURE_BUMP);

	texture *pColorTexture = MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pColorTexture2 = MakeTexture(L"crate_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	texture *pColorTextureCobble = MakeTexture(L"cobblestone_color.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
	texture *pHeightTextureCobble = MakeTexture(L"cobblestone_height.jpg", texture::TEXTURE_TYPE::TEXTURE_HEIGHT);
	
//	texture *dwarf = MakeTexture(L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR);

	//texture *pColorTextureTest = MakeTexture(L"asymmetrical.png", texture::TEXTURE_TYPE::TEXTURE_COLOR);
//*/

	// Add Flat Objects
/*
	m_pContext = AddFlatContext(8192, 8192);
	std::shared_ptr<quad> background = m_pContext->AddQuad(1.5, 1.5, point(0.0f, 0.0f, 0.5f));
	background->SetColorTexture(pColorTexture);
	std::shared_ptr<text> pText = m_pContext->AddText(L"ArialDistance.fnt", "Hello World", 1.0, true);
	pText->MoveTo(0.5f, 0.5f, 0.0f);
	std::shared_ptr<text> pText2 = m_pContext->AddText(L"ArialDistance.fnt", "Sababa", 1.0, true);
	RenderToTexture(m_pContext); 

	m_pQuad = AddQuad(10.0f, 10.0f);
	m_pQuad->MoveTo(0.0f, 2.0f, 0.0f);
	texture* test = m_pContext->GetFramebuffer()->GetTexture();
	m_pQuad->SetColorTexture(m_pContext->GetFramebuffer()->GetTexture());
	m_pQuad->RotateXByDeg(45.0f);
/*
	FlatContext* pContext2 = AddFlatContext();
	std::shared_ptr<text> pText3 = pContext2->AddText(L"ArialDistance.fnt", "second context", 1.5f, true);

	quad* pQuad2 = AddQuad(5.0f, 5.0f);
	pQuad2->MoveTo(0.0f, 3.0f, 0.0f);
	RenderToTexture(pContext2);
	pQuad2->SetColorTexture(pContext2->GetFramebuffer()->GetTexture());

/*
	for (float x = 0.0f; x < 5.0f; x += 1.0f) {
		std::shared_ptr<quad> pFQuad2 = pContext2->AddQuad(0.25f, 0.25f, point(-x/10.0f, -x/10.0f, x/10.0f));
		pFQuad2->SetColorTexture(pColorTexture);
	}

/*
	FlatContext *pContext = AddFlatContext();
	for (float x = 0.0f; x < 5.0f; x += 1.0f) {
		std::shared_ptr<quad> pFQuad2 = pContext->MakeQuad(x/10.0f, x/10.0f, point(x/10.0f, x/10.0f, x/10.0f));
		pFQuad2->SetColorTexture(pColorTexture);
	}
//*/

	// Add base plane
///*
	quad *pBQuad = AddQuad(10.0f, 20.0f, 200, 200);// , pHeightTextureCobble);
	pBQuad->MoveTo(point(0.0f, -1.5f, 0.0f));
	//pBQuad->SetColorTexture(pColorTextureCobble);
	//pBQuad->SetBumpTexture(pBumpTexture);

// Rotation testing TODO: move to seperate app
/*
	pVolume = AddVolume(0.5f, 1.5f, 0.7f);
	pVolume->MoveTo(point(0.0f, 1.0f, 0.0f));
	
	pVolume->RotateXBy(10000.0f*float(M_PI));
/*
	pVolume2 = AddVolume(0.5f, 1.5f, 0.7f);
	pVolume2->MoveTo(point(0.0f, 2.0f, 0.0f));
	pVolume2->SetColorTexture(dwarf);
	//pVolume2->SetRotateX(float(M_PI) / 2.0f);

	pVolume2 = AddModel(L"\\Models\\Bear\\bear-obj.obj",
		nullptr,
		point(-0.0f, -1.0f, 1.0f),
		0.1f,
		vector(0.0f, 0.0f, 0.0f));

	//pVolume2->RotateXByDeg(90.0f);
	//pVolume2->RotateYBy(2.0f*(float)M_PI_2);
	//pVolume2->RotateZByDeg(90.0f);
	//pVolume2->RotateXByDeg(90.0f);
	//pVolume2->RotateZByDeg(90.0f);
//	pVolume2->RotateYByDeg(45.0f);
	//pVolume2->RotateByDeg(90.0f, 90.0f, 90.0f);
	quaternion_precision pi2 = (quaternion_precision)M_PI_2;
	pVolume2->RotateBy(pi2, pi2, pi2);

	//pVolume2->SetRotate((float)M_PI_2, 0.0f, 0.0f);
	//pVolume2->RotateZByDeg(90.0f);


	pVolume3 = AddVolume(0.5f, 1.5f, 0.7f);
	pVolume3->MoveTo(point(0.0f, 3.0f, 0.0f));

	quaternion q = pVolume3->GetOrientation();
	quaternion_precision x, y, z;
	q.GetEulerAngles(&x, &y, &z);
	quaternion q2 = quaternion::MakeQuaternionWithEuler(x, y, z);

	pVolume4 = AddVolume(0.5f, 1.5f, 0.7f);
	pVolume4->MoveTo(point(0.0f, 4.0f, 0.0f));

	//pVolume2->SetRotateY(2.0f*float(M_PI_4));
	//pVolume2->SetRotateX(1.0f*float(M_PI_4));
	//pVolume2->SetRotateZ(2.0f*float(M_PI_4));
	//pVolume2->SetRotateY(1.0f*float(M_PI_4));

//*/
	
	// Add billboards
/*
	quad *pQuad = AddQuad(1.0f, 1.0f, 10, 10);
	pQuad->MoveTo(1.0f, 1.0f, 0.0f);
	pQuad->SetBillboard(true);

	quad *pQuad2 = AddQuad(1.0f, 1.0f, 10, 10);
	pQuad2->MoveTo(3.0f, 1.0f, 3.0f);
	pQuad2->SetScaledBillboard(true);
	pQuad2->SetBillboard(true);
//*/

	// Add spheres
/*
	m_pSphere = AddSphere(1.0f, 30, 30, color(COLOR_RED));
	m_pSphere->MoveTo(0.0f, 2.0f, 0.0f);

///*
	std::shared_ptr<sphere> pSphere2(MakeSphere(0.5f, 40, 40, color(COLOR_BLUE)));
	//sphere *pSphere2 = AddSphere(0.5f, 40, 40);

	pSphere2->SetColorTexture(pColorTexture2);
	pSphere2->SetBumpTexture(pBumpTexture2);
	pSphere2->translateX(5.0f);
///*
	m_pSphere->AddChild(pSphere2);
//*/

	// Add volumes
/*
	volume *pVolume = AddVolume(1.0f);
	pVolume->translateX(5.0f);
//*/

	// Add models
///*
	AddModel(L"\\Models\\Boar\\boar-obj.obj",
		nullptr,
		point(-3.0f, -4.2f - 2.5f, 0.0f),
		0.15f,
		vector(0.0f, 0.0f, 0.0f));
///*
	AddModel(L"\\Models\\Dwarf\\dwarf_2_low.obj",
		//new OGLTexture(this, L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		MakeTexture(L"..\\Models\\Dwarf\\dwarf_2_1K_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(0.0f, -4.9f - 2.1f, 0.0f),
		0.1f,
		vector(0.0f, 0.0f, 0.0f));

	AddModel(L"\\Models\\car\\untitled.obj",
		nullptr,
		point(10.0f, -3.7f - 4.0f, -1.0f - 6.0f),
		0.015f,
		vector(0.0f, 0.0f, 0.0f));// ->SetOrientation(quaternion(vector(1.0, 1.0, 1.0)))->RotateZByDeg(90);

	AddModel(L"\\Models\\toys\\poly.obj",
		MakeTexture(L"..\\Models\\toys\\lego.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(-4.5f, -4.8f - 2.6f, 4.0f),
		1.0f,
		vector(0.0f, 0.0f, 0.0f));
		
/*
	AddModel(L"\\Models\\terrain\\untitled.obj",
		MakeTexture(L"..\\Models\\terrain\\floor.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR),
		point(0.0f, -10.0f, 0.0f),
		7.0f,
		vector(0.0f, 0.0f, 0.0f));
//*/
#endif // ! TESTING

	m_pIconTexture = std::shared_ptr<texture>(MakeTexture(L"brickwall_color.jpg", texture::TEXTURE_TYPE::TEXTURE_COLOR));

	IconFormat iconFormat;
	LabelFormat labelFormat;
	UIBarFormat barFormat;
	
	m_pDreamUIBar = std::make_shared<DreamUIBar>(this, iconFormat, labelFormat, barFormat);
	CN(m_pDreamUIBar);
	CV(m_pDreamUIBar);

	//Hardcoded menu for now, will be replaced with api requests
	m_menu[""] = { "lorem", "ipsum", "dolor", "sit" };
	m_menu["lorem"] = { "Watch", "Listen", "Play", "Whisper", "Present" };
	m_menu["ipsum"] = { "1", "2", "3" };
	m_menu["Play"] = { "a", "b", "c" };

	m_menuPath = {};

	m_pDreamContentView = LaunchDreamApp<DreamContentView>(this);
	CNM(m_pDreamContentView, "Failed to create dream content view");

	m_pDreamContentView->SetParams(point(0.0f, 2.0f, -2.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

	m_pDreamContentView->SetVisible(false);
	CR(GetCloudController()->RegisterEnvironmentAssetCallback(std::bind(&DreamGarage::HandleOnEnvironmentAsset, this, std::placeholders::_1)));

	m_pDreamContentView->SetFitTextureAspectRatio(true);

Error:
	return r;
}

RESULT DreamGarage::SendHeadPosition() {
	RESULT r = R_PASS;

	point ptPosition = GetCameraPosition();
	quaternion qOrientation = GetCameraOrientation();

	//CR(SendUpdateHeadMessage(NULL, ptPosition, qOrientation));
	CR(BroadcastUpdateHeadMessage(ptPosition, qOrientation));

Error:
	return r;
}

RESULT DreamGarage::SendHandPosition() {
	RESULT r = R_PASS;

	hand *pLeftHand = GetHand(hand::HAND_LEFT);
	hand *pRightHand = GetHand(hand::HAND_RIGHT);

	if (pLeftHand != nullptr) {
		CR(BroadcastUpdateHandMessage(pLeftHand->GetHandState()));
	}

	if (pRightHand != nullptr) {
		CR(BroadcastUpdateHandMessage(pRightHand->GetHandState()));
	}

Error:
	return r;
}


class SwitchHeadMessage : public Message {
public:
	SwitchHeadMessage(long senderUserID, long receiverUserID) :
		Message(senderUserID, 
				receiverUserID, 
				(Message::MessageType)((uint16_t)(Message::MessageType::MESSAGE_CUSTOM) + 1), 
				sizeof(SwitchHeadMessage))
	{
		// empty
	}
};

RESULT DreamGarage::SendSwitchHeadMessage() {
	RESULT r = R_PASS;

	SwitchHeadMessage switchHeadMessage(NULL, NULL);
	//CR(SendDataMessage(NULL, &(switchHeadMessage)));
	CR(BroadcastDataMessage(&(switchHeadMessage)));

Error:
	return r;
}

// Head update time
#define UPDATE_HEAD_COUNT_THROTTLE 90	
#define UPDATE_HEAD_COUNT_MS ((1000.0f) / UPDATE_HEAD_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastHeadUpdateTime = std::chrono::system_clock::now();

// Hands update time
#define UPDATE_HAND_COUNT_THROTTLE 90	
#define UPDATE_HAND_COUNT_MS ((1000.0f) / UPDATE_HAND_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastHandUpdateTime = std::chrono::system_clock::now();


RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;
	
	m_browsers.Update();
	m_pDreamUIBar->Update();

#ifdef TESTING
///*
	// Update stuff ...
	if (m_pSphere != nullptr) {
		//m_pSphere->translateY(0.0005f);
		//m_pSphere->RotateBy(0.001f, 0.002f, 0.001f);
		m_pSphere->RotateYBy(0.001f);
		if (m_pSphere->HasChildren()) {
			for (auto &childObj : m_pSphere->GetChildren()) {
				childObj->RotateYBy(0.001f);
			}
		}
///*
		m_pSphere->translateX(0.001f);
	}
// Rotation testing TODO: move to seperate app
/*
//	pVolume->RotateXBy(0.01f);
//	pVolume->RotateYBy(0.01f);
	quaternion q = pVolume->GetOrientation();
//	DEBUG_LINEOUT("%f %f %f %f", q.w(), q.x(), q.y(), q.z());
//	pVolume->RotateZBy(0.01f);
	tick += 0.002f;
	//pVolume2->SetRotateY(float(3.0f*M_PI_4));
//	pVolume2->SetRotateY(tick);
	//pVolume2->SetRotateZ((float)M_PI_4);
//	pVolume2->SetRotateX(tick*10.0f);
	//pVolume2->SetRotateZ(-1.0f*(float)M_PI_4);
	//pVolume2->SetRotateZ((float)M_PI_4);
//	pVolume2->SetRotateY((float)M_PI_4);
	//pVolume2->SetRotateY(tick);
	//pVolume2->SetRotateX((float)M_PI_4);
	float fpi4 = (float)M_PI_4;

//	pVolume2->RotateXBy(0.002f);
//	pVolume2->RotateZBy(0.002f);
	//pVolume2->SetRotate(0.0f, tick, fpi4);
	//pVolume2->SetRotateX(fpi4);
	//pVolume2->SetRotateY(tick);
	//pVolume2->SetRotateZ(fpi4);
	//pVolume2->SetRotateZ(tick);
	//pVolume2->SetRotateX(10000.0f*float(M_PI));
	//pVolume2->SetRotateZ(-1.0f*(float)M_PI_4);

	//pVolume2->SetRotateZ((float)M_PI_2);
//	pVolume2->SetRotateZ(tick*1.0f);
	//pVolume2->SetRotateX(0.5f);
	//pVolume2->SetRotateZ(0.5f);
//	quaternion q2 = pVolume2->GetOrientation();
	//DEBUG_LINEOUT("%f %f %f %f         %f %f %f %f", q.w(), q.x(), q.y(), q.z(), q2.w(), q2.x(), q2.y(), q2.z());
//	pVolume2->SetRotateZ(tick);
//*/

#endif

	// TODO: Switch to message queue that runs on own thread
	// for now just throttle it down
	//g_updateCount++;
	//if (g_updateCount != 0 && (g_updateCount % UPDATE_COUNT_THROTTLE) == 0) {

	// Head update
	// TODO: this should go up into DreamOS or even sandbox
	///*
	std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

	if(std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastHeadUpdateTime).count() > UPDATE_HEAD_COUNT_MS) {
		SendHeadPosition();
		g_lastHeadUpdateTime = timeNow;
	}

	// Hand update
	// TODO: this should go up into DreamOS or even sandbox
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastHandUpdateTime).count() > UPDATE_HAND_COUNT_MS) {
		SendHandPosition();
		g_lastHandUpdateTime = timeNow;
	}
	//*/
	
	/*
	static quaternion_precision theta = 0.0f;
	quaternion qOrientation;
	//qOrientation = pUpdateHeadMessage->GetOrientation();

	qOrientation = quaternion((quaternion_precision)0.0f, vector::kVector(1.0f));
	//qOrientation.RotateY((theta += 0.0005f));
	qOrientation.RotateY(((quaternion_precision)(M_PI)));
	
	DEBUG_LINEOUT_RETURN("theta %.2f", theta);

	m_pPeerUser->SetOrientation(qOrientation);
	*/

	/*
	for(int i = 0; i < 4; i++)
		qOrientation *= quaternion((quaternion_precision)(M_PI_4/2.0f), vector::jVector(1.0f));
	*/

	/*
	quaternion qOrientation = quaternion((quaternion_precision)0.0f, vector::kVector(1.0f));;
	qOrientation.RotateX(((quaternion_precision)(M_PI * 1.5f)));
	m_pPeerUser->SetOrientation(qOrientation);
	m_pPeerUser->SetPosition(point(0.0f, 2.0f, 0.0f));
	//*/

	//m_pPeerUser->SetOrientation(quaternion((quaternion_precision)0.0f, vector::kVector(1.0f)));
	//m_pPeerUser->RotateYByDeg(180.0f);

	//qOrientation.Reverse();

	//g_pLight->RotateLightDirectionYAxis(0.001f);
	//g_pLight->RotateLightDirectionXAxis(0.0005f * 1.3f);

//Error:
	return r;
}

RESULT DreamGarage::SetRoundtablePosition(float angle) {
	RESULT r = R_PASS;

	camera *pCamera = GetCamera();
	float radius = 2.0f;

	float ptX = -radius*sin(angle*M_PI / 180.0f);
	float ptZ =  radius*cos(angle*M_PI / 180.0f);
	
	point offset = point(ptX, 0.0f, ptZ);
	pCamera->SetPosition(offset);

	if (!pCamera->HasHMD()) {
		pCamera->RotateYByDeg(angle);
	}
	else {
		quaternion qOffset = quaternion::MakeQuaternionWithEuler(0.0f, angle * M_PI / 180.0f, 0.0f);
		pCamera->SetOffsetOrientation(qOffset);
	}

	return r;
}

// Cloud Controller
RESULT DreamGarage::HandlePeersUpdate(long index) {
	RESULT r = R_PASS;

	if (m_isSeated) {
		LOG(INFO) << "HandlePeersUpdate already seated" << index;
		return R_PASS;
	}

	LOG(INFO) << "HandlePeersUpdate " << index;
	
	OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(index)).c_str());

	if (!m_isSeated) {

		// full circle configuration
		/*
		std::vector<int> m_seatOrdering = { 4, 0, 6, 2, 5, 7, 1, 3 };
		float m_initialAngle = 0.0f;
		float m_differenceAngle = 45.0f;
		//*/

		// seating should be separate from DreamGarage, so naming these 
		// as if they were member variables for now
		// semi circle configuration
		std::vector<int> m_seatLookup = { 4, 1, 3, 2, 5, 0 };
		float m_initialAngle = 90.0f;
		float m_differenceAngle = -30.0f;

		CB(index < m_seatLookup.size());

		SetRoundtablePosition(m_initialAngle + (m_differenceAngle * m_seatLookup[index]));

		m_isSeated = true;
	}

Error:
	return r;
}

RESULT DreamGarage::HandleDataMessage(long senderUserID, Message *pDataMessage) {
	RESULT r = R_PASS;
	LOG(INFO) << "data received";

	if (pDataMessage) {
		std::string st((char*)pDataMessage);
		st = "<- " + st;
		HUD_OUT(st.c_str());
	}

	/*
	Message::MessageType switchHeadModelMessage = (Message::MessageType)((uint16_t)(Message::MessageType::MESSAGE_CUSTOM) + 1);

	if (pDataMessage->GetType() == switchHeadModelMessage) {
		HUD_OUT("Other user changed the head model");
		CR(m_pPeerUser->SwitchHeadModel());
	}

Error:
*/
	return r;
}

user*	DreamGarage::ActivateUser(long userId) {
	if (m_peerUsers.find(userId) == m_peerUsers.end()) {
		if (m_usersPool.empty()) {
			LOG(ERROR) << "cannot activate a new user, no reserved users exist";
			return nullptr;
		}

		m_peerUsers[userId] = m_usersPool.back();
		m_usersPool.pop_back();

		if (m_peerUsers[userId] != nullptr) {
			user *u = m_peerUsers[userId];
			m_peerUsers[userId]->SetVisible();
			m_peerUsers[userId]->Activate();
		}
	}

	return m_peerUsers[userId];
}

RESULT DreamGarage::HandleUpdateHeadMessage(long senderUserID, UpdateHeadMessage *pUpdateHeadMessage) {
	RESULT r = R_PASS;

	user* pUser = ActivateUser(senderUserID);

	point headPos = pUpdateHeadMessage->GetPosition();

	std::string st = "pos" + std::to_string(senderUserID);

	WCN(pUser);

	quaternion qOrientation = pUpdateHeadMessage->GetOrientation();

	pUser->GetHead()->SetPosition(headPos);

	OVERLAY_DEBUG_SET(st, (st + "=" + std::to_string(headPos.x()) + "," + std::to_string(headPos.y()) + "," + std::to_string(headPos.z())).c_str());

	pUser->GetHead()->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamGarage::HandleUpdateHandMessage(long senderUserID, UpdateHandMessage *pUpdateHandMessage) {
	RESULT r = R_PASS;

	//DEBUG_LINEOUT("HandleUpdateHandMessage");
	//pUpdateHandMessage->PrintMessage();

	user* pUser = ActivateUser(senderUserID);

	hand::HandState handState;

	WCN(pUser);

	handState = pUpdateHandMessage->GetHandState();
	pUser->UpdateHand(handState);

Error:
	return r;
}

RESULT DreamGarage::HandleAudioData(long senderUserID, AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	user* pUser = ActivateUser(senderUserID);

	WCN(pUser);

	auto msg = pAudioDataMessage->GetAudioData();

	size_t size = msg.number_of_channels * msg.number_of_frames;
	float average = 0;

	for (int i = 0; i < size; ++i) {
		int16_t val = *(static_cast<const int16_t*>(msg.audio_data) + i);
		if (abs(val) > 10)
			average += abs(val);
	}

	float mouthScale = average / size / 1000.0f;

	if (mouthScale > 1.0f) mouthScale = 1.0f;
	if (mouthScale < 0.1f) mouthScale = 0.0f;

	pUser->UpdateMouth(mouthScale);

Error:
	return r;
}

RESULT DreamGarage::HandleOnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (m_pDreamContentView != nullptr) {
		m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset);
		m_pDreamContentView->SetVisible(true);
	}

	return r;
}

RESULT DreamGarage::Notify(SenseKeyboardEvent *kbEvent)  {
	RESULT r = R_PASS;

//Error:
	return r;
}

RESULT DreamGarage::Notify(SenseTypingEvent *kbEvent) {
	RESULT r = R_PASS;

	if (kbEvent->KeyState != 0) {
		m_browsers.OnKey(kbEvent->KeyCode, kbEvent->u16character);
	}

	//Error:
	return r;
}

RESULT DreamGarage::Notify(SenseControllerEvent *event) {
	RESULT r = R_PASS;

	SENSE_CONTROLLER_EVENT_TYPE eventType = event->type;
	OVERLAY_DEBUG_SET("event", "none");

	if (event->state.type == CONTROLLER_RIGHT) {
		// TODO:  soon this code will be replaced with api requests, 
		// as opposed to accessing the hardcoded local data structures
		if (eventType == SENSE_CONTROLLER_TRIGGER_UP) {
			OVERLAY_DEBUG_SET("event", "trigger up");
			CR(m_pDreamUIBar->HandleTriggerUp());
		}
		else if (eventType == SENSE_CONTROLLER_MENU_UP) {
			OVERLAY_DEBUG_SET("event", "menu up");
			CR(m_pDreamUIBar->HandleMenuUp());
		}
	}
Error:
	return r;
}

RESULT DreamGarage::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("list") == 0) {
		HUD_OUT("<blank>");
	}

	if (event->GetArg(1).compare("cef") == 0) {
		if (event->GetArg(2).compare("new") == 0) {
			// defaults
			std::string url{ "www.dreamos.com" };
			unsigned int width = 800;
			unsigned int height = 600;

			if (event->GetArg(3) != "")
				url = event->GetArg(3);
			if (event->GetArg(4) != "")
				width = std::stoi(event->GetArg(4));
			if (event->GetArg(5) != "")
				height = std::stoi(event->GetArg(5));

			m_browsers.CreateNewBrowser(width, height, url);
		}
		else {
			auto browser = m_browsers.GetBrowser(event->GetArg(2));

			if (!browser) {
				HUD_OUT("browser id does not exist");
			}
			else {
				if (event->GetArg(3).compare("type") == 0) {
					browser->SendKeySequence(event->GetArg(4));
				}
				else if (event->GetArg(3).compare("control") == 0) {
					m_browsers.SetKeyFocus(event->GetArg(2));
					HUD_OUT(("controling browser " + event->GetArg(2) + " (hit 'esc' to release control)").c_str());
				}
				else {
					browser->LoadURL(event->GetArg(3));
				}
			}
		}
	}

	// app ui add <title> - adds ui menu item to the current menu layer
	// app ui remove - removes the last menu item
	// app ui list - lists current menu items in menu layer
	// TODO: these events still use the hardcoded menu/path
	if (event->GetArg(1).compare("ui") == 0) {
		if (event->GetArg(2).compare("add") == 0) {
			if (!m_menuPath.empty()) {
				m_menu[m_menuPath.top()].emplace_back(event->GetArg(3));
				UILayerInfo info;
				info.labels = m_menu[m_menuPath.top()];
				info.labels.emplace_back(m_menuPath.top());
				for (size_t i = 0; i < info.labels.size(); i++) {
					info.icons.emplace_back(m_pIconTexture);
				}
				HUD_OUT(("added item " + event->GetArg(3)).c_str());
				m_pDreamUIBar->UpdateCurrentUILayer(info);
			}
		}
		else if (event->GetArg(2).compare("remove") == 0) {
			if (!m_menuPath.empty() && m_menu[m_menuPath.top()].size() > 0) {
				HUD_OUT(("removed item " + m_menu[m_menuPath.top()].back()).c_str());
				m_menu[m_menuPath.top()].pop_back();
				UILayerInfo info;
				info.labels = m_menu[m_menuPath.top()];
				info.labels.emplace_back(m_menuPath.top());
				for (size_t i = 0; i < info.labels.size(); i++) {
					info.icons.emplace_back(m_pIconTexture);
				}
				m_pDreamUIBar->UpdateCurrentUILayer(info);
			}
		}
		else if (event->GetArg(2).compare("list") == 0) {
			HUD_OUT(("current menu: " + m_menuPath.top()).c_str());
			for (auto& s : m_menu[m_menuPath.top()]) {
				HUD_OUT(("\t + " + s).c_str());
			}
		}
	}

	return r;
}


// Browsers

void Browsers::Init(composite* composite) {
	m_composite = composite;
	m_BrowserService = WebBrowser::CreateNewCefBrowserService();
}

void Browsers::Update() {
	for (auto& b : m_Browsers) {
		// TODO: optimize with actual dirty rects copy
		if (b.second.controller->PollNewDirtyFrames([&](unsigned char *output, unsigned int width, unsigned int height, unsigned int left, unsigned int top, unsigned int right, unsigned int bottom) -> bool {
			b.second.texture->Update(output, width, height, texture::PixelFormat::BGRA);
			// poll whole frame and stop iterations
			return false;
		})) {
		}
	}
}

std::string Browsers::CreateNewBrowser(unsigned int width, unsigned int height, const std::string& url) {
	static int id = 0;
	id++;

	Browser browser{
		m_BrowserService->CreateNewWebBrowser(url, width, height),
		nullptr,
		nullptr
	};

	if (browser.controller == nullptr) {
		return "";
	}

	std::vector<unsigned char>	buffer(width * height * 4, 0);

	browser.texture = m_composite->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, width, height, texture::PixelFormat::RGBA, 4, &buffer[0], width * height * 4);

	float quadWidth	 = 4.0f * width / 512;
	float quadHeight = 4.0f * width / 512;

	std::shared_ptr<quad> quad = m_composite->AddQuad(quadWidth, quadHeight);

	// vertical flip
	quad->TransformUV({{ 0, 0 }}, {{ 1, 0, 0, -1 }});

	quad->ResetRotation();
	quad->RotateXBy(0.3f);

	static float xOffset = -quadWidth / 2;
	
	quad->MoveTo(xOffset + quadWidth / 2, -0.3f + 0.8f, 0);
	
	xOffset += quadWidth;

	quad->GetMaterial()->Set(color(0.5f, 0.5f, 0.5f, 1.0f), color(0.5f, 0.5f, 0.5f, 1.0f), color(0.0f, 0.0f, 0.0f, 1.0f));

	quad->SetMaterialTexture(DimObj::MaterialTexture::Ambient, browser.texture.get());
	quad->SetMaterialTexture(DimObj::MaterialTexture::Diffuse, browser.texture.get());
	
	m_Browsers[std::to_string(id)] = browser;

	HUD_OUT("created browser id = %d (%dx%d)", id, width, height);

	return std::to_string(id);
}

WebBrowserController*	Browsers::GetBrowser(const std::string& id) {
	if (m_Browsers.find(id) != m_Browsers.end()) {
		return m_Browsers[id].controller;
	}

	return nullptr;
}

void Browsers::SetKeyFocus(const std::string& id) {
	m_browserInKeyFocus = GetBrowser(id);
}

void Browsers::OnKey(unsigned int scanCode, char16_t chr) {
	if (m_browserInKeyFocus) {
		if (scanCode == VK_ESCAPE) {
			m_browserInKeyFocus = nullptr;
			HUD_OUT("browser control is released");
		}
		else {
			// Process displayable characters. 
			std::string nonUnicodeChar = utf16_to_utf8(std::u16string(1, chr));

			m_browserInKeyFocus->SendKeySequence(nonUnicodeChar);
		}
	}
}

