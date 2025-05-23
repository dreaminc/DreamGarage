#include "DreamMeetClient.h"

#include <string>
#include "chrono"                                       // for system_clock, system_clock::time_point
#include "stdint.h"                                     // for uint8_t
#include <corecrt_math_defines.h>                       // for M_PI

#include "core/Utilities.h"
#include "core/hand/hand.h"
#include "core/camera/camera.h"

#include "sandbox/CommandLineManager.h"
#include "sandbox/PathManager.h"

#include "DreamGarageMessage.h"
#include "UpdateHeadMessage.h"
#include "UpdateHandMessage.h"
#include "UpdateMouthMessage.h"
#include "AudioDataMessage.h"

#include "os/DreamMessage.h"                               // for DreamMessage

#include "apps/DreamEnvironmentApp/DreamEnvironmentApp.h"
#include "apps/DreamVCamApp/DreamVCamApp.h"
#include "apps/DreamSettingsApp/DreamSettingsApp.h"
#include "apps/DreamLoginApp/DreamLoginApp.h"
#include "apps/DreamFormApp/DreamFormApp.h"								// for FormType, DreamFormApp, FormType::SIGN_IN, FormType::CERTIFICATE_ERROR, FormType::LOAD_RESOURCE_ERROR, FormType::SIGN_UP_WELCOME, FormType::TEAMS_MISSING, FormType::SETTINGS, FormType::SIGN_UP
#include "apps/DreamPeerApp/DreamPeerApp.h"                             // for DreamPeerApp
#include "apps/DreamUserControlAreaApp/DreamUserControlAreaApp.h"  
#include "apps/DreamUserControlAreaApp/DreamContentSource.h"    
#include "apps/DreamShareViewApp/DreamShareViewApp.h"

#include "modules/DreamSoundSystem/DreamSoundSystem.h"               // for DreamSoundSystem, DreamSoundSystem::MIXDOWN_TARGET

// TODO: no OGL at this level
#include "hal/ogl/OGLProgram.h"                      // for OGLProgram
#include "hal/ogl/OGLProgramScreenFade.h"
#include "hal/ogl/OGLProgramReflection.h"
#include "hal/ogl/OGLProgramRefraction.h"
#include "hal/ogl/OGLProgramSkybox.h"
#include "hal/ogl/OGLProgramWater.h"

#include "hal/HALImp.h"                                 // for HALImp::HALConfiguration, HALImp
#include "hal/UIStageProgram.h"
#include "hal/EnvironmentProgram.h"
#include "hal/SkyboxScatterProgram.h"
#include "hal/FogProgram.h"

#include "cloud/Environment/PeerConnection.h"
#include "cloud/Environment/EnvironmentShare.h"
#include "cloud/Environment/EnvironmentController.h"    // for EnvironmentController
#include "cloud/User/User.h"                            // for User
#include "cloud/User/UserController.h"                  // for UserController
#include "cloud/webrtc/WebRTCPeerConnection.h"          // for WebRTCPeerConnectionProxy

#include "sound/SoundCommon.h"                          // for type, type::SIGNED_16_BIT
#include "sound/AudioPacket.h"

#include "pipeline/Pipeline.h"                      // for Pipeline
#include "pipeline/SinkNode.h"
#include "pipeline/SourceNode.h"
#include "pipeline/ProgramNode.h"

#include "scene/ObjectStoreNode.h"
#include "scene/CameraNode.h"

#include "webbrowser/CEFBrowser/CEFBrowserManager.h"

#include "core/hand/HandState.h"                  

#include "hmd/HMD.h"                                    // for HMD
#include "hmd/HMDFactory.h"                             // for ::HMD_ANY_AVAILABLE

#include "sense/SenseKeyboard.h"                        // for SenseKeyboardEvent, SenseTypingEvent (ptr only)

class SinkNode;
class light;
class stereocamera;

/* Comment this out to enable 3rd party camera
#define _USE_3RD_PARTY_CAMERA
//#define _USE_3RD_PARTY_CAMERA_HD
//#define _USE_3RD_PARTY_CAMERA_UHD
#define _USE_3RD_PARTY_CAMERA_HALF_UHD
//*/

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

RESULT DreamMeetClient::ConfigureSandbox() {
	RESULT r = R_PASS;

	Sandbox::configuration sandboxconfig;
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fUseLeap = false;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = false;
	sandboxconfig.fInitCloud = true;
	sandboxconfig.fInitSound = true;
	sandboxconfig.fInitUserApp = true;
	sandboxconfig.fInitNamedPipe = true;
	sandboxconfig.fInitKeyboard = true;

	sandboxconfig.fHideWindow = true;
	sandboxconfig.fHMDMirror = false;
	sandboxconfig.f3rdPersonCamera = false;
	sandboxconfig.hmdType = HMD_ANY_AVAILABLE;

	// Enable HMD mirror for non-production builds
#ifndef PRODUCTION_BUILD
	sandboxconfig.fHideWindow = false;
	sandboxconfig.fHMDMirror = true;
#endif

#ifdef PRODUCTION_BUILD
	sandboxconfig.hmdType = HMD_OPENVR;
#endif

#ifdef OCULUS_PRODUCTION_BUILD
	sandboxconfig.hmdType = HMD_OVR;
#endif

#ifdef _USE_3RD_PARTY_CAMERA
	sandboxconfig.fHideWindow = false;
	sandboxconfig.fHMDMirror = false;
	sandboxconfig.f3rdPersonCamera = true;
	sandboxconfig.fUseGamepad = true;
#endif

/*
#ifdef _DEBUG
	sandboxconfig.fUseHMD = true;
	sandboxconfig.fMouseLook = true;
	sandboxconfig.fUseGamepad = true;
	sandboxconfig.fInitSound = true;
	sandboxconfig.fHMDMirror = false;
	sandboxconfig.f3rdPersonCamera = true;
	sandboxconfig.fMouseLook = false;
	sandboxconfig.fHideWindow = false;
#endif
//*/

	SetSandboxConfiguration(sandboxconfig);

	// Set up API routes
	// Set up command line manager
	auto pCommandLineManager = CommandLineManager::instance();
	CN(pCommandLineManager);

	// TODO: Since DreamOS project doesn't get PRODUCTION pre-processors and the OCULUS_PRODUCTION_BUILD one is supposed to be temporary
	//		 This will need to be reworked at that time as well.
#ifdef PRODUCTION_BUILD
	CR(pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.dreamos.com:443"));
	CR(pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.dreamos.com:443"));
	CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.dreamos.com:443"));

	// Disable these in production
	CR(pCommandLineManager->DisableParameter("www.ip"));
	CR(pCommandLineManager->DisableParameter("api.ip"));
	CR(pCommandLineManager->DisableParameter("ws.ip"));
#else
	#ifdef USE_LOCALHOST
		// TODO: Localhost (docker) API / WWW 
		CR(pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.develop.dreamos.com:443"));
		CR(pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.develop.dreamos.com:443"));
		CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "ws://localhost:8000"));
	#else
		CR(pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.develop.dreamos.com:443"));
		CR(pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.develop.dreamos.com:443"));
		CR(pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.develop.dreamos.com:443"));
	#endif

#endif

Error:
	return r;
}

RESULT DreamMeetClient::SetupMirrorPipeline(Pipeline *pRenderPipeline) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = GetHALImp();

	SinkNode *pAuxSinkNode;
	pAuxSinkNode = pRenderPipeline->GetAuxiliarySinkNode();
	CNM(pAuxSinkNode, "Aux sink node isn't set");

	CR(pHAL->MakeCurrentContext());

	// Aux Camera
#if defined(_USE_3RD_PARTY_CAMERA_UHD)
	m_pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(3840, 2107, 60));
#elif defined(_USE_3RD_PARTY_CAMERA_HD)
	m_pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(1920, 1080, 60));
#elif defined(_USE_3RD_PARTY_CAMERA_HALF_UHD)
	m_pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 6.0f), viewport(2560, 1386, 60));
#else 
	m_pAuxCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), viewport(1280, 720, 60));
#endif

	CN(m_pAuxCamera);
	CB(m_pAuxCamera->incRefCount());

	{
		OGLProgram* pRenderProgramNode = nullptr;
		OGLProgram* pUIProgramNode = nullptr;
		MakePipeline(m_pAuxCamera, pRenderProgramNode, pUIProgramNode, Sandbox::PipelineType::MAIN);

		//m_pUIMirrorProgramNode = pUIProgramNode;
		m_pUIMirrorProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

		auto pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

		// Connect Program to Display
		// Connect to aux (we will likely need to reproduce the pipeline)
		if (pAuxSinkNode != nullptr) {
			CR(pAuxSinkNode->ConnectToInput("camera", m_pAuxCamera->Output("stereocamera")));
			CR(pAuxSinkNode->ConnectToInput("input_framebuffer", pUIProgramNode->Output("output_framebuffer")));
			//CR(pAuxSinkNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));
		}

		//CR(pHAL->ReleaseCurrentContext());
	}

Error:
	return r;
}

RESULT DreamMeetClient::MakePipeline(CameraNode* pCamera, OGLProgram* &pRenderNode, OGLProgram* &pEndNode, Sandbox::PipelineType pipelineType) {
	RESULT r = R_PASS;

	{
		ProgramNode* pScatteringSkyboxProgram;
		pScatteringSkyboxProgram = MakeProgramNode("skybox_scatter_cube");
		CN(pScatteringSkyboxProgram);
		CR(pScatteringSkyboxProgram->ConnectToInput("camera", pCamera->Output("stereocamera")));

		ProgramNode* pSkyboxConvolutionProgramNode;
		pSkyboxConvolutionProgramNode = MakeProgramNode("cubemap_convolution");
		CN(pSkyboxConvolutionProgramNode);
		CR(pSkyboxConvolutionProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));
		CR(pSkyboxConvolutionProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		// Reflection

		auto pReflectionProgramNode = MakeProgramNode("reflection");
		CN(pReflectionProgramNode);
		CR(pReflectionProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));

		ProgramNode* pReflectionSkyboxProgram;
		pReflectionSkyboxProgram = MakeProgramNode("skybox", PIPELINE_FLAGS::PASSTHRU);
		CN(pReflectionSkyboxProgram);
		CR(pReflectionSkyboxProgram->ConnectToInput("camera", pCamera->Output("stereocamera")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pReflectionSkyboxProgram->ConnectToInput("input_framebuffer", pReflectionProgramNode->Output("output_framebuffer")));

		// Refraction

		auto pRefractionProgramNode = MakeProgramNode("refraction");
		CN(pRefractionProgramNode);
		CR(pRefractionProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));

		// "Water"

		ProgramNode* pWaterProgramNode = MakeProgramNode("water");
		CN(pWaterProgramNode);
		// Still need scene graph for lights
		// TODO: make lights a different node
		CR(pWaterProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));

		// TODO: This is not particularly general yet
		// Uncomment below to turn on water effects
		CR(pWaterProgramNode->ConnectToInput("input_refraction_map", pRefractionProgramNode->Output("output_framebuffer")));
		CR(pWaterProgramNode->ConnectToInput("input_reflection_map", pReflectionSkyboxProgram->Output("output_framebuffer")));

		// Environment shader

		auto pRenderEnvironmentProgramNode = MakeProgramNode("environment", PIPELINE_FLAGS::PASSTHRU);
		CN(pRenderEnvironmentProgramNode);
		CR(pRenderEnvironmentProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));

		CR(pRenderEnvironmentProgramNode->ConnectToInput("input_framebuffer", pWaterProgramNode->Output("output_framebuffer")));

		// Everything else
		ProgramNode* pRenderProgramNode = MakeProgramNode("standard", PIPELINE_FLAGS::PASSTHRU);
		CN(pRenderProgramNode);
		if (static_cast<int>(pipelineType & Sandbox::PipelineType::MAIN) != 0) {
			CR(pRenderProgramNode->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		}
		else {
			CR(pRenderProgramNode->ConnectToInput("scenegraph", GetAuxSceneGraphNode()->Output("objectstore")));
		}
		CR(pRenderProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer_irradiance_cubemap", pSkyboxConvolutionProgramNode->Output("output_framebuffer_cube")));
		CR(pRenderProgramNode->ConnectToInput("input_framebuffer_environment_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		// NOTE: Add this in if you want to have reflective objects
		//CR(pRenderProgramNode->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));

		CR(pRenderProgramNode->ConnectToInput("input_framebuffer", pRenderEnvironmentProgramNode->Output("output_framebuffer")));

		// Reference Geometry Shader Program
		ProgramNode* pReferenceGeometryProgram = MakeProgramNode("reference", PIPELINE_FLAGS::PASSTHRU);
		CN(pReferenceGeometryProgram);
		CR(pReferenceGeometryProgram->ConnectToInput("scenegraph", GetSceneGraphNode()->Output("objectstore")));
		CR(pReferenceGeometryProgram->ConnectToInput("camera", pCamera->Output("stereocamera")));

		CR(pReferenceGeometryProgram->ConnectToInput("input_framebuffer", pRenderProgramNode->Output("output_framebuffer")));

		// Skybox
		ProgramNode* pSkyboxProgram;
		pSkyboxProgram = MakeProgramNode("skybox", PIPELINE_FLAGS::PASSTHRU);
		CN(pSkyboxProgram);
		CR(pSkyboxProgram->ConnectToInput("camera", pCamera->Output("stereocamera")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer_cubemap", pScatteringSkyboxProgram->Output("output_framebuffer_cube")));
		CR(pSkyboxProgram->ConnectToInput("input_framebuffer", pReferenceGeometryProgram->Output("output_framebuffer")));

		ProgramNode* pUIProgramNode = MakeProgramNode("uistage", PIPELINE_FLAGS::PASSTHRU);
		CN(pUIProgramNode);
		if (static_cast<int>(pipelineType & Sandbox::PipelineType::MAIN) != 0) {
			CR(pUIProgramNode->ConnectToInput("scenegraph", GetUISceneGraphNode()->Output("objectstore")));
			CR(pUIProgramNode->ConnectToInput("clippingscenegraph", GetUIClippingSceneGraphNode()->Output("objectstore")));
		}
		else {
			CR(pUIProgramNode->ConnectToInput("scenegraph", GetAuxUISceneGraphNode()->Output("objectstore")));
		}
		CR(pUIProgramNode->ConnectToInput("camera", pCamera->Output("stereocamera")));

		// Connect output as pass-thru to internal blend program
		CR(pUIProgramNode->ConnectToInput("input_framebuffer", pSkyboxProgram->Output("output_framebuffer")));

		pRenderNode = dynamic_cast<OGLProgram*>(pRenderProgramNode);
		CN(pRenderNode);

		pEndNode = dynamic_cast<OGLProgram*>(pUIProgramNode);
		CN(pEndNode);

		// save interfaces to skybox nodes
		m_skyboxProgramNodes.push_back(dynamic_cast<SkyboxScatterProgram*>(pScatteringSkyboxProgram));
		//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pReflectionSkyboxProgram));
		//m_skyboxProgramNodes.emplace_back(dynamic_cast<SkyboxScatterProgram*>(pSkyboxProgram));

		// save interfaces to fog nodes
		m_fogProgramNodes.push_back(dynamic_cast<FogProgram*>(pRenderEnvironmentProgramNode));
		m_fogProgramNodes.push_back(dynamic_cast<FogProgram*>(pRefractionProgramNode));
		m_fogProgramNodes.push_back(dynamic_cast<FogProgram*>(pReflectionProgramNode));

		// save interface to water node
		m_waterProgramNodes.push_back(pWaterProgramNode);

		if (m_pWaterQuad == nullptr) {
			m_pWaterQuad = MakeQuad(1000.0f, 1000.0f);
			point ptQuadOffset = point(90.0f, -2.38f, -25.0f);
			m_pWaterQuad->SetPosition(ptQuadOffset);
			m_pWaterQuad->SetMaterialColors(color(57.0f / 255.0f, 112.0f / 255.0f, 151.0f / 255.0f, 1.0f));
		}
		CN(m_pWaterQuad);

		if (pWaterProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramWater*>(pWaterProgramNode)->SetPlaneObject(m_pWaterQuad));
		}

		if (pReflectionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramReflection*>(pReflectionProgramNode)->SetReflectionObject(m_pWaterQuad));
		}

		if (pRefractionProgramNode != nullptr) {
			CR(dynamic_cast<OGLProgramRefraction*>(pRefractionProgramNode)->SetRefractionObject(m_pWaterQuad));
		}

		if (pReflectionSkyboxProgram != nullptr) {
			CR(dynamic_cast<OGLProgramSkybox*>(pReflectionSkyboxProgram)->SetReflectionObject(m_pWaterQuad));
		}

		if(m_pDreamEnvironmentApp == nullptr) {			// Pipelines made before Environment app will need to get the scenegraph node from it in LoadScene() apparently
			if (m_pReflectionProgramNode == nullptr) {	// assumes main pipeline is the first one made
				m_pReflectionProgramNode = pReflectionProgramNode;
				m_pRefractionProgramNode = pRefractionProgramNode;
				m_pRenderEnvironmentProgramNode = pRenderEnvironmentProgramNode;
			}
			else if (m_pReflectionProgramNodeMirror == nullptr && GetSandboxConfiguration().f3rdPersonCamera) {	// an extra check in case we add more pipelines pre-environment
				m_pReflectionProgramNodeMirror = pReflectionProgramNode;
				m_pRefractionProgramNodeMirror = pRefractionProgramNode;
				m_pRenderEnvironmentProgramNodeMirror = pRenderEnvironmentProgramNode;	
			}
		}
		else {
			CN(pRenderEnvironmentProgramNode);
			CR(pRenderEnvironmentProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));
			
			CN(pReflectionProgramNode);
			CR(pReflectionProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

			CN(pRefractionProgramNode);
			CR(pRefractionProgramNode->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

			CR(m_pDreamEnvironmentApp->SetFogPrograms(m_fogProgramNodes));
			CR(m_pDreamEnvironmentApp->SetSkyboxPrograms(m_skyboxProgramNodes));
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::SetupPipeline(Pipeline* pRenderPipeline) {
	RESULT r = R_PASS;

	// Set up the pipeline
	HALImp *pHAL = GetHALImp();

	SinkNode* pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	//CR(pHAL->MakeCurrentContext());

	{
		OGLProgram* pRenderProgramNode = nullptr;
		OGLProgram* pUIProgramNode = nullptr;
		MakePipeline(GetCameraNode(), pRenderProgramNode, pUIProgramNode, Sandbox::PipelineType::MAIN);

		// save interface for UI apps
		m_pUIProgramNode = dynamic_cast<UIStageProgram*>(pUIProgramNode);

		auto pEnvironmentNode = dynamic_cast<EnvironmentProgram*>(pRenderProgramNode);

		HMD* pHMD = GetHMD();
		if (pHMD != nullptr) {
			bool fARHMD = pHMD->IsARHMD();

			m_pUIProgramNode->SetIsAugmented(fARHMD);
			pEnvironmentNode->SetIsAugmented(fARHMD);
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
	}

	if (GetSandboxConfiguration().f3rdPersonCamera == true) {
		CR(SetupMirrorPipeline(pRenderPipeline));
	}

Error:
	return r;
}

RESULT DreamMeetClient::SetupUserModelPool() {
	RESULT r = R_PASS;

	// Set up user pool
	for (int i = 0; i < MAX_PEERS; i++) {
		m_usersModelPool[i] = std::make_pair<DreamPeerApp*, user*>(nullptr, MakeUser());
		//m_usersModelPool[i].second->SetVisible(false);
	}

//Error:
	return r;
}

RESULT DreamMeetClient::AllocateAndAssignUserModelFromPool(DreamPeerApp *pDreamPeer) {
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

RESULT DreamMeetClient::UnallocateUserModelFromPool(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	for (auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer.get()) {
			// release model and set to invisible
			//pDreamPeer->GetUserModel()->GetMouth()->SetVisible(false);
			/*
			if (userModelPair.first != nullptr) {
				auto pLabelComposite = userModelPair.first->GetUserLabelComposite();
				if (userModelPair.first->GetUserLabelComposite() != n->SetVisible(false);
			}
			//*/
			//userModelPair.first = nullptr;
			if (userModelPair.second != nullptr) {
				userModelPair.second->SetVisible(false);
				if (userModelPair.second->GetMouth() != nullptr) {
					userModelPair.second->GetMouth()->SetVisible(false);
				}
			}
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

user* DreamMeetClient::FindUserModelInPool(DreamPeerApp *pDreamPeer) {
	for (const auto& userModelPair : m_usersModelPool) {
		if (userModelPair.first == pDreamPeer) {
			return userModelPair.second;
		}
	}

	return nullptr;
}

RESULT DreamMeetClient::LoadScene() {
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

	if (GetSandboxConfiguration().f3rdPersonCamera) {
		CN(m_pRenderEnvironmentProgramNodeMirror);
		CR(m_pRenderEnvironmentProgramNodeMirror->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

		CN(m_pReflectionProgramNodeMirror);
		CR(m_pReflectionProgramNodeMirror->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));

		CN(m_pRefractionProgramNodeMirror);
		CR(m_pRefractionProgramNodeMirror->ConnectToInput("scenegraph", m_pDreamEnvironmentApp->GetSceneGraphNode()->Output("objectstore")));
	}

	CR(SetupUserModelPool());
	DOSLOG(INFO, "UserModelPool has been setup");

	AddSkybox();
	DOSLOG(INFO, "Added Skybox");

Error:
	return r;
}

std::shared_ptr<DreamPeerApp> g_pDreamPeerApp = nullptr;

RESULT DreamMeetClient::DidFinishLoading() {
	RESULT r = R_PASS;

	std::string strFormType;

	// what used to be in this function is now in DreamUserControlArea::InitializeApp
	//auto pDreamUserApp = LaunchDreamApp<DreamUserApp>(this, GetSandboxConfiguration().f3rdPersonCamera);
	//auto pDreamUserApp = LaunchDreamApp<DreamUserApp>(this, false);

	m_pDreamUserControlArea = LaunchDreamApp<DreamUserControlAreaApp>(this, false).get();
	CN(m_pDreamUserControlArea);

	m_pDreamUserControlArea->SetDreamUserApp(GetUserApp());
	m_pDreamUserControlArea->SetUIProgramNode(m_pUIProgramNode);

	if (m_pDreamEnvironmentApp != nullptr) {
		m_pDreamEnvironmentApp->SetSkyboxPrograms(m_skyboxProgramNodes);
		m_pDreamEnvironmentApp->SetScreenFadeProgram(m_pScreenFadeProgramNode);
		m_pDreamEnvironmentApp->SetFogPrograms(m_fogProgramNodes);
	}

	m_pDreamShareViewApp = LaunchDreamApp<DreamShareViewApp>(this, false);
	CN(m_pDreamShareViewApp);

	m_pDreamSettings = LaunchDreamApp<DreamSettingsApp>(this, false);
	CN(m_pDreamSettings);

	m_pDreamLoginApp = LaunchDreamApp<DreamLoginApp>(this, false);
	CN(m_pDreamLoginApp);

	m_pDreamGeneralForm = LaunchDreamApp<DreamFormApp>(this, false);
	CN(m_pDreamSettings);

	if (GetSandboxConfiguration().fUseGamepad) {
		m_pDreamGamepadCameraApp = LaunchDreamApp<DreamGamepadCameraApp>(this, false).get();
		CN(m_pDreamGamepadCameraApp);

		if (m_pAuxCamera != nullptr) {
			CR(m_pDreamGamepadCameraApp->SetCamera(m_pAuxCamera, DreamGamepadCameraApp::CameraControlType::GAMEPAD));
		}
		else {
			CR(m_pDreamGamepadCameraApp->SetCamera(GetCamera(), DreamGamepadCameraApp::CameraControlType::GAMEPAD));
		}
	}

	// TODO: could be somewhere else(?)
	CR(RegisterDOSObserver(this));

	m_fFirstLogin = m_pDreamLoginApp->IsFirstLaunch();
	m_fHasCredentials = m_pDreamLoginApp->HasStoredCredentials(m_strRefreshToken, m_strAccessToken);

	// TODO: This might need to be reworked
	CRM(GetCloudController()->Start(false), "Failed to start cloud controller");

	// UserController is initialized during CloudController::Initialize,
	// which is in Sandbox::Initialize while fInitCloud is true
	m_pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	CN(m_pUserController);

	{
		std::string strCertificateErrorFormKey = DreamFormApp::StringFromType(FormType::CERTIFICATE_ERROR);
		m_pUserController->RequestFormURL(strCertificateErrorFormKey);
		
		std::string strLoadErrorFormKey = DreamFormApp::StringFromType(FormType::LOAD_RESOURCE_ERROR);
		m_pUserController->RequestFormURL(strLoadErrorFormKey);
	}

	// DEBUG:
#ifdef _DEBUG
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
			return m_pUserController->RequestAccessToken(strDebugRefreshToken);
		}
	}
#endif

	// Initial step of login flow:
	DOSLOG(INFO, "Checking API connection (internet access)");
	CRM(m_pUserController->CheckAPIConnection(), "Checking connection to API");

Error:
	return r;
}

RESULT DreamMeetClient::OnDreamVersion(version dreamVersion) {
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

RESULT DreamMeetClient::OnAPIConnectionCheck(bool fIsConnected) {
	RESULT r = R_PASS;

	if (fIsConnected) {
#if defined(PRODUCTION_BUILD) || defined(OCULUS_PRODUCTION_BUILD) || defined(STAGING_BUILD)
		CR(m_pUserController->RequestDreamVersion());
		//*
#else
		DOSLOG(INFO, "API check ok, authing from stored creds");
		CR(AuthenticateFromStoredCredentials());
#endif
//*/
	}
	else {
		DOSLOG(INFO, "API check failed, showing internet required");

		m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::INTERNET_REQUIRED);
		m_pDreamUserApp->ShowMessageQuad();
		m_pDreamEnvironmentApp->FadeIn();
	}

Error:
	return r;
}

version DreamMeetClient::GetDreamVersion() {
	return m_versionDreamClient;
}

CameraNode *DreamMeetClient::GetAuxCameraNode() {
	return m_pAuxCamera;
}

RESULT DreamMeetClient::AuthenticateFromStoredCredentials() {
	RESULT r = R_PASS;

	std::string strFormType;
	// if there has already been a successful login, try to authenticate
	if (!m_fFirstLogin && m_fHasCredentials) {
		DOSLOG(INFO, "Not first login and has creds");
		m_pUserController->RequestAccessToken(m_strRefreshToken);
	}
	else {
		// Otherwise, start by showing the login form

		if (!m_fFirstLogin) {
			DOSLOG(INFO, "Not first login, going to sign-in");
			strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);
			CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::SIGN_IN));
		}
		else {
			DOSLOG(INFO, "Is first login, going to sign-up");
			//strFormType = DreamFormApp::StringFromType(FormType::SIGN_UP);
			strFormType = DreamFormApp::StringFromType(FormType::SIGN_UP_WELCOME);
			CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::WELCOME));
		}

		CR(m_pDreamUserApp->ShowMessageQuad());

		CR(m_pUserController->RequestFormURL(strFormType));

		if (m_pDreamEnvironmentApp != nullptr) {
			// fade into lobby (with no environment showing)
			CR(m_pDreamEnvironmentApp->FadeIn());
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHeadMessage updateHeadMessage(GetUserID(), userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);
	CR(SendDataMessage(userID, &updateHeadMessage));

Error:
	return r;
}

RESULT DreamMeetClient::SendUpdateHandMessage(long userID, HandState handState) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHandMessage updateHandMessage(GetUserID(), userID, handState);
	CR(SendDataMessage(userID, &updateHandMessage));

Error:
	return r;
}

RESULT DreamMeetClient::BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	RESULT r = R_PASS;
	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;


	// Create the message
	UpdateHeadMessage updateHeadMessage(GetUserID(), -1, ptPosition, qOrientation, vVelocity, qAngularVelocity);
	CR(BroadcastDataMessage(&updateHeadMessage));

Error:
	return r;
}

RESULT DreamMeetClient::BroadcastUpdateHandMessage(HandState handState) {
	RESULT r = R_PASS;

	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateHandMessage updateHandMessage(GetUserID(), -1, handState);
	CR(BroadcastDataMessage(&updateHandMessage));

Error:
	return r;
}

RESULT DreamMeetClient::BroadcastUpdateMouthMessage(float mouthSize) {
	RESULT r = R_PASS;

	uint8_t *pDatachannelBuffer = nullptr;
	int pDatachannelBuffer_n = 0;

	// Create the message
	UpdateMouthMessage updateMouthMessage(GetUserID(), -1, mouthSize);
	CR(BroadcastDataMessage(&updateMouthMessage));

Error:
	return r;
}

RESULT DreamMeetClient::SendHeadPosition() {
	RESULT r = R_PASS;

	point ptPosition = GetCameraPosition();
	quaternion qOrientation = GetCameraOrientation();

	CR(BroadcastUpdateHeadMessage(ptPosition, qOrientation));

Error:
	return r;
}

RESULT DreamMeetClient::SendHandPosition() {
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

RESULT DreamMeetClient::SendMouthSize() {
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

RESULT DreamMeetClient::Update(void) {
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
	std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

	///*
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

	// Periodically check peer app states
	if (std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - g_lastPeerStateCheckTime).count() > CHECK_PEER_APP_STATE_INTERVAL_MS) {
		CR(CheckDreamPeerAppStates());
		g_lastPeerStateCheckTime = timeNow;
	}
	//*/

	if (m_fInitHands) {
		//CRM(m_pDreamUserApp->ClearHands(), "failed to clear hands");
		//CR(ClearPeers());
	}

	if (m_fPendLogout) {

		EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		if(pEnvironmentController != nullptr && m_pUserController != nullptr) {
			if (!pEnvironmentController->HasPeerConnections()) {
				CR(m_pUserController->Logout());
			}
		}

		m_fPendLogout = false;
	}

	if (m_fPendSwitchTeams) {

		EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		if(pEnvironmentController != nullptr && m_pUserController != nullptr) {
			if (!pEnvironmentController->HasPeerConnections()) {
				CR(m_pUserController->SwitchTeam());
			}
		}

		m_fPendSwitchTeams = false;
	}

	if (m_fPendExit) {
		EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		if(pEnvironmentController != nullptr && m_pUserController != nullptr) {
			if (!pEnvironmentController->HasPeerConnections()) {
				CR(DreamOS::Exit(r));
			}
		}

		m_fPendExit = false;
	}

Error:
	return r;
}

RESULT DreamMeetClient::SetRoundtablePosition(int seatingPosition) {
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
	}

	if (m_pDreamUserControlArea != nullptr) {
		m_pDreamUserControlArea->ResetAppComposite();
	}
	m_pDreamUserApp->ResetAppComposite();
	m_pDreamUserApp->SetSeatingPosition(seatingPosition);

Error:
	return r;
}

RESULT DreamMeetClient::SetRoundtablePosition(DreamPeerApp *pDreamPeer, int seatingPosition) {
	RESULT r = R_PASS;

	point ptSeatPosition;
	quaternion qRotation;
	vector vCameraDirection;
	vector vCameraDifference;

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->GetEnvironmentSeatingPositionAndOrientation(ptSeatPosition, qRotation, seatingPosition));

	pDreamPeer->SetOrientation(qRotation);
	pDreamPeer->SetPosition(ptSeatPosition);

	// update username label
	vCameraDirection = ptSeatPosition - GetCamera()->GetPosition(true);
	vCameraDirection = vector(vCameraDirection.x(), 0.0f, vCameraDirection.z()).Normal();

//	pDreamPeer->SetUserLabelPosition(ptSeatPosition);

	// Making a quaternion with two vectors uses cross product,
	// vector(0,0,1) and vector(0,0,-1) are incompatible with vector(0,0,-1)
	if (vCameraDirection == vector::kVector(1.0f)) {
		pDreamPeer->SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, (float)M_PI, 0.0f));
	}
	else if (vCameraDirection == vector::kVector(-1.0f)) {
		pDreamPeer->SetUserLabelOrientation(quaternion::MakeQuaternionWithEuler(0.0f, 0.0f, 0.0f));
	}
	else {
		pDreamPeer->SetUserLabelOrientation(quaternion(vector::kVector(-1.0f), vCameraDirection));
	}

Error:
	return r;
}

// Cloud Controller

RESULT DreamMeetClient::OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) {
	RESULT r = R_PASS;

	CR(m_pDreamShareViewApp->DeallocateSpheres(pDreamPeer->GetPeerUserID()));

	CR(UnallocateUserModelFromPool(pDreamPeer));

Error:
	return r;
}

RESULT DreamMeetClient::OnNewSocketConnection(int seatPosition) {
	RESULT r = R_PASS;

	if (!m_fSeated) {
		//*
		point ptScreenPosition;
		quaternion qScreenRotation;
		float screenScale;

		long avatarID;

		auto fnOnFadeInCallback = [&](void *pContext) {
			
			if (m_fFirstLogin) {
				m_pDreamGeneralForm->Show();
				m_pDreamUserApp->ResetAppComposite();
			}
			
			return R_PASS;
		};

		CR(m_pDreamEnvironmentApp->GetSharedScreenPlacement(ptScreenPosition, qScreenRotation, screenScale));
		CR(m_pDreamShareViewApp->UpdateScreenPosition(ptScreenPosition, qScreenRotation, screenScale));

		//CR(m_pDreamEnvironmentApp->ShowEnvironment(nullptr, fnOnFadeInCallback));
		CR(m_pDreamEnvironmentApp->ShowEnvironment(nullptr));
		//*/

		CR(SetRoundtablePosition(seatPosition));
		m_fSeated = true;

		avatarID = m_pUserController->GetUser().GetAvatarID();

		if (!m_fInitHands && GetHMD() != nullptr) {
			auto pLeftHand = GetHMD()->GetHand(HAND_TYPE::HAND_LEFT);
			pLeftHand->PendCreateHandModel(avatarID);

			auto pRightHand = GetHMD()->GetHand(HAND_TYPE::HAND_RIGHT);
			pRightHand->PendCreateHandModel(avatarID);
			m_fInitHands = true;
		}

		if (GetHMD() != nullptr) {
			CR(m_pDreamUserApp->SetEventApp(nullptr));
			CR(m_pDreamUserApp->SetHasOpenApp(false));
		}

		CR(m_pDreamUserApp->HideMessageQuad());
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnNewDreamPeer(DreamPeerApp *pDreamPeer) {
	RESULT r = R_PASS;

	std::vector<std::shared_ptr<EnvironmentShare>> pendingDeleteShares;
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
	pDreamPeer->SetSeatingPosition(remoteSeatingPosition);

	CR(SetRoundtablePosition(pDreamPeer, remoteSeatingPosition));
	pDreamPeer->SetVisible(true);

	// Turn on sound
	WebRTCPeerConnectionProxy *pWebRTCPeerConnectionProxy;
	pWebRTCPeerConnectionProxy = GetWebRTCPeerConnectionProxy(pPeerConnection);
	CN(pWebRTCPeerConnectionProxy);

	if (pWebRTCPeerConnectionProxy != nullptr) {
		pWebRTCPeerConnectionProxy->SetAudioVolume(1.0f);
	}

	/*
	if (pPeerConnection->GetPeerUserID() == m_pendingAssetReceiveUserID) {
		m_pDreamShareView->StartReceiving(pPeerConnection);
		m_pendingAssetReceiveUserID = -1;
	}
	//*/


	for (int i = 0; i < m_pPendingEnvironmentShares.size(); i++) {
		auto pPendingShare = m_pPendingEnvironmentShares[i];
		if (pPendingShare != nullptr && pPeerConnection->GetPeerUserID() == pPendingShare->GetUserID()) {
			if (pPendingShare->GetShareType() == SHARE_TYPE_SCREEN) {
				m_pDreamShareViewApp->StartReceiving(pPeerConnection);
			}
			if (pPendingShare->GetShareType() == SHARE_TYPE_CAMERA) {
				m_pDreamUserControlArea->GetVCam()->StartReceiving(pPeerConnection, pPendingShare);
			}
			pendingDeleteShares.emplace_back(pPendingShare);
		}
	}

	for (auto deleteShare : pendingDeleteShares) {
		m_pPendingEnvironmentShares.erase(std::find(m_pPendingEnvironmentShares.begin(), m_pPendingEnvironmentShares.end(), deleteShare));
	}

	if (m_pDreamUserControlArea->IsSharingScreen()) {
		m_pDreamUserControlArea->SendFirstFrame();
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) {
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

user* DreamMeetClient::ActivateUser(long userId) {
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
		}
	}

	return m_peerUsers[userId];
	*/

	// TODO:

	return nullptr;
}

RESULT DreamMeetClient::OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) {
	RESULT r = R_PASS;

	long senderUserID = pPeerConnection->GetPeerUserID();
	long recieverUserID = pPeerConnection->GetUserID();

	AudioDataMessage audioDataMessage(senderUserID, recieverUserID, pAudioDataBuffer, bitsPerSample, samplingRate, channels, frames);

	if (strAudioTrackLabel == kUserAudioLabel) {
		int channel = (int)pPeerConnection->GetPeerSeatPosition();
		channel += 2;

		//
		//// TODO: Move this all into DreamSoundSystem or lower
		//int16_t *pInt16Soundbuffer = new int16_t[frames];
		//memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);
		//

		AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, sound::type::SIGNED_16_BIT, (uint8_t*)pAudioDataBuffer);
		CR(m_pDreamSoundSystem->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, channel));

		// hack to make them connect?
		pendingPacket.SetSoundType(sound::type::SIGNED_16_BIT);
		
		// Send audio to Mixdown
		DreamSoundSystem::MIXDOWN_TARGET mixdownTarget = 
			(DreamSoundSystem::MIXDOWN_TARGET)((int)(DreamSoundSystem::MIXDOWN_TARGET::LOCAL_MIC) + channel);
		CR(PushAudioPacketToMixdown(mixdownTarget, (int)frames, pendingPacket));

		// Sets the mouth position
		CR(HandleUserAudioDataMessage( pPeerConnection, &audioDataMessage));
	}
	else if (strAudioTrackLabel == kChromeAudioLabel) {

		// Only stream when it's the user that's currently sharing
		PeerConnection *pStreamingPeerConnection = m_pDreamShareViewApp->GetStreamingPeerConnection();

		if (pStreamingPeerConnection != nullptr && pStreamingPeerConnection->GetPeerUserID() == pPeerConnection->GetPeerUserID()) {

			int channel = 0;

			//CR(m_pDreamShareView->HandleChromeAudioDataMessage(pPeerConnection, &audioDataMessage));

			//int16_t *pInt16Soundbuffer = new int16_t[frames];
			//memcpy((void*)pInt16Soundbuffer, pAudioDataBuffer, sizeof(int16_t) * frames);

			AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, sound::type::SIGNED_16_BIT, (uint8_t*)pAudioDataBuffer);
			CR(m_pDreamSoundSystem->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, channel));

			// hack to make them connect?
			// same as with peer mic
			pendingPacket.SetSoundType(sound::type::SIGNED_16_BIT);

			// Send audio to Mixdown
			DreamSoundSystem::MIXDOWN_TARGET mixdownTarget =
				(DreamSoundSystem::MIXDOWN_TARGET)((int)(DreamSoundSystem::MIXDOWN_TARGET::REMOTE_BROWSER_MONO_0) + channel);
			CR(PushAudioPacketToMixdown(mixdownTarget, (int)frames, pendingPacket));
		}
	}
	else if (strAudioTrackLabel == kVCamAudiolabel) {
		if (m_pCameraVideoStreamPeerConnectionSource != nullptr && m_pCameraVideoStreamPeerConnectionSource->GetPeerUserID() == pPeerConnection->GetPeerUserID()) {
			int channel = 1;

			AudioPacket pendingPacket((int)frames, (int)channels, (int)bitsPerSample, (int)samplingRate, (uint8_t*)pAudioDataBuffer);
			CR(m_pDreamSoundSystem->PlayAudioPacketSigned16Bit(pendingPacket, strAudioTrackLabel, channel));
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::HandleHeadUpdateMessage(PeerConnection* pPeerConnection, UpdateHeadMessage *pUpdateHeadMessage) {
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

RESULT DreamMeetClient::HandleHandUpdateMessage(PeerConnection* pPeerConnection, UpdateHandMessage *pUpdateHandMessage) {
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

	HandState handState = pUpdateHandMessage->GetHandState();

	auto pDreamPeer = FindPeer(pPeerConnection);
	CN(pDreamPeer);

	pDreamPeer->UpdateHand(handState);

Error:
	return r;
}

RESULT DreamMeetClient::HandleMouthUpdateMessage(PeerConnection* pPeerConnection, UpdateMouthMessage *pUpdateMouthMessage) {
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
RESULT DreamMeetClient::HandleUserAudioDataMessage(PeerConnection* pPeerConnection, AudioDataMessage *pAudioDataMessage) {
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

RESULT DreamMeetClient::OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (m_pDreamUserControlArea != nullptr) {
		//CR(m_pDreamUserControlArea->AddEnvironmentAsset(pEnvironmentAsset));
		CR(m_pDreamUserControlArea->PendEnvironmentAsset(pEnvironmentAsset));
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	if (m_pDreamUserControlArea != nullptr) {
		CR(m_pDreamUserControlArea->CloseActiveAsset());
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) {
	RESULT r = R_PASS;

	auto pUserControllerProxy = (UserControllerProxy*)(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
	CN(pUserControllerProxy);
	pUserControllerProxy->RequestGetSettings(m_strAccessToken);
	CN(m_pDreamUserControlArea->PendCameraEnvironmentAsset(pEnvironmentAsset));

Error:
	return r;
}

RESULT DreamMeetClient::OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) { 
	RESULT r = R_PASS;

//	m_pDreamUserControlArea->GetA
	if (m_pDreamUserControlArea != nullptr) {

		if (pEnvironmentAsset->GetAssetID() == m_pDreamUserControlArea->GetActiveSource()->GetCurrentAssetID()) {
			CR(m_pDreamUserControlArea->CloseActiveAsset());
		}
		else {
			CR(m_pDreamUserControlArea->CloseCameraTab());
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnSendCameraPlacement() {
	RESULT r = R_PASS;

	CN(m_pDreamUserControlArea);
	CR(m_pDreamUserControlArea->AddEnvironmentCameraAsset());

	CR(m_pDreamUserControlArea->GetVCam()->SetIsSendingCameraPlacement(true));

Error:
	return r;
}

RESULT DreamMeetClient::OnStopSendingCameraPlacement() {
	RESULT r = R_PASS;

	CR(m_pDreamUserControlArea->GetVCam()->SetIsSendingCameraPlacement(false));

Error:
	return r;
}

RESULT DreamMeetClient::OnReceiveCameraPlacement(long userID) {
	RESULT r = R_PASS;

	CR(m_pDreamUserControlArea->GetVCam()->SetIsReceivingCameraPlacement(true));

Error:
	return r;
}

RESULT DreamMeetClient::OnStopReceivingCameraPlacement() {
	RESULT r = R_PASS;

	CR(m_pDreamUserControlArea->GetVCam()->SetIsReceivingCameraPlacement(false));

Error:
	return r;
}

RESULT DreamMeetClient::SaveCameraSettings(point ptPosition, quaternion qOrientation) {
	RESULT r = R_PASS;

	CN(m_pUserController);
	CR(m_pUserController->RequestSetSettings(m_strAccessToken, ptPosition, qOrientation));

Error:
	return r;
}

RESULT DreamMeetClient::HandleDOSMessage(std::string& strMessage) {
	RESULT r = R_PASS;

	auto pCloudController = GetCloudController();
	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	if (strMessage == "DreamShareView.IsActive") {
		m_pDreamUserControlArea->UpdateIsActive(true);
	}
	else if (strMessage == "DreamShareView.IsNotActive") {
	//	m_pDreamUserControlArea->UpdateIsActive(false);
		CR(pEnvironmentControllerProxy->RequestCurrentScreenShare(SHARE_TYPE_SCREEN));
	}

	else if (strMessage == "UIKeyboard.FormCancel") {
		// for sign up/sign in forms, reload the original form when the login method is canceled
		CR(m_pDreamLoginApp->ResetForm());
	}

	else if (pCloudController != nullptr && pCloudController->IsUserLoggedIn() && pCloudController->IsEnvironmentConnected()) {
		// Resuming Dream functions if form was accessed out of Menu
		if (strMessage == "DreamEnvironmentApp.OnFadeIn") {
			m_pDreamUserApp->ResetAppComposite();
		}
		// DreamFormApp success or DreamLoginApp success
		else {
			m_pDreamUserControlArea->OnDreamFormSuccess();
		}
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
			CR(m_pUserController->RequestTeam(m_strAccessToken));
			CR(m_pUserController->RequestUserProfile(m_strAccessToken));
			CR(m_pUserController->RequestTwilioNTSInformation(m_strAccessToken));
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnLogin() {
	RESULT r = R_PASS;

	// TODO: choose environment based on api information
	// TODO: with seating pass, the cave will look better

	// the fade in now happens in OnNewSocketConnection
	// TODO: would definitely prefer UserController to respond to OnNewSocketConection so that
	// it is a part of UpdateLoginState and the environment can fade in here

	//m_pDreamEnvironmentApp->SetCurrentEnvironment(CAVE);
	//CR(m_pDreamEnvironmentApp->SetCurrentEnvironment(ISLAND));
	//CR(m_pDreamEnvironmentApp->ShowEnvironment(nullptr));

//Error:
	return r;
}

RESULT DreamMeetClient::OnLogout() {
	RESULT r = R_PASS;

	UserController *pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));

	// Show login form, given this is not the first launch.
	std::string strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);

	CNM(pUserController, "User controller was nullptr");

	m_pDreamLoginApp->ClearCredential(CREDENTIAL_REFRESH_TOKEN);

	CR(pUserController->RequestFormURL(strFormType));

	CR(m_pDreamShareViewApp->Hide());
	CR(m_pDreamEnvironmentApp->HideEnvironment(nullptr));

	CRM(m_pDreamUserControlArea->ShutdownAllSources(), "failed to shutdown source");

	if (m_pDreamUserApp->GetBrowserManager() != nullptr) {
		CRM(m_pDreamUserApp->GetBrowserManager()->DeleteCookies(), "deleting cookies failed");
	}

	m_fSeated = false;

Error:
	return r;
}

RESULT DreamMeetClient::OnPendLogout() {
	RESULT r = R_PASS;

	m_fPendLogout = true;

	CRM(Exit(r), "Failed to exit Dream");

Error:
	return r;
}

RESULT DreamMeetClient::OnSwitchTeams() {
	RESULT r = R_PASS;

	m_fSeated = false;

	auto fnOnFadeOutCallback = [&](void *pContext) {

		//UserController *pUserController = dynamic_cast<UserController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::USER));
		//CNM(pUserController, "User controller was nullptr");

		// questionable what kind of resets the flags need

		//CRM(m_pDreamUserControlArea->ShutdownAllSources(), "failed to shutdown source");

		//CRM(pUserController->SwitchTeam(), "switch team failed");
		CR(PendSwitchTeams());

		CR(m_pDreamShareViewApp->Hide());

	Error:
		return r;
	};

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->FadeOut(fnOnFadeOutCallback));

	CR(m_pDreamShareViewApp->HidePointers());
	CR(m_pDreamShareViewApp->StopReceiving());

	CRM(m_pDreamUserControlArea->ShutdownAllSources(), "failed to shutdown source");


Error:
	return r;
}

RESULT DreamMeetClient::PendSwitchTeams() {
	RESULT r = R_PASS;

	EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CBRM(pEnvironmentController->IsEnvironmentSocketConnected(), R_SKIPPED, "Environment socket is not connected.");
	CR(pEnvironmentController->DisconnectFromEnvironmentSocket());

	m_fPendSwitchTeams = true;

Error:
	return r;
}

RESULT DreamMeetClient::OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) {
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
	else if (type == FormType::SIGN_UP_WELCOME || type == FormType::SIGN_IN || type == FormType::SIGN_UP || type == FormType::TEAMS_MISSING) {
	//	m_pDreamLoginApp->GetComposite()->SetVisible(true, false);
		CR(m_pDreamLoginApp->UpdateWithNewForm(strURL));
		//CR(m_pDreamLoginApp->Show());

		// Login app doesn't show at the start, but it needs to receive the controller events in the lobby
		m_pDreamLoginApp->SetAsActive();
	}
	else if (type == FormType::CERTIFICATE_ERROR) {
		m_pDreamUserControlArea->SetCertificateErrorURL(strURL);
	}
	else if (type == FormType::LOAD_RESOURCE_ERROR) {
		m_pDreamUserControlArea->SetLoadErrorURL(strURL);
	}
	// TODO: general form?

Error:
	return r;
}

RESULT DreamMeetClient::OnAccessToken(bool fSuccess, std::string& strAccessToken) {
	RESULT r = R_PASS;

	if (!fSuccess) {

		std::string strFormType = DreamFormApp::StringFromType(FormType::SIGN_IN);

		m_pDreamLoginApp->ClearTokens();

		CR(m_pDreamUserApp->SetStartupMessageType(DreamUserApp::StartupMessage::INVALID_REFRESH_TOKEN));
		CR(m_pDreamUserApp->ShowMessageQuad());

		CR(m_pUserController->RequestFormURL(strFormType));

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
		CR(m_pUserController->RequestTeam(m_strAccessToken));
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	CN(m_pDreamUserControlArea);

	if (pEnvironmentShare->GetShareType() == SHARE_TYPE_SCREEN) {
		CN(m_pDreamShareViewApp);

		CR(m_pDreamShareViewApp->ShowCastingTexture());
		CR(m_pDreamShareViewApp->BeginStream());
		CR(m_pDreamShareViewApp->Show());

		CR(m_pDreamUserControlArea->StartSharing(pEnvironmentShare));
	}
	else if (pEnvironmentShare->GetShareType() == SHARE_TYPE_CAMERA) {
		CN(m_pDreamUserControlArea->GetVCam());
		CR(m_pDreamUserControlArea->GetVCam()->StartSharing(pEnvironmentShare));
	}

Error:
	return r;
}

// TODO: Make waterquad and light positionings programmatic?
RESULT DreamMeetClient::OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) {
	RESULT r = R_PASS;

	if (!fSuccess) {
		// need to create a team, since the user has no teams
		std::string strFormType = DreamFormApp::StringFromType(FormType::TEAMS_MISSING);
		CR(m_pUserController->RequestFormURL(strFormType));
	}
	else {
		CR(m_pDreamLoginApp->HandleDreamFormSetEnvironmentId(environmentId));
		CR(m_pDreamEnvironmentApp->SetCurrentEnvironment(environment::type(environmentModelId)));
		if (environment::type(environmentModelId) == environment::type::CAVE) {
			m_pWaterQuad->SetPosition(point(90.0f, -1.28f, 0.0f));
			
			vector vWaterLightDirection = vector(-1.0f, -0.35f, 0.1f);
			float lightIntensity = 2.0f;
			auto pLight = MakeLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(vWaterLightDirection));
			CN(pLight);
			
			// This is breaking encapsulation - to be fixed with rest of garage 
			for (auto pWaterProgram : m_waterProgramNodes) {
				auto pOGLWaterProgram = dynamic_cast<OGLProgramWater*>(pWaterProgram);
				pOGLWaterProgram->SetWaterReflectionLight(pLight);
			}
		}
		else if (environment::type(environmentModelId) == environment::type::CANYON) {
			m_pWaterQuad->SetPosition(point(90.0f, -18.32f, 0.0f));
			
			vector vWaterLightDirection = vector(0.4f, -0.35f, -1.0f);
			float lightIntensity = 2.0f;
			auto pLight = MakeLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(vWaterLightDirection));
			CN(pLight);
			
			// This is breaking encapsulation - to be fixed with rest of garage 
			for (auto pWaterProgram : m_waterProgramNodes) {
				auto pOGLWaterProgram = dynamic_cast<OGLProgramWater*>(pWaterProgram);
				pOGLWaterProgram->SetWaterReflectionLight(pLight);
			}
		}
		else if (environment::type(environmentModelId) == environment::type::HOUSE) {
			m_pWaterQuad->SetPosition(point(90.0f, -2.6f, 0.0f));

			// surprisingly close enough, though the sun position is just barely different from cave
			vector vWaterLightDirection = vector(-1.0f, -0.35f, 0.1f);
			float lightIntensity = 2.0f;
			auto pLight = MakeLight(LIGHT_DIRECTIONAL, lightIntensity, point(0.0f, 10.0f, 2.0f), color(COLOR_WHITE), color(COLOR_WHITE), (vector)(vWaterLightDirection));
			CN(pLight);

			// This is breaking encapsulation - to be fixed with rest of garage 
			for (auto pWaterProgram : m_waterProgramNodes) {
				auto pOGLWaterProgram = dynamic_cast<OGLProgramWater*>(pWaterProgram);
				pOGLWaterProgram->SetWaterReflectionLight(pLight);
			}
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (m_pDreamShareViewApp != nullptr && pEnvironmentShare->GetShareType() == SHARE_TYPE_SCREEN) {

		m_pDreamShareViewApp->PendReceiving();

		m_pDreamUserControlArea->OnReceiveAsset();

		// if not connected yet, save the userID and start receiving during
		// OnNewPeerConnection; otherwise this user should receive the dream message
		// to start receiving
		if (FindPeer(pEnvironmentShare->GetUserID()) == nullptr) {
			//m_pendingAssetReceiveUserID = pEnvironmentShare->GetUserID();
			m_pPendingEnvironmentShares.emplace_back(pEnvironmentShare);
		}

		//m_pDreamBrowser->StartReceiving();
	}
	else if (pEnvironmentShare->GetShareType() == SHARE_TYPE_CAMERA) {
		auto pPeer = FindPeer(pEnvironmentShare->GetUserID());

		if (pPeer == nullptr) {
			m_pPendingEnvironmentShares.emplace_back(pEnvironmentShare);
		}
		else {
			m_pDreamUserControlArea->GetVCam()->StartReceiving(pPeer->GetPeerConnection(), pEnvironmentShare);
		}
	}


	return r;
}

RESULT DreamMeetClient::OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;
	
	CN(pEnvironmentShare);

	if (pEnvironmentShare->GetShareType() == SHARE_TYPE_SCREEN) {
		CR(m_pDreamShareViewApp->StopSending());
		CR(m_pDreamUserControlArea->HandleStopSending());
	}
	else if (pEnvironmentShare->GetShareType() == SHARE_TYPE_CAMERA) {
		CR(m_pDreamUserControlArea->OnVirtualCameraReleased());
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (pEnvironmentShare->GetShareType() == SHARE_TYPE_SCREEN) {
		CR(m_pDreamShareViewApp->StopReceiving());
	}
	else if (pEnvironmentShare->GetShareType() == SHARE_TYPE_CAMERA) {
		CR(m_pDreamUserControlArea->GetVCam()->StopReceiving());
	}

	m_pendingAssetReceiveUserID = -1;

	for (auto pPendingShare : m_pPendingEnvironmentShares) {
		if (pPendingShare == pEnvironmentShare) {
			pPendingShare = nullptr;
		}
	}

	{
		auto deleteShare = std::find(m_pPendingEnvironmentShares.begin(), m_pPendingEnvironmentShares.end(), pEnvironmentShare);
		if (deleteShare != m_pPendingEnvironmentShares.end()) {
			m_pPendingEnvironmentShares.erase(deleteShare);
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnGetByShareType(std::shared_ptr<EnvironmentShare> pEnvironmentShare) {
	RESULT r = R_PASS;

	if (pEnvironmentShare == nullptr) {
		CNR(m_pDreamUserControlArea, R_SKIPPED);
		m_pDreamUserControlArea->UpdateIsActive(false);
		CR(m_pDreamUserControlArea->SetVirtualCameraSource(DreamVCamApp::SourceType::CAMERA));
	}

Error:
	return r;
}

RESULT DreamMeetClient::OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) {
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

		// forms that show at the beginning of being in the environment wait until 
		// the environment is faded in to show
		//if (strKey != DreamFormApp::StringFromType(FormType::ENVIRONMENTS_WELCOME) ||
		//	(GetCloudController() != nullptr && !GetCloudController()->IsUserLoggedIn())) {
			CR(m_pDreamGeneralForm->Show());
		//}

		// Used for special case with disabling button presses on welcome form
		CR(m_pDreamGeneralForm->SetFormType(DreamFormApp::TypeFromString(strKey)));

	}

Error:
	return r;
}

RESULT DreamMeetClient::OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) {
	RESULT r = R_PASS;

	long assetID = -1;
	auto pEnvironmentControllerProxy = (EnvironmentControllerProxy*)(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
	CN(pEnvironmentControllerProxy);

	CN(m_pDreamUserControlArea);
	//CN(m_pDreamUserControlArea->GetActiveSource());

	// if the user does not have settings, use the defaults for the current environment
	if (!fIsSet) {
		CR(m_pDreamEnvironmentApp->GetDefaultCameraPlacement(ptPosition, qOrientation));
	}

	CR(m_pDreamUserControlArea->OnVirtualCameraCaptured());
	CR(m_pDreamUserControlArea->OnVirtualCameraSettings(ptPosition, qOrientation));

	CR(pEnvironmentControllerProxy->RequestShareCamera(m_pDreamUserControlArea->GetVCam()->GetCurrentAssetID()));

Error:
	return r;
}

RESULT DreamMeetClient::Exit(RESULT r) {

	auto fnOnFadeOutCallback = [&](void *pContext) {

		EnvironmentController *pEnvironmentController = dynamic_cast<EnvironmentController*>(GetCloudController()->GetControllerProxy(CLOUD_CONTROLLER_TYPE::ENVIRONMENT));
		CBRM(pEnvironmentController->IsEnvironmentSocketConnected(), R_SKIPPED, "Environment socket is not connected.");
		CR(pEnvironmentController->DisconnectFromEnvironmentSocket());
		//CRM(DreamOS::Exit(r), "Exit dream failed");
		m_fPendExit = true;

	Error:
		return r;
	};

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->FadeOut(fnOnFadeOutCallback));

Error:
	return r;
}

RESULT DreamMeetClient::Notify(SenseKeyboardEvent *kbEvent)  {
	RESULT r = R_PASS;

	if (GetSandboxConfiguration().f3rdPersonCamera) {
		if (kbEvent->KeyCode == 65 && kbEvent->KeyState == 0) {
			m_pDreamUserApp->ToggleUserModel();
		}
	}

Error:
	return r;
}

RESULT DreamMeetClient::Notify(SenseTypingEvent *kbEvent) {
	RESULT r = R_PASS;

Error:
	return r;
}

texture* DreamMeetClient::GetSharedCameraTexture() {
	RESULT r = R_PASS;

	CN(m_pDreamUserControlArea);
	CN(m_pDreamUserControlArea->GetVCam());

	return m_pDreamUserControlArea->GetVCam()->GetCameraQuadTexture();

Error:
	return nullptr;
}

RESULT DreamMeetClient::GetDefaultVCamPlacement(point& ptPosition, quaternion& qOrientation) {
	RESULT r = R_PASS;

	CN(m_pDreamEnvironmentApp);
	CR(m_pDreamEnvironmentApp->GetDefaultCameraPlacement(ptPosition, qOrientation));

Error:
	return r;
}

bool DreamMeetClient::IsCameraInUse() {
	RESULT r = R_PASS;
	
	CNR(m_pDreamUserControlArea, R_SKIPPED);
	CNR(m_pDreamUserControlArea->GetVCam(), R_SKIPPED);

	return m_pDreamUserControlArea->GetVCam()->IsReceivingCameraPlacement();
Error:
	return false;
}
