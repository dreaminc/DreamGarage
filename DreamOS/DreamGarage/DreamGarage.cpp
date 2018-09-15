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
#include "DreamGarage/DreamLoginApp.h"
#include "DreamUserApp.h"
#include "WebBrowser/CEFBrowser/CEFBrowserManager.h"	
#include "DreamGarage/DreamGamepadCameraApp.h"

#include "HAL/opengl/OGLObj.h"
#include "HAL/opengl/OGLProgramStandard.h"
#include "HAL/opengl/OGLProgramScreenFade.h"
#include "HAL/opengl/OGLProgramSkybox.h"

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
	sandboxconfig.fInitSound = true;

#ifdef _DEBUG
	sandboxconfig.fUseHMD = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = true;
	sandboxconfig.fInitSound = true;
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

		// Skybox

		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = pHAL->MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// Reflection 
		
		m_pReflectionProgramNode = pHAL->MakeProgramNode("reflection");
		CN(m_pReflectionProgramNode);
		//CR(m_pReflectionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(m_pReflectionProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		ProgramNode* pReflectionSkyboxProgram;
		pReflectionSkyboxProgram = pHAL->MakeProgramNode("skybox");
		CN(pReflectionSkyboxProgram);
		CR(pReflectionSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", m_pReflectionProgramNode->Output("output_framebuffer")));

		// Refraction

		
		m_pRefractionProgramNode = pHAL->MakeProgramNode("refraction");
		CN(m_pRefractionProgramNode);
		//CR(pRefractionProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(m_pRefractionProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		//ProgramNode* pRefractionSkyboxProgram;
		//pRefractionSkyboxProgram = pHAL->MakeProgramNode("skybox");
		//CN(pRefractionSkyboxProgram);
		//CR(pRefractionSkyboxProgram->ConnectToInput("camera", m_pDreamOS->GetCameraNode()->Output("stereocamera")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		//CR(pRefractionSkyboxProgram->ConnectToInput("input_framebuffer", m_pRefractionProgramNode->Output("output_framebuffer")));

		// "Water"

		ProgramNode* pWaterProgramNode = pHAL->MakeProgramNode("water");
		CN(pWaterProgramNode);
		// Still need scene graph for lights 
		// TODO: make lights a different node
		//CR(pWaterProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pWaterProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// TODO: This is not particularly general yet
		// Uncomment below to turn on water effects
		CR(pWaterProgramNode->ConnectToInput("input_refraction_map", m_pRefractionProgramNode->Output("output_framebuffer")));
		CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Environment shader

		m_pRenderEnvironmentProgramNode = pHAL->MakeProgramNode("minimal_texture");
		CN(m_pRenderEnvironmentProgramNode);
		//CR(m_pRenderEnvironmentProgramNode->ConnectToInput("scenegraph", m_pDreamOS->GetSceneGraphNode()->Output("objectstore")));
		CR(m_pRenderEnvironmentProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		CR(m_pRenderEnvironmentProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

		// Everything else
		ProgramNode* pRenderProgramNode = pHAL->MakeProgramNode("standard");
		CN(pRenderProgramNode);
		CR(pRenderProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pRenderProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// NOTE: Add this in if you want to have reflective objects
		//CR(pRenderProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		CR(pRenderProgramNode->ConnectToInput("input_framebuffer", m_pRenderEnvironmentProgramNode->Output("output_framebuffer")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram = pHAL->MakeProgramNode("reference");
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = pHAL->MakeProgramNode("skybox");
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode* pUIProgramNode = pHAL->MakeProgramNode("uistage");
		CN(pUIProgramNode);
		CR(pUIProgramNode->ConnectToInput("clippingscenegraph", GetUIClippingSceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
		CR(pUIProgramNode->ConnectToInput("camera", GetCameraNode()->Output("stereocamera")));

		// TODO: Matrix node
		//CR(pUIProgramNode->ConnectToInput("clipping_matrix", &m_pClippingView))

		// Connect output as pass-thru to internal blend program
		CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		// save interface for UI apps
		m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

		// save interfaces to skybox nodes
		m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pScatteringSkyboxProgram));
		//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
		//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

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
		ProgramNode *pRenderScreenFade = pHAL->MakeProgramNode("screenfade");
		//ProgramNode *pRenderScreenFade = pHAL->MakeProgramNode("screenquad");
		CN(pRenderScreenFade);
		CR(pRenderScreenFade->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));

		m_pScreenFadeProgramNode = dynamic_cast<OGLProgramScreenFade*>(pRenderScreenFade);
		// Connect Program to Display
		CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenFade->Output("output_framebuffer")));
		//CR(pDestSinkNode->ConnectToAllInputs(pRenderScreenFade->Output("output_framebuffer")));

		//CR(pHAL->ReleaseCurrentContext());

		quad *pWaterQuad = MakeQuad(1000.0f, 1000.0f);
		point ptQuadOffset = point(90.0f, -1.3f, -25.0f);
		pWaterQuad->SetPosition(ptQuadOffset);
		pWaterQuad->SetMaterialColors(color(57.0f / 255.0f, 112.0f / 255.0f, 151.0f / 255.0f, 1.0f));
		CN(pWaterQuad);

		if (pWaterProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(pWaterQuad));
		}

		if (m_pReflectionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramReflection*>(m_pReflectionProgramNode)->SetReflectionObject(pWaterQuad));
		}

		if (m_pRefractionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramRefraction*>(m_pRefractionProgramNode)->SetRefractionObject(pWaterQuad));
		}

		if (pReflectionSkyboxProgram != nullptr) {
			CR(dynamic_cast<OGLProgramSkybox*>(pReflectionSkyboxProgram)->SetReflectionObject(pWaterQuad));
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
			//pDreamPeer->GetUserModel()->GetMouth()->SetVisible(false);
			userModelPair.first = nullptr;
			userModelPair.second->SetVisible(false);
			userModelPair.second->GetMouth()->SetVisible(false);
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

	// Environment App is rendered directly by the environment program node
	m_pDreamEnvironmentApp = LaunchDreamApp<DreamEnvironmentApp>(this, false).get();
	CN(m_pDreamEnvironmentApp);
	DOSLOG(INFO, "DreamEnvironmentApp Launched");

	CNM(m_pDreamEnvironmentApp, "Dream Environment App not set");

	CN(m_pRenderEnvironmentProgramNode);
	CR(m_pRenderEnvironmentProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

	CN(m_pReflectionProgramNode);
	CR(m_pReflectionProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

	CN(m_pRefractionProgramNode);
	CR(m_pRefractionProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

	CR(SetupUserModelPool());
	DOSLOG(INFO, "UserModelPool has been setup");

	AddSkybox();
	DOSLOG(INFO, "Added Skybox");

Error:
	return r;
}

std::shared_ptr<DreamPeerApp> g_pDreamPeerApp = nullptr;

RESULT DreamGarage::DidFinishLoading() {
	RESULT r = R_PASS;

	std::string strFormType;
	//CR(InitializeKeyboard());
	// what used to be in this function is now in DreamUserControlArea::InitializeApp
	auto pDreamUserApp = LaunchDreamApp<DreamUserApp>(this, false);
	CN(pDreamUserApp);
	m_pDreamUserApp = pDreamUserApp.get();

	m_pDreamUserControlArea = LaunchDreamApp<DreamUserControlArea>(this, false).get();
	CN(m_pDreamUserControlArea);

	m_pDreamUserControlArea->SetDreamUserApp(pDreamUserApp);
	m_pDreamUserControlArea->SetUIProgramNode(m_pUIProgramNode);

	if (m_pDreamEnvironmentApp != nullptr) {
		m_pDreamEnvironmentApp->SetSkyboxPrograms(m_skyboxProgramNodes);
		m_pDreamEnvironmentApp->SetScreenFadeProgram(m_pScreenFadeProgramNode);
	}

	m_pDreamShareView = LaunchDreamApp<DreamShareView>(this, false);
	CN(m_pDreamShareView);

	m_pDreamSettings = LaunchDreamApp<DreamSettingsApp>(this, false);
	CN(m_pDreamSettings);

	m_pDreamLoginApp = LaunchDreamApp<DreamLoginApp>(this, false);
	CN(m_pDreamLoginApp);

	m_pDreamGeneralForm = LaunchDreamApp<DreamFormApp>(this, false);
	CN(m_pDreamSettings);

	if (GetSandboxConfiguration().fUseGamepad) {
		m_pDreamGamePadCameraApp = LaunchDreamApp<DreamGamepadCameraApp>(this, false).get();
	}

	// TODO: could be somewhere else(?)
	CR(RegisterDOSObserver(this));

	m_fFirstLogin = m_pDreamLoginApp->IsFirstLaunch();
	m_fHasCredentials = m_pDreamLoginApp->HasStoredCredentials(m_strRefreshToken, m_strAccessToken);

	// TODO: This might need to be reworked
	CRM(GetCloudController()->Start(false), "Failed to start cloud controller");

	// UserController is initialized during CloudController::Initialize,
	// which is in SandboxApp::Initialize while fInitCloud is true
	m_pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	CN(m_pUserController);
	
	// DEBUG:
//#ifdef _DEBUG
	{
		m_fHasCredentials = true;

		std::map<int, std::string> testRefreshTokens = {
			{ 0, "NakvA43v1eVBqvvTJuqUdXHWL02CNuDqrgHMEBrIY6P5FoHZ2GtgbCVDYvHMaRTw" },
			{ 1, "daehZbIcTcXaPh29tWQy75ZYSLrRL4prhBoBYMRQtU48NMs6svnt5CkzCA5RLKJq" },
			{ 2, "GckLS9Q691PO6RmdmwRp368JjWaETNOMEoASqQF0TCnImHzpmOv2Rch1RDrgr2V7" },
			{ 3, "HYlowX58aRPRB85IT0M2wB20RC8rd0zpOxfIIvEgMF9XVzzFbL8UzY3yyCovdEIQ" },
			{ 4, "sROmFa73UM38v7snrTaDy3JF1vCJGdJhBBLvBcCLaWxjoEYVfAqcgMAZPVHzaZrR" },
			{ 5, "gc2EPtlKmKtkmiZC6cRfUtMIHwiWW9Tf55wbFBcq45Wg8DBRDWV3iZiLsqBedfqF" },
			{ 6, "F5EwwHxmgf4pqLXZjP6zWH4NBn42UtLQUmrlU4vl62BGeprnug0Hn1WeMm3snHQa" },
			{ 7, "cuX1beJjJE58DdU4cYOrsIoNFil534fOWscH9bzmhmcFkV1qn3M8zPkdW7J3UEH1" },
			{ 8, "B3Wwz6Lbwfj2emo7caKBQXtKoMYXR9P70eOvkFzFIfh9NRlal6PLFqIagTFXiDHy" },
			{ 9, "HPfaNfjFrAhlbqS9DuZD5dCrAzI215ETDTRzFMVXrtoYrI2A9XBS3VEKOjGlDSVE" }
		};

		CommandLineManager *pCommandLineManager = CommandLineManager::instance();
		CN(pCommandLineManager);

		std::string strTestUserNumber = pCommandLineManager->GetParameterValue("rtoken");
		if ((strTestUserNumber.compare("") == 0) == false) {
			
			int testUserNumber = stoi(strTestUserNumber);

			std::string strDebugRefreshToken = testRefreshTokens[testUserNumber];
			return m_pUserController->GetAccessToken(strDebugRefreshToken);
		}
	}
//#endif
	
	// Initial step of login flow:
	if(IsConnectedToInternet()) {

#if defined(PRODUCTION_BUILD) || defined(OCULUS_PRODUCTION_BUILD) || defined(DEV_PRODUCTION_BUILD)
		CR(m_pUserController->RequestDreamVersion());
//*
#else
		CR(AuthenticateFromStoredCredentials());
#endif
//*/
	}
	else {
		m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::INTERNET_REQUIRED);
		m_pDreamUserApp->ShowMessageQuad();
		m_pDreamEnvironmentApp->FadeIn();
	}

Error:
	return r;
}

RESULT DreamGarage::OnDreamVersion(version dreamVersion) {
	RESULT r = R_PASS;

	std::string strFormType;

	if (m_versionDreamClient < dreamVersion) {	// If the server version isn't GREATER than current, we don't make them update... 
		if (m_pDreamUserApp != nullptr) {
		//	CR(m_pDreamUserApp->FadeInWithMessageQuad(DreamEnvironmentApp::StartupMessage::UPDATE_REQUIRED));
			m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::UPDATE_REQUIRED);
			m_pDreamUserApp->ShowMessageQuad();
			m_pDreamEnvironmentApp->FadeIn();
		}
	}
	else {
		CR(AuthenticateFromStoredCredentials());
	}

Error:
	return r;
}

version DreamGarage::GetDreamVersion() {
	return m_versionDreamClient;
}

RESULT DreamGarage::AuthenticateFromStoredCredentials() {
	RESULT r = R_PASS;

	std::string strFormType;
	// if there has already been a successful login, try to authenticate
	if (!m_fFirstLogin && m_fHasCredentials) {
		m_pUserController->GetAccessToken(m_strRefreshToken);
	}
	else {
		// Otherwise, start by showing the login form

		if (!m_fFirstLogin) {
			strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);
			CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::SIGN_IN));
		}
		else {
			strFormType = DreamFormApp::StringFromType(FormType::SIGN_UP);
			CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::WELCOME));
		}

		CR(m_pDreamUserApp->ShowMessageQuad());

		CR(m_pUserController->GetFormURL(strFormType));

		if (m_pDreamEnvironmentApp != nullptr) {	
			// fade into lobby (with no environment showing)
			CR(m_pDreamEnvironmentApp->FadeIn()); 
		}
	}

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

	// Periodically check peer app states
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastPeerStateCheckTime).count() > CHECK_PEER_APP_STATE_INTERVAL_MS) {
		CR(CheckDreamPeerAppStates());
		g_lastPeerStateCheckTime = timeNow;
	}

Error:
	return r;
}

RESULT DreamGarage::SetRoundtablePosition(int seatingPosition) {
	RESULT r = R_PASS;

	stereocamera* pCamera = GetCamera();

	point ptSeatPosition;
	quaternion qOffset;
	quaternion qUIOffset;

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->GetEnvironmentSeatingPositionAndOrientation(ptSeatPosition, qOffset, seatingPosition));
	qUIOffset = m_pDreamEnvironmentApp->GetUIOffsetOrientation(seatingPosition);

	CN(m_pDreamUserApp);
	CR(m_pDreamUserApp->SetAppCompositeOrientation(qOffset*qUIOffset));

	if (!pCamera->HasHMD()) {
		pCamera->SetOrientation(qOffset);
		pCamera->SetPosition(ptSeatPosition);
		CR(m_pDreamUserApp->SetAppCompositePosition(ptSeatPosition));
	}
	else {
		pCamera->SetOffsetOrientation(qOffset);
		pCamera->SetPosition(ptSeatPosition);

		m_pDreamUserApp->SetAppCompositePosition(ptSeatPosition);

		point ptUser;
		m_pDreamUserApp->GetAppBasisPosition(ptUser);

		RecenterHMD();
	}

	if (m_pDreamUserControlArea != nullptr) {
		m_pDreamUserControlArea->ResetAppComposite();
	}

Error:
	return r;
}

RESULT DreamGarage::SetRoundtablePosition(DreamPeerApp *pDreamPeer, int seatingPosition) {
	RESULT r = R_PASS;

	point ptSeatPosition;
	quaternion qRotation;
	vector vCameraDirection;
	vector vCameraDifference;

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->GetEnvironmentSeatingPositionAndOrientation(ptSeatPosition, qRotation, seatingPosition));

	//pDreamPeer->GetUserModel()->GetHead()->SetOrientation(qRotation);
	pDreamPeer->SetOrientation(qRotation);
	pDreamPeer->SetPosition(ptSeatPosition);

	// update username label
	vCameraDirection = ptSeatPosition - GetCamera()->GetPosition(true);
	vCameraDirection = vector(vCameraDirection.x(), 0.0f, vCameraDirection.z()).Normal();

	pDreamPeer->SetUserLabelPosition(ptSeatPosition);

	// Making a quaternion with two vectors uses cross product,
	// vector(0,0,1) and vector(0,0,-1) are incompatible with vector(0,0,-1)
	if (vCameraDirection == vector(0.0f, 0.0f, 1.0f)) {
		pDreamPeer->SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, (float)M_PI, 0.0f));
	}
	else if (vCameraDirection == vector(0.0f, 0.0f, -1.0f)) {
		pDreamPeer->SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, 0.0f));
	}
	else {
		pDreamPeer->SetUserLabelOrientation(quaternion(vector(0.0f, 0.0f, -1.0f), vCameraDirection));
	}
	//pDreamPeer->SetUserLabelOrientation(qRotation);
	//pDreamPeer->SetUserLabelOrientation((quaternion(qRotation.RotateVector(vector(0.0f, 0.0f, -1.0f)), vCameraDirection)) * qRotation);

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
		//*
		point ptScreenPosition;
		quaternion qScreenRotation;
		float screenScale;

		long avatarID;

		CR(m_pDreamEnvironmentApp->GetSharedScreenPosition(ptScreenPosition, qScreenRotation, screenScale));
		CR(m_pDreamShareView->UpdateScreenPosition(ptScreenPosition, qScreenRotation, screenScale));
		
		CR(m_pDreamEnvironmentApp->ShowEnvironment(nullptr));
		//*/

		CR(SetRoundtablePosition(seatPosition));
		m_fSeated = true;

		avatarID = m_pUserController->GetUser().GetAvatarID();

		if (GetHMD() != nullptr) {
			GetHMD()->GetHand(HAND_TYPE::HAND_LEFT)->PendCreateHandModel(avatarID);
			GetHMD()->GetHand(HAND_TYPE::HAND_RIGHT)->PendCreateHandModel(avatarID);
		}
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

	// Remote seating position
	long remoteSeatingPosition = (fOfferor) ? pPeerConnection->GetAnswererPosition() : pPeerConnection->GetOfferorPosition();

	DOSLOG(INFO, "OnNewDreamPeer local seat position %v", localSeatingPosition);
	//OVERLAY_DEBUG_SET("seat", (std::string("seat=") + std::to_string(localSeatingPosition)).c_str());

	if (!m_fSeated) {
		CR(SetRoundtablePosition(localSeatingPosition));

		m_fSeated = true;
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

	if (strAudioTrackLabel == kUserAudioLabel) {
		CR(HandleUserAudioDataMessage(pPeerConnection, &audioDataMessage));
	}
	else if (strAudioTrackLabel == kChromeAudioLabel) {
		// TODO: 
		//HANDLE that shit
	}

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
RESULT DreamGarage::HandleUserAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage) {
	RESULT r = R_PASS;

	/*
	long senderUserID = pPeerConnection->GetPeerUserID();
	user* pUser = ActivateUser(senderUserID);
	WCN(pUser);
	*/

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	CR(pDreamPeer->HandleUserAudioDataMessage(pAudioDataMessage));

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

RESULT DreamGarage::HandleDOSMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	auto pCloudController = GetCloudController();
	if (pCloudController != nullptr && pCloudController->IsUserLoggedIn() && pCloudController->IsEnvironmentConnected()) {
		// Resuming Dream functions if form was accessed out of Menu
		m_pDreamUserControlArea->OnDreamFormSuccess();
	}
	else {
		// once login has succeeded, save the launch date
		// environment id should have been set through DreamLoginApp responding to javascript
		if (strMessage == m_pDreamLoginApp->GetSuccessString()) {
			m_strAccessToken = m_pDreamLoginApp->GetAccessToken();

			CR(m_pDreamLoginApp->SetLaunchDate());

			// TODO: potentially where the lobby environment changes to the team environment
			// could also be once the environment id is set

			// TODO: populate user
			CR(m_pUserController->GetTeam(m_strAccessToken));
			CR(m_pUserController->RequestUserProfile(m_strAccessToken));
			CR(m_pUserController->RequestTwilioNTSInformation(m_strAccessToken));
		}
	}

Error:
	return r;
}

RESULT DreamGarage::OnLogin() {
	RESULT r = R_PASS;

	// TODO: choose environment based on api information
	// TODO: with seating pass, the cave will look better

	// the fade in now happens in OnNewSocketConnection
	// TODO: would definitely prefer UserController to respond to OnNewSocketConection so that 
	// it is a part of UpdateLoginState and the environment can fade in here

	//m_pDreamEnvironmentApp->SetCurrentEnvironment(CAVE);
	//CR(m_pDreamEnvironmentApp->SetCurrentEnvironment(ISLAND));
	//CR(m_pDreamEnvironmentApp->ShowEnvironment(nullptr));

	// TODO: uncomment when everything else works
	//CR(pUserController->RequestGetSettings(GetHardwareID(), GetHMDTypeString()));
	
//Error:
	return r;
}

RESULT DreamGarage::OnLogout() {
	RESULT r = R_PASS;

	UserController *pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

	// Show login form, given this is not the first launch.
	std::string strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);

	CNM(pUserController, "User controller was nullptr");

	CRM(m_pDreamLoginApp->ClearCredential(CREDENTIAL_REFRESH_TOKEN), "clearing refresh token failed");

	// Don't clear last login on logout
	//CRM(m_pDreamLoginApp->ClearCredential(CREDENTIAL_LAST_LOGIN), "clearing last login failed");

	CR(pUserController->GetFormURL(strFormType));
	CR(m_pDreamEnvironmentApp->HideEnvironment(nullptr));

	CRM(m_pDreamUserControlArea->ShutdownAllSources(), "failed to shutdown source");

	CRM(m_pDreamUserApp->GetBrowserManager()->DeleteCookies(), "deleting cookies failed");
	CRM(m_pDreamUserApp->ClearHands(), "failed to clear hands");

	// TODO: clear out DreamPeerApp and user assets

	m_fSeated = false;

Error:
	return r;
}

RESULT DreamGarage::OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) {
	RESULT r = R_PASS;

	FormType type = DreamFormApp::TypeFromString(strKey);
	DOSLOG(INFO, "OnFormURL: %s, %s, %s", strKey, strTitle, strURL);
	if (type == FormType::SETTINGS) {
	//	m_pDreamSettings->GetComposite()->SetVisible(true, false);
		CR(m_pDreamSettings->UpdateWithNewForm(strURL));
		CR(m_pDreamSettings->Show());
	}
	// the behavior of sign in, sign up, and teams create should be executed the same
	// way with regards to the functions that they use
	// TODO: potentially, the teams form will do other stuff later
	else if (type == FormType::SIGN_IN || type == FormType::SIGN_UP || type == FormType::TEAMS_MISSING) {
	//	m_pDreamLoginApp->GetComposite()->SetVisible(true, false);
		CR(m_pDreamLoginApp->UpdateWithNewForm(strURL));
		//CR(m_pDreamLoginApp->Show());

		// Login app doesn't show at the start, but it needs to receive the controller events in the lobby
		m_pDreamLoginApp->SetAsActive();
	}
	// TODO: general form?

Error:
	return r;
}

RESULT DreamGarage::OnAccessToken(bool fSuccess, std::string& strAccessToken) {
	RESULT r = R_PASS;

	if (!fSuccess) {

		std::string strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);

		m_pDreamLoginApp->ClearTokens();

		CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::INVALID_REFRESH_TOKEN));
		CR(m_pDreamUserApp->ShowMessageQuad());

		CR(m_pUserController->GetFormURL(strFormType));

		if (m_pDreamEnvironmentApp != nullptr) {
			CR(m_pDreamEnvironmentApp->FadeIn());
		}
	}
	else {
		// TODO: should be temporary
		m_strAccessToken = strAccessToken;

		CR(m_pDreamLoginApp->SetAccessToken(m_strAccessToken));
		//CR(m_pUserController->GetSettings(m_strAccessToken));
		CR(m_pUserController->RequestUserProfile(m_strAccessToken));
		CR(m_pUserController->RequestTwilioNTSInformation(m_strAccessToken));
		CR(m_pUserController->GetTeam(m_strAccessToken));
	}

Error:
	return r;
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

RESULT DreamGarage::OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) {
	RESULT r = R_PASS;

	if (!fSuccess) {
		// need to create a team, since the user has no teams
		std::string strFormType = DreamFormApp::StringFromType(FormType::TEAMS_MISSING);
	}
	else {
		CR(m_pDreamLoginApp->HandleDreamFormSetEnvironmentId(environmentId));
		CR(m_pDreamEnvironmentApp->SetCurrentEnvironment(environment::type(environmentModelId)));
	}

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

RESULT DreamGarage::OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) {
	RESULT r = R_PASS;

	// TODO: enum to string dictionary
	if (strKey == "FormKey.UsersSettings") {
		CR(m_pDreamSettings->UpdateWithNewForm(strURL));

		// more complicated form for testing until signup exists
		//CR(m_pDreamSettings->InitializeSettingsForm("https://www.develop.dreamos.com/forms/account/signup"));
		CR(m_pDreamSettings->Show());
	}
	else {
		CR(m_pDreamGeneralForm->UpdateWithNewForm(strURL));
		CR(m_pDreamGeneralForm->Show());
	}

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
