#include "DreamGarage.h"

#include "DreamLogger/DreamLogger.h"

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
#include "DreamGarage/DreamEnvironmentApp.h"
#include "DreamControlView/DreamControlView.h"
#include "DreamShareView/DreamShareView.h"
#include "DreamGarage/DreamDesktopDupplicationApp/DreamDesktopApp.h"
#include "DreamGarage/DreamSettingsApp.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramStandard.h"

#include "PhysicsEngine/CollisionManifold.h"

#include "HAL/Pipeline/ProgramNode.h"
#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/SourceNode.h"
#include "HAL/UIStageProgram.h"
#include "HAL/EnvironmentProgram.h"

#include "Core/Utilities.h"

#include "Cloud/Environment/PeerConnection.h"

#include "DreamGarageMessage.h"
#include "UpdateHeadMessage.h"
#include "UpdateHandMessage.h"
#include "UpdateMouthMessage.h"
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
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = false;
	sandboxconfig.fInitCloud = true;

#ifdef _DEBUG
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fMouseLook = false;
#endif

	SetSandboxConfiguration(sandboxconfig);

	//Error:
	return r;
}

// Temp:
#include "HAL/opengl/OGLProgramReflection.h"
#include "HAL/opengl/OGLProgramRefraction.h"
#include "HAL/opengl/OGLProgramWater.h"
#include "HAL/opengl/OGLProgramSkyboxScatter.h"

RESULT DreamGarage::SetupPipeline(Pipeline* pRenderPipeline) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = GetHALImp();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	{

		// Reflection 

		ProgramNode* pReflectionProgramNode;
		pReflectionProgramNode = nullptr;
		pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
		CN(pReflectionProgramNode);
		CR(pReflectionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pReflectionProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		ProgramNode* pReflectionSkyboxProgram;
		pReflectionSkyboxProgram = nullptr;
		pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		CN(pReflectionSkyboxProgram);
		CR(pReflectionSkyboxProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pReflectionSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", pReflectionProgramNode->Output("output_framebuffer")));

		// Refraction

		ProgramNode* pRefractionProgramNode;
		pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
		CN(pRefractionProgramNode);
		CR(pRefractionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pRefractionProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		//ProgramNode* pRefractionSkyboxProgram;
		//pRefractionSkyboxProgram = nullptr;
		//pRefractionSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter");
		//CN(pRefractionSkyboxProgram);
		//CR(pRefractionSkyboxProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
		//
		//// Connect output as pass-thru to internal blend program
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer", pRefractionProgramNode->Output("output_framebuffer")));

		// "Water"

		ProgramNode* pWaterProgramNode;
		pWaterProgramNode = nullptr;
		pWaterProgramNode = pHAL->MakeProgramNode("water");
		CN(pWaterProgramNode);
		CR(pWaterProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pWaterProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// TODO: This is not particularly general yet
		// Uncomment below to turn on water effects
		CR(pWaterProgramNode->ConnectToInput("input_refraction_map", pRefractionProgramNode->Output("output_framebuffer")));
		CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Standard shader

		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		CR(pRenderProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

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

		ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
		CN(pUIProgramNode);
		CR(pUIProgramNode->ConnectToInput("clippingscenegraph", GetUIClippingSceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		//TODO: Matrix node
		//CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

		// Connect output as pass-thru to internal blend program
		CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// save interface for UI apps
		m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

		// save interfaces to skybox nodes
		m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
		m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

		auto pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

		if (GetHMD() != nullptr) {
			if (GetHMD()->GetDeviceType() == HMDDeviceType::META) {
				m_pUIProgramNode->SetIsAugmented(true);
				pEnvironmentNode->SetIsAugmented(true);
			}
		}

		/*
		ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(pUIProgramNode);
		CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
		CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		//*/

		// Screen Quad Shader (opt - we could replace this if we need to)
		//ProgramNode *pRenderScreenQuad = pHAL->MakeProgramNode("screenquad");
		//CN(pRenderScreenQuad);
		//CR(pRenderScreenQuad->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

		// Connect Program to Display
		//CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenQuad->Output("output_framebuffer")));
		CR(pDestSinkNode->ConnectToAllInputs(pUIProgramNode->Output("output_framebuffer")));

		//CR(pHAL->ReleaseCurrentContext());


		quad *pWaterQuad = MakeQuad(1000.0f, 1000.0f);
		point ptQuadOffset = point(90.0f, -1.3f, -25.0f);
		pWaterQuad->SetPosition(ptQuadOffset);
		pWaterQuad->SetMaterialColors(color(57.0f / 255.0f, 88.0f / 255.0f, 151.0f / 255.0f, 1.0f));
		CN(pWaterQuad);

		if (pWaterProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pWaterQuad));
		}

		if (pReflectionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramReflection*>(pReflectionProgramNode)->SetReflectionObject(pWaterQuad));
		}

		if (pRefractionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramRefraction*>(pRefractionProgramNode)->SetRefractionObject(pWaterQuad));
		}

		if (pReflectionSkyboxProgram != nullptr) {
			CR(dynamic_cast<OGLProgramSkyboxScatter*>(pReflectionSkyboxProgram)->SetReflectionObject(pWaterQuad));
		}

	}

Error:
	return r;
}

RESULT DreamGarage::SetupUserModelPool() {
	RESULT r = R_PASS;

	// Set up user pool
	for (int i = 0; i < MAX_PEERS; i++) {
		m_usersModelPool[i] = std::make_pair<DreamPeerApp*, user*>(nullptr, MakeUser());
		//m_usersModelPool[i].second->SetVisible(false);
	}

//Error:
	return r;
}

RESULT DreamGarage::AllocateAndAssignUserModelFromPool(DreamPeerApp *pDreamPeer) {
	RESULT r = R_PASS;

	for (auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == nullptr) {

			//userModelPair.second->SetVisible(false);
			CR(pDreamPeer->AssignUserModel(userModelPair.second));

			userModelPair.first = pDreamPeer;

			return R_PASS;
		}
	}

	return R_POOL_FULL;

Error:
	return r;
}

RESULT DreamGarage::UnallocateUserModelFromPool(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	for (auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer.get()) {
			// release model and set to invisible
			userModelPair.first = nullptr;
			userModelPair.second->SetVisible(false);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

user* DreamGarage::FindUserModelInPool(DreamPeerApp *pDreamPeer) {
	for (const auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer) {
			return userModelPair.second;
		}
	}

	return nullptr;
}

RESULT DreamGarage::LoadScene() {
	RESULT r = R_PASS;

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

#ifndef _DEBUG

	bool fShowModels = true;
	auto pHMD = GetHMD();

	if (pHMD != nullptr) {
		if (pHMD->GetDeviceType() == HMDDeviceType::META) {
			fShowModels = false;
		}
	}

	if (fShowModels) {
		m_pDreamEnvironmentApp = LaunchDreamApp<DreamEnvironmentApp>(this);
		CN(m_pDreamEnvironmentApp);
	}

#endif

	CR(SetupUserModelPool());

	AddSkybox();


Error:
	return r;
}

std::shared_ptr<DreamPeerApp> g_pDreamPeerApp = nullptr;

RESULT DreamGarage::DidFinishLoading() {
	RESULT r = R_PASS;

	//CR(InitializeKeyboard());
	// what used to be in this function is now in DreamUserControlArea::InitializeApp

	m_pDreamUserApp = LaunchDreamApp<DreamUserApp>(this, false);
	CN(m_pDreamUserApp);

	m_pDreamUserControlArea = LaunchDreamApp<DreamUserControlArea>(this, false);
	CN(m_pDreamUserControlArea);

	m_pDreamUserControlArea->SetDreamUserApp(m_pDreamUserApp);
	m_pDreamUserControlArea->SetUIProgramNode(m_pUIProgramNode);

	m_pDreamEnvironmentApp->SetSkyboxPrograms(m_skyboxProgramNodes);

	m_pDreamShareView = LaunchDreamApp<DreamShareView>(this);
	CN(m_pDreamShareView);

	m_pDreamSettings = LaunchDreamApp<DreamSettingsApp>(this, false);
	CN(m_pDreamSettings);

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

RESULT DreamGarage::BroadcastUpdateMouthMessage(float mouthSize) {
	RESULT r = R_PASS;

	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateMouthMessage updateMouthMessage(GetUserID(), -1, mouthSize);
	CR(BroadcastDataMessage(&updateMouthMessage));

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

	hand *pLeftHand = GetHand(HAND_TYPE::HAND_LEFT);
	hand *pRightHand = GetHand(HAND_TYPE::HAND_RIGHT);

	if (pLeftHand != nullptr) {
		CR(BroadcastUpdateHandMessage(pLeftHand->GetHandState()));
	}

	if (pRightHand != nullptr) {
		CR(BroadcastUpdateHandMessage(pRightHand->GetHandState()));
	}

Error:
	return r;
}

RESULT DreamGarage::SendMouthSize() {
	RESULT r = R_PASS;

	// TODO: get actual mouth size from audio (or create observer pathway - prefer former)
	float mouthSize = GetCloudController()->GetRunTimeMicAverage();

	CR(BroadcastUpdateMouthMessage(mouthSize));

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

// Mouth update time
#define UPDATE_MOUTH_COUNT_THROTTLE 90
#define UPDATE_MOUTH_COUNT_MS ((1000.0f) / UPDATE_MOUTH_COUNT_THROTTLE)
std::chrono::system_clock::time_point g_lastMouthUpdateTime = std::chrono::system_clock::now();

// Hands update time
#define CHECK_PEER_APP_STATE_INTERVAL_MS (3000.0f)
std::chrono::system_clock::time_point g_lastPeerStateCheckTime = std::chrono::system_clock::now();

// For testing
std::chrono::system_clock::time_point g_lastDebugUpdate = std::chrono::system_clock::now();

RESULT DreamGarage::Update(void) {
	RESULT r = R_PASS;

	//m_browsers.Update();

	// TODO: Move this into DreamApp arch
	//if (m_pDreamUIBar != nullptr) {
	//	m_pDreamUIBar->Update();
	//}

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

	// Mouth update
	// TODO: this should go up into DreamOS or even sandbox
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastMouthUpdateTime).count() > UPDATE_MOUTH_COUNT_MS) {
		SendMouthSize();
		g_lastMouthUpdateTime = timeNow;
	}
	//*/

	/*
	// For testing
	if (std::chrono::duration_cast<std::chrono::seconds>(timeNow - g_lastDebugUpdate).count() > 10) {
		static int index = 1;

		point ptSeatPosition;
		float angleRotation;

		GetRoundtablePosition(index++, ptSeatPosition, angleRotation);

		m_usersModelPool[index].second->GetHead()->RotateYByDeg(angleRotation);
		m_usersModelPool[index].second->SetPosition(ptSeatPosition);
		m_usersModelPool[index].second->SetVisible(true);

		g_lastDebugUpdate = timeNow;
	}
	//*/

	// Periodically check peer app states
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastPeerStateCheckTime).count() > CHECK_PEER_APP_STATE_INTERVAL_MS) {
		CR(CheckDreamPeerAppStates());
		g_lastPeerStateCheckTime = timeNow;
	}

	//TODO: use the DreamUserControlArea
	if (m_fShouldUpdateAppComposites) {
		m_pDreamUserControlArea->ResetAppComposite();

		m_fShouldUpdateAppComposites = false;
	}

Error:
	return r;
}

RESULT DreamGarage::GetRoundtablePosition(int index, point &ptPosition, float &rotationAngle) {
	RESULT r = R_PASS;

	point ptSeatingCenter = point(0.0f, 1.0f, 1.0f);

	CB((index < m_seatLookup.size()));

	{

		float diffAngle = (180.0f - (m_keepOutAngle * 2.0f)) / m_seatLookup.size();
		diffAngle *= -1.0f;

		rotationAngle = m_initialAngle + (diffAngle * m_seatLookup[index]);

		//TODO: fuck this
		//if (m_pDreamBrowser != nullptr) {
		//	ptSeatingCenter.y() = (m_pDreamBrowser->GetHeight() / 3.0f);
	//	}

		float ptX = -1.0f * m_seatPositioningRadius * std::sin(rotationAngle * M_PI / 180.0f);
		float ptZ = m_seatPositioningRadius * std::cos(rotationAngle * M_PI / 180.0f);

		ptPosition = point(ptX, 0.0f, ptZ) + ptSeatingCenter;

		// TODO: Remove this (this is a double reverse)
		//rotationAngle *= -1.0f;
	}

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

RESULT DreamGarage::SetRoundtablePosition(DreamPeerApp *pDreamPeer, int seatingPosition) {
	RESULT r = R_PASS;

	point ptSeatPosition;
	float angleRotation;

	CR(GetRoundtablePosition(seatingPosition, ptSeatPosition, angleRotation));

	pDreamPeer->GetUserModel()->GetHead()->RotateYByDeg(angleRotation);
	pDreamPeer->SetPosition(ptSeatPosition);

Error:
	return r;
}

// Cloud Controller

RESULT DreamGarage::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	RESULT r = R_PASS;

	CR(UnallocateUserModelFromPool(pDreamPeer));

Error:
	return r;
}

RESULT DreamGarage::OnNewSocketConnection(int seatPosition) {
	RESULT r = R_PASS;

	if (!m_fSeated) {
		CB(seatPosition < m_seatLookup.size());
		CR(SetRoundtablePosition(seatPosition));
		m_fSeated = true;
		m_fShouldUpdateAppComposites = true;
	}

Error:
	return r;
}

RESULT DreamGarage::OnNewDreamPeer(DreamPeerApp *pDreamPeer) {
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

	DOSLOG(INFO, "OnNewDreamPeer local seat position %v", localSeatingPosition);
	//OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(localSeatingPosition)).c_str());

	if (!m_fSeated) {
		CBM((localSeatingPosition < m_seatLookup.size()), "Peer index %d not supported by client", localSeatingPosition);
		CR(SetRoundtablePosition(localSeatingPosition));

		m_fSeated = true;
		m_fShouldUpdateAppComposites = true;
	}
	//*/

	// Assign Model From Pool and position peer
	pDreamPeer->SetVisible(false);
	CR(AllocateAndAssignUserModelFromPool(pDreamPeer));
	CR(SetRoundtablePosition(pDreamPeer, remoteSeatingPosition));
	pDreamPeer->SetVisible(true);

	// Turn on sound
	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy;
	pWebRTCPeerConnectionProxy = GetWebRTCPeerConnectionProxy(pPeerConnection);
	CN(pWebRTCPeerConnectionProxy);

	if (pWebRTCPeerConnectionProxy != nullptr) {
		pWebRTCPeerConnectionProxy->SetAudioVolume(1.0f);
	}

	if (pPeerConnection->GetPeerUserID() == m_pendingAssetReceiveUserID) {
		m_pDreamShareView->StartReceiving(pPeerConnection);
		m_pendingAssetReceiveUserID = -1;
	}

Error:
	return r;
}

RESULT DreamGarage::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) {
	RESULT r = R_PASS;
	//DOSLOG(INFO, "[DreamGarage] Data received");

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

		case DreamGarageMessage::type::UPDATE_MOUTH: {
			UpdateMouthMessage *pUpdateMouthMessage = reinterpret_cast<UpdateMouthMessage*>(pDreamMessage);
			CR(HandleMouthUpdateMessage(pPeerConnection, pUpdateMouthMessage));
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

RESULT DreamGarage::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
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

	/*
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
	*/

	point ptHeadPosition = pUpdateHeadMessage->GetPosition();
	quaternion qOrientation = pUpdateHeadMessage->GetOrientation();

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	pDreamPeer->SetPosition(ptHeadPosition);
	pDreamPeer->SetOrientation(qOrientation);

Error:
	return r;
}

RESULT DreamGarage::HandleHandUpdateMessage(PeerConnection* pPeerConnection, UpdateHandMessage *pUpdateHandMessage) {
	RESULT r = R_PASS;

	//DEBUG_LINEOUT("HandleUpdateHandMessage");
	//pUpdateHandMessage->PrintMessage();

	/*
	long senderUserID = pPeerConnection->GetPeerUserID();
	hand::HandState handState;

	user* pUser = ActivateUser(senderUserID);
	WCN(pUser);

	handState = pUpdateHandMessage->GetHandState();
	pUser->UpdateHand(handState);
	*/

	hand::HandState handState = pUpdateHandMessage->GetHandState();

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	pDreamPeer->UpdateHand(handState);

Error:
	return r;
}

RESULT DreamGarage::HandleMouthUpdateMessage(PeerConnection* pPeerConnection, UpdateMouthMessage *pUpdateMouthMessage) {
	RESULT r = R_PASS;

	float mouthSize = pUpdateMouthMessage->GetMouthSize();
	float mouthScale = mouthSize * 10.0f;
	util::Clamp<float>(mouthScale, 0.0f, 1.0f);

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	pDreamPeer->UpdateMouth(mouthScale);

Error:
	return r;
}

// This function is currently defunct, but will be removed when the actual audio infrastructure is turned on
RESULT DreamGarage::HandleAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	/*
	long senderUserID = pPeerConnection->GetPeerUserID();
	user* pUser = ActivateUser(senderUserID);
	WCN(pUser);
	*/

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	{
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
		mouthScale *= 10.0f;

		util::Clamp<float>(mouthScale, 0.0f, 1.0f);
		pDreamPeer->UpdateMouth(mouthScale);
	}

Error:
	return r;
}

RESULT DreamGarage::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (m_pDreamUserControlArea != nullptr) {
		CR(m_pDreamUserControlArea->AddEnvironmentAsset(pEnvironmentAsset));
	}

Error:
	return r;
}

RESULT DreamGarage::OnCloseAsset() {
	RESULT r = R_PASS;

	if (m_pDreamUserControlArea != nullptr) {
		CR(m_pDreamUserControlArea->CloseActiveAsset());
	}

Error:
	return r;
}

RESULT DreamGarage::OnGetSettings(float height, float depth, float scale) {

	m_pDreamUserApp->UpdateHeight(height);
	m_pDreamUserApp->UpdateDepth(depth);
	m_pDreamUserApp->UpdateScale(scale);

	return R_PASS;
}

RESULT DreamGarage::OnSetSettings() {
	return R_PASS;
}

RESULT DreamGarage::OnSettings(std::string strURL) {
	RESULT r = R_PASS;

	CR(m_pDreamSettings->InitializeSettingsForm(strURL));
	// more complicated form for testing until signup exists
	//CR(m_pDreamSettings->InitializeSettingsForm("https://www.develop.dreamos.com/forms/account/signup"));
	CR(m_pDreamSettings->Show());

Error:
	return r;
}

RESULT DreamGarage::OnLogin() {
	RESULT r = R_PASS;

	//TODO: other pieces of login flow
	UserControllerProxy *pUserController = dynamic_cast<UserControllerProxy*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

	//TODO: uncomment when everything else works
	//CR(pUserController->RequestGetSettings(GetHardwareID(), GetHMDTypeString()));
	
//Error:
	return r;
}

RESULT DreamGarage::OnLogout() {
	return R_PASS;
}

RESULT DreamGarage::OnShareAsset() {
	RESULT r = R_PASS;

	CN(m_pDreamUserControlArea);
	CN(m_pDreamShareView);

	CR(m_pDreamShareView->ShowCastingTexture());
	CR(m_pDreamShareView->BeginStream());
	CR(m_pDreamShareView->Show());

Error:
	return r;
}

RESULT DreamGarage::OnReceiveAsset(long userID) {
	RESULT r = R_PASS;
	if (m_pDreamShareView != nullptr) {

		m_pDreamShareView->PendReceiving();

		m_pDreamUserControlArea->OnReceiveAsset();

		// if not connected yet, save the userID and start receiving during
		// OnNewPeerConnection; otherwise this user should receive the dream message
		// to start receiving
		if (FindPeer(userID) == nullptr) {
			m_pendingAssetReceiveUserID = userID;
		}

		//m_pDreamBrowser->StartReceiving();
	}
	return r;
}

RESULT DreamGarage::OnStopSending() {
	RESULT r = R_PASS;
	CR(m_pDreamShareView->StopSending());
Error:
	return r;
}

RESULT DreamGarage::OnStopReceiving() {
	RESULT r = R_PASS;
	CR(m_pDreamShareView->StopReceiving());

	m_pendingAssetReceiveUserID = -1;

Error:
	return r;
}

RESULT DreamGarage::OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) {
	RESULT r = R_PASS;
	CN(m_pDreamUserControlArea);
	
	m_pDreamUserControlArea->OnDesktopFrame(messageSize, pMessageData, pxHeight, pxWidth);
	
Error:
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
