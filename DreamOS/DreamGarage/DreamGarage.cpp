#include "DreamGarage.h"

#include "Logger/Logger.h"
#include "easylogging++.h"

#include <string>
#include <array>

light *g_pLight = nullptr;

#include "Cloud/CloudController.h"
//#include "Cloud/Message/UpdateHeadMessage.h"
//#include "Cloud/Message/UpdateHandMessage.h"
//#include "Cloud/Message/AudioDataMessage.h"

#include "DreamGarage/DreamContentView.h"
#include "DreamGarage/DreamUIBar.h"
#include "DreamGarage/DreamBrowser.h"
#include "DreamGarage/DreamControlView.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramEnvironmentObjects.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"

#include "Core/Utilities.h"

#include "Cloud/Environment/PeerConnection.h"

#include "DreamGarageMessage.h"
#include "UpdateHeadMessage.h"
#include "UpdateHandMessage.h"
#include "AudioDataMessage.h"

// TODO: Should this go into the DreamOS side?
/*
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
*/


RESULT DreamGarage::ConfigureSandbox() {
	RESULT r = R_PASS;

	SandboxApp::configuration sandboxconfig;
	sandboxconfig.fUseHMD = false;
	sandboxconfig.fUseLeap = false;

#ifdef _DEBUG
	sandboxconfig.fMouseLook = true;
#endif

	SetSandboxConfiguration(sandboxconfig);

	//Error:
	return r;
}

RESULT DreamGarage::SetupPipeline(Pipeline* pRenderPipeline) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = GetHALImp();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("environment");
	CN(pRenderProgramNode);
	CR(pRenderProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
	CR(pRenderProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

	// Reference Geometry Shader Program
	ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
	CN(pReferenceGeometryProgram);
	CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
	CR(pReferenceGeometryProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

	CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

	// Skybox
	ProgramNode* pSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
	CN(pSkyboxProgram);
	CR(pSkyboxProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
	CR(pSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

	ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture");
	CN(pUIProgramNode);
	CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
	CR(pUIProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
	CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

	// Debug Console
	ProgramNode* pDreamConsoleProgram = pHAL->MakeProgramNode("debugconsole");
	CN(pDreamConsoleProgram);
	CR(pDreamConsoleProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

	// Connect output as pass-thru to internal blend program
	CR(pDreamConsoleProgram->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

	// Screen Quad Shader (opt - we could replace this if we need to)
	ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
	CN(pRenderScreenQuad);
	
	CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pDreamConsoleProgram->Output("output_framebuffer")));

	// Connect Program to Display
	CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));

	//CR(pHAL->ReleaseCurrentContext());

Error:
	return r;
}

RESULT DreamGarage::SetupUserModelPool() {
	RESULT r = R_PASS;

	// Set up user pool
	for (int i = 0; i < MAX_PEERS; i++) {
		m_usersModelPool[i] = std::make_pair<DreamPeer*, user*>(nullptr, AddUser());
		m_usersModelPool[i].second->SetVisible(false);
	}

//Error:
	return r;
}

RESULT DreamGarage::AllocateAndAssignUserModelFromPool(DreamPeer *pDreamPeer) {
	RESULT r = R_PASS;

	for (auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == nullptr) {
			userModelPair.second->SetVisible(0.0f);
			CR(pDreamPeer->AssignUserModel(userModelPair.second));

			userModelPair.first = pDreamPeer;

			return R_PASS;
		}
	}

	return R_POOL_FULL;

Error:
	return r;
}

RESULT DreamGarage::UnallocateUserModelFromPool(DreamPeer *pDreamPeer) {
	for (auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer) {
			// release model and set to invisible
			userModelPair.first = nullptr;
			userModelPair.second->SetVisible(false);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

user* DreamGarage::FindUserModelInPool(DreamPeer *pDreamPeer) {
	for (const auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer) {
			return userModelPair.second;
		}
	}

	return nullptr;
}

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

	point sceneOffset = point(90, -5, -25);
	float sceneScale = 0.1f;
	vector sceneDirection = vector(0.0f, 0.0f, 0.0f);

	// Keyboard
	RegisterSubscriber(SenseVirtualKey::SVK_ALL, this);
	RegisterSubscriber(SENSE_TYPING_EVENT_TYPE::CHARACTER_TYPING, this);

	///*
	HALImp::HALConfiguration halconf;
	halconf.fRenderReferenceGeometry = false;
	halconf.fDrawWireframe = false;
	halconf.fRenderProfiler = false;
	SetHALConfiguration(halconf);
	//*/

	// Console
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::DreamApp, this);

	CR(SetupUserModelPool());
	
	AddSkybox();

	g_pLight = AddLight(LIGHT_DIRECITONAL, 2.0f, point(0.0f, 10.0f, 0.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, -1.0f, 0.0f));
	g_pLight->EnableShadows();

	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, 4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(-4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));
	AddLight(LIGHT_POINT, 1.0f, point(4.0f, 7.0f, -4.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

	AddLight(LIGHT_POINT, 5.0f, point(20.0f, 7.0f, -40.0f), color(COLOR_WHITE), color(COLOR_WHITE), vector(0.0f, 0.0f, 0.0f));

#ifndef _DEBUG
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
				// Do some stuff pre-render
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
#endif

	m_pDreamUIBar = LaunchDreamApp<DreamUIBar>(this, false);
	CN(m_pDreamUIBar);

#ifndef _DEBUG
	m_pDreamBrowser = LaunchDreamApp<DreamBrowser>(this);
	CNM(m_pDreamBrowser, "Failed to create dream browser");

	m_pDreamBrowser->SetNormalVector(vector(0.0f, 0.0f, 1.0f));
	m_pDreamBrowser->SetDiagonalSize(9.0f);
	m_pDreamBrowser->SetPosition(point(0.0f, 2.0f, -2.0f));
	
	m_pDreamBrowser->SetVisible(false);
#endif

	//m_pDreamControlView = LaunchDreamApp<DreamControlView>(this);
	//CN(m_pDreamControlView);

	//m_pDreamControlView->SetSharedViewContext(m_pDreamBrowser);

	//TODO: collisions doesn't follow properly
	//m_pDreamBrowser->SetParams(point(0.0f, 2.0f, -2.0f), 5.0f, 1.7f, vector(0.0f, 0.0f, 1.0f));
	//m_pDreamBrowser->SetPosition(point(0.0f, 2.0f, 0.0f));
	//*/
/*
	m_pDreamContentView = LaunchDreamApp<DreamContentView>(this);
	CNM(m_pDreamContentView, "Failed to create dream content view");

	m_pDreamContentView->SetParams(point(0.0f, 2.0f, -2.0f), 5.0f, DreamContentView::AspectRatio::ASPECT_16_9, vector(0.0f, 0.0f, 1.0f));

	m_pDreamContentView->SetVisible(false);
	m_pDreamContentView->SetFitTextureAspectRatio(true);
	//*/

	//CR(GetCloudController()->RegisterEnvironmentAssetCallback(std::bind(&DreamGarage::HandleOnEnvironmentAsset, this, std::placeholders::_1)));

	// UIKeyboard App
	CR(InitializeKeyboard());

Error:
	return r;
}

RESULT DreamGarage::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHeadMessage updateHeadMessage(GetUserID(), userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);
	CR(SendDataMessage(userID, &updateHeadMessage));

Error:
	return r;
}

RESULT DreamGarage::SendUpdateHandMessage(long userID, hand::HandState handState) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHandMessage updateHandMessage(GetUserID(), userID, handState);
	CR(SendDataMessage(userID, &updateHandMessage));

Error:
	return r;
}

RESULT DreamGarage::BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;


	// Create the message
	UpdateHeadMessage updateHeadMessage(GetUserID(), -1, ptPosition, qOrientation, vVelocity, qAngularVelocity);
	CR(BroadcastDataMessage(&updateHeadMessage));

Error:
	return r;
}

RESULT DreamGarage::BroadcastUpdateHandMessage(hand::HandState handState) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHandMessage updateHandMessage(GetUserID(), -1, handState);
	CR(BroadcastDataMessage(&updateHandMessage));

Error:
	return r;
}

RESULT DreamGarage::SendHeadPosition() {
	RESULT r = R_PASS;

	point ptPosition = GetCameraPosition();
	quaternion qOrientation = GetCameraOrientation();

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

/*
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
*/

// Head update time
#define UPDATE_HEAD_COUNT_THROTTLE 90	
#define UPDATE_HEAD_COUNT_MS ((1000.0f) / UPDATE_HEAD_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastHeadUpdateTime = std::chrono::system_clock::now();

// Hands update time
#define UPDATE_HAND_COUNT_THROTTLE 90	
#define UPDATE_HAND_COUNT_MS ((1000.0f) / UPDATE_HAND_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastHandUpdateTime = std::chrono::system_clock::now();

// For testing
std::chrono::system_clock::time_point g_lastDebugUpdate = std::chrono::system_clock::now();

RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;
	
	//m_browsers.Update();

	// TODO: Move this into DreamApp arch
	m_pDreamUIBar->Update();

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
	// For testing
	if (std::chrono::duration_cast<std::chrono::seconds>(timeNow - g_lastDebugUpdate).count() > 10) {
		static int index = 0;
		//SetRoundtablePosition(index++);
		
		auto pSphere = AddSphere(0.25f, 10, 10);
		pSphere->SetPosition(GetRoundtablePosition(index++));

		g_lastDebugUpdate = timeNow;
	}
	//*/

//Error:
	return r;
}

RESULT DreamGarage::GetRoundtablePosition(int index, point &ptPosition, float &rotationAngle) {
	RESULT r = R_PASS;

	point ptSeatingCenter = point(0.0f, 1.0f, 1.0f);

	CB((index < m_seatLookup.size()));

	float diffAngle = (180.0f - (m_keepOutAngle * 2.0f)) / m_seatLookup.size();
	diffAngle *= -1.0f;

	rotationAngle = m_initialAngle + (diffAngle * m_seatLookup[index]);

	if (m_pDreamBrowser != nullptr) {
		ptSeatingCenter.y() = (m_pDreamBrowser->GetHeight() / 3.0f);
	}

	float ptX = -1.0f * m_seatPositioningRadius * std::sin(rotationAngle * M_PI / 180.0f);
	float ptZ = m_seatPositioningRadius * std::cos(rotationAngle * M_PI / 180.0f);

	ptPosition = point(ptX, 0.0f, ptZ) + ptSeatingCenter;

Error:
	return r;
}

RESULT DreamGarage::SetRoundtablePosition(int seatingPosition) {
	RESULT r = R_PASS;

	stereocamera* pCamera = GetCamera();

	point ptSeatPosition;
	float angleRotation;

	CB((seatingPosition < m_seatLookup.size()));
	CR(GetRoundtablePosition(seatingPosition, ptSeatPosition, angleRotation));
	
	if (!pCamera->HasHMD()) {
		pCamera->RotateYByDeg(angleRotation);
		pCamera->SetPosition(ptSeatPosition);
	}
	else {
		quaternion qOffset = quaternion::MakeQuaternionWithEuler(0.0f, angleRotation * M_PI / 180.0f, 0.0f);
		pCamera->SetOffsetOrientation(qOffset);
		pCamera->SetHMDAdjustedPosition(ptSeatPosition);
	}

Error:
	return r;
}

RESULT DreamGarage::SetRoundtablePosition(DreamPeer *pDreamPeer, int seatingPosition) {
	RESULT r = R_PASS;

	point ptSeatPosition;
	float angleRotation;

	CR(GetRoundtablePosition(seatingPosition, ptSeatPosition, angleRotation));

	pDreamPeer->GetUserModel()->RotateYByDeg(angleRotation);
	pDreamPeer->SetPosition(ptSeatPosition);

Error:
	return r;
}

// Cloud Controller

RESULT DreamGarage::OnNewDreamPeer(DreamPeer *pDreamPeer) {
	RESULT r = R_PASS;

	///*
	//int index = pPeerConnection->GetLoca
	PeerConnection *pPeerConnection = pDreamPeer->GetPeerConnection();

	bool fOfferor = (pPeerConnection->GetOfferUserID() == GetUserID());

	// My seating position
	long localSeatingPosition = (fOfferor) ? pPeerConnection->GetOfferorPosition() : pPeerConnection->GetAnswererPosition();
	localSeatingPosition -= 1;

	// Remote seating position
	long remoteSeatingPosition = (fOfferor) ? pPeerConnection->GetAnswererPosition() : pPeerConnection->GetOfferorPosition();
	remoteSeatingPosition -= 1;

	if (m_fSeated) {
		LOG(INFO) << "HandlePeersUpdate already seated" << localSeatingPosition;
		return R_PASS;
	}

	LOG(INFO) << "HandlePeersUpdate " << localSeatingPosition;
	OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(localSeatingPosition)).c_str());

	if (!m_fSeated) {
		CBM((localSeatingPosition < m_seatLookup.size()), "Peer index %d not supported by client", localSeatingPosition);
		CR(SetRoundtablePosition(localSeatingPosition));
		m_fSeated = true;
	}
	//*/

	// Assign Model From Pool and position peer
	CR(AllocateAndAssignUserModelFromPool(pDreamPeer));
	CR(SetRoundtablePosition(pDreamPeer, remoteSeatingPosition));
	pDreamPeer->SetVisible();

	// Turn on sound
	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy = GetWebRTCPeerConnectionProxy(pPeerConnection);

	if (pWebRTCPeerConnectionProxy != nullptr) {
		pWebRTCPeerConnectionProxy->SetAudioVolume(10.0f);
	}

Error:
	return r;
}

RESULT DreamGarage::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) {
	RESULT r = R_PASS;
	//LOG(INFO) << "data received";

	/*
	if (pDataMessage) {
		std::string st((char*)pDataMessage);
		st = "<- " + st;
		HUD_OUT(st.c_str());
	}*/

	/*
	Message::MessageType switchHeadModelMessage = (Message::MessageType)((uint16_t)(Message::MessageType::MESSAGE_CUSTOM) + 1);

	if (pDataMessage->GetType() == switchHeadModelMessage) {
		HUD_OUT("Other user changed the head model");
		CR(m_pPeerUser->SwitchHeadModel());
	}

	// TODO: Handle the appropriate message here
	*/

	DreamGarageMessage::type dreamGarageMsgType = (DreamGarageMessage::type)(pDreamMessage->GetType());
	switch (dreamGarageMsgType) {
		case DreamGarageMessage::type::UPDATE_HEAD: {
			UpdateHeadMessage *pUpdateHeadMessage = reinterpret_cast<UpdateHeadMessage*>(pDreamMessage);
			CR(HandleHeadUpdateMessage(pPeerConnection, pUpdateHeadMessage));
		} break;

		case DreamGarageMessage::type::UPDATE_HAND: {
			UpdateHandMessage *pUpdateHandMessage = reinterpret_cast<UpdateHandMessage*>(pDreamMessage);
			CR(HandleHandUpdateMessage(pPeerConnection, pUpdateHandMessage));
		} break;

		case DreamGarageMessage::type::AUDIO_DATA: {
			// empty
		} break;

		default:
		case DreamGarageMessage::type::UPDATE_CHAT:
		case DreamGarageMessage::type::CUSTOM:
		case DreamGarageMessage::type::INVALID: {
			DEBUG_LINEOUT("Unhandled Dream Client Message of Type 0x%I64x", dreamGarageMsgType);
		} break;
	}

Error:
	return r;
}

user* DreamGarage::ActivateUser(long userId) {
	/*
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
	*/

	// TODO: 

	return nullptr;
}

RESULT DreamGarage::OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	RESULT r = R_PASS;

	long senderUserID = pPeerConnection->GetPeerUserID();
	long recieverUserID = pPeerConnection->GetUserID();

	AudioDataMessage audioDataMessage(senderUserID, recieverUserID, pAudioDataBuffer, bitsPerSample, samplingRate, channels, frames);

	CR(HandleAudioDataMessage(pPeerConnection, &audioDataMessage));

Error:
	return r;
}

RESULT DreamGarage::HandleHeadUpdateMessage(PeerConnection* pPeerConnection, UpdateHeadMessage *pUpdateHeadMessage) {
	RESULT r = R_PASS;

	// This will set visible 
	long senderUserID = pPeerConnection->GetPeerUserID();
	user* pUser = ActivateUser(senderUserID);

	point headPos = pUpdateHeadMessage->GetPosition();

	std::string strPosition = "pos" + std::to_string(senderUserID);

	WCN(pUser);

	quaternion qOrientation = pUpdateHeadMessage->GetOrientation();

	pUser->GetHead()->SetPosition(headPos);

	OVERLAY_DEBUG_SET(strPosition, (strPosition + "=" + std::to_string(headPos.x()) + "," + std::to_string(headPos.y()) + "," + std::to_string(headPos.z())).c_str());

	pUser->GetHead()->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamGarage::HandleHandUpdateMessage(PeerConnection* pPeerConnection, UpdateHandMessage *pUpdateHandMessage) {
	RESULT r = R_PASS;

	//DEBUG_LINEOUT("HandleUpdateHandMessage");
	//pUpdateHandMessage->PrintMessage();

	long senderUserID = pPeerConnection->GetPeerUserID();
	hand::HandState handState;

	user* pUser = ActivateUser(senderUserID);
	WCN(pUser);

	handState = pUpdateHandMessage->GetHandState();
	pUser->UpdateHand(handState);

Error:
	return r;
}

RESULT DreamGarage::HandleAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	long senderUserID = pPeerConnection->GetPeerUserID();
	user* pUser = ActivateUser(senderUserID);
	WCN(pUser);

	//auto msg = pAudioDataMessage->GetAudioMessageBody();
	auto pAudioBuffer = pAudioDataMessage->GetAudioMessageBuffer();
	CN(pAudioBuffer);

	size_t numSamples = pAudioDataMessage->GetChannels() * pAudioDataMessage->GetFrames();
	float averageAccumulator = 0.0f;

	for (int i = 0; i < numSamples; ++i) {
		//int16_t val = static_cast<const int16_t>(msg.pAudioDataBuffer[i]);
		int16_t value = *(static_cast<const int16_t*>(pAudioBuffer) + i);
		float scaledValue = (float)(value) / (std::numeric_limits<int16_t>::max());

		averageAccumulator += std::abs(scaledValue);
	}

	float mouthScale = averageAccumulator / numSamples;

	util::Clamp<float>(mouthScale, 0.0f, 1.0f);
	pUser->UpdateMouth(mouthScale);

Error:
	return r;
}

RESULT DreamGarage::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	/*
	if (m_pDreamContentView != nullptr) {
		m_pDreamContentView->SetEnvironmentAsset(pEnvironmentAsset);
		m_pDreamContentView->SetVisible(true);
	}

	//*/
	if (m_pDreamBrowser != nullptr) {
		m_pDreamBrowser->SetVisible(true);
		m_pDreamBrowser->FadeQuadToBlack();
		m_pDreamBrowser->SetEnvironmentAsset(pEnvironmentAsset);
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

	CR(r);

Error:
	return r;
}

RESULT DreamGarage::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("list") == 0) {
		HUD_OUT("<blank>");
	}

	/*
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
					HUD_OUT(("controlling browser " + event->GetArg(2) + " (hit 'esc' to release control)").c_str());
				}
				else {
					browser->LoadURL(event->GetArg(3));
				}
			}
		}
	}
	//*/

	return r;
}

