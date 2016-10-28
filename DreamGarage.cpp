#include "Logger/Logger.h"
#include "DreamGarage.h"
#include <string>

// TODO make it possible to have different Dream Applications, then split the TESTING code into a new app
//#define TESTING

light *g_pLight = nullptr;
light *g_pLight2 = nullptr;

#include "Cloud/CloudController.h"
#include "Cloud/Message/UpdateHeadMessage.h"
#include "Cloud/Message/UpdateHandMessage.h"
#include "Cloud/Message/AudioDataMessage.h"

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

// Rotation testing TODO: move to seperate app
/*
volume *pVolume;
composite *pVolume2;
volume *pVolume3;
volume *pVolume4;
std::vector<composite*> bears;
//*/

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	// TODO: This should go into an "initialize" function
	InitializeCloudControllerCallbacks();

	// IO
	RegisterSubscriber((SK_SCAN_CODE)('C'), this);

	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamApp, this);

	for (auto x : std::array<int, 8>()) {
		user* pNewUser = AddUser();
		pNewUser->SetVisible(false);
		m_usersPool.push_back(pNewUser);
	}

	AddSkybox();

	g_pLight2 = AddLight(LIGHT_DIRECITONAL, 1.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
	g_pLight2->EnableShadows();

	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.3f, -1.0f, 0.2f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.3f, -1.0f, 0.2f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.3f, -1.0f, 0.2f));
	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.3f, -1.0f, 0.2f));


	AddSphere(0.2f, 30, 30, color(COLOR_RED))->MoveTo(point(0.5f, -1.0f, 0));
	AddSphere(0.2f, 30, 30, color(COLOR_RED))->MoveTo(point(0.0f, -1.0f, 0.5f));
	AddVolume(0.2f)->MoveTo(point(0.0f, -1.0f, 0.0f));

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

//Error:
	return r;
}

RESULT DreamGarage::SendHeadPosition() {
	RESULT r = R_PASS;

	point ptPosition = GetCameraPosition();
	ptPosition.y() *= -1.0f;	// TODO: This is an issue with the OVR position 

	quaternion qOrientation = GetCameraOrientation();

	//CR(SendUpdateHeadMessage(NULL, ptPosition, qOrientation));
	CR(BroadcastUpdateHeadMessage(ptPosition, qOrientation));

Error:
	return r;
}

RESULT DreamGarage::SendHandPosition() {
	RESULT r = R_PASS;

	///*
	hand *pLeftHand = GetHand(hand::HAND_LEFT);
	hand *pRightHand = GetHand(hand::HAND_RIGHT);

	if (pLeftHand != nullptr) {
		//CR(SendUpdateHandMessage(NULL, pLeftHand->GetHandState()));
		CR(BroadcastUpdateHandMessage(pLeftHand->GetHandState()));
	}

	if (pRightHand != nullptr) {
		//CR(SendUpdateHandMessage(NULL, pRightHand->GetHandState()));
		CR(BroadcastUpdateHandMessage(pRightHand->GetHandState()));
	}

	//CR(SendUpdateHandMessage(NULL, hand::GetDebugHandState(hand::HAND_LEFT)));

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

// Cloud Controller
RESULT DreamGarage::HandlePeersUpdate(long index) {
	RESULT r = R_PASS;

	if (m_isSeated) {
		LOG(INFO) << "HandlePeersUpdate olready seated" << index;
		return R_PASS;
	}

	LOG(INFO) << "HandlePeersUpdate " << index;
	
	OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(index)).c_str());

	if (!m_isSeated) {
		// an initial imp for seating. would be change once we decide final seating configurations
		camera* cam = GetCamera();
		const float rad = 2.0f;

		auto setCameraRoundtablePos = [&](uint16_t angle) {
			cam->SetPosition(point(+rad*sin(angle*M_PI / 180.0f), 0.0f, -rad*cos(angle*M_PI / 180.0f)));
			cam->RotateYByDeg(angle);
		};

		switch (index) {
			case 0: setCameraRoundtablePos(180); break;
			case 1: setCameraRoundtablePos(0); break;
			case 2: setCameraRoundtablePos(270); break;
			case 3: setCameraRoundtablePos(90); break;
			case 4: setCameraRoundtablePos(180 + 45); break;
			case 5: setCameraRoundtablePos(270 + 45); break;
			case 6: setCameraRoundtablePos(45); break;
			case 7: setCameraRoundtablePos(90 + 45); break;
		}

		m_isSeated = true;
	}

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
			m_peerUsers[userId]->SetVisible(true);
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
	
	// camera coordinate system is reversed from world coordinate system.
	// TODO: fix camera coordinate sysmte
	headPos.x() = -headPos.x();
	headPos.z() = -headPos.z();

	pUser->SetPosition(headPos);

	OVERLAY_DEBUG_SET(st, (st + "=" + std::to_string(headPos.x()) + "," + std::to_string(headPos.y()) + "," + std::to_string(headPos.z())).c_str());

	quaternion qOrientation = pUpdateHeadMessage->GetOrientation();
	qOrientation.Reverse();

	pUser->SetOrientation(qOrientation);

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

RESULT DreamGarage::Notify(SenseKeyboardEvent *kbEvent)  {
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

RESULT DreamGarage::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	HUD_OUT("DreamAPP command");

	return r;
}
