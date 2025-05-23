#include "Sandbox.h"
\
#include <synchapi.h>                           // for Sleep
#include <WinUser.h>                            // for GetAsyncKeyState, VK_ESCAPE

#include "CommandLineManager.h"

#include "cloud/CloudController.h"

#include "core/primitives/ray.h"
#include "core/primitives/PrimParams.h"

#include "core/model/ModelFactory.h"

#include "core/hysteresis/HysteresisPlane.h"
#include "core/hysteresis/HysteresisSphere.h"
#include "core/hysteresis/HysteresisCylinder.h"

#include "scene/CameraNode.h"
#include "scene/ObjectStoreNode.h"

#include "pipeline/ProgramNode.h"
#include "pipeline/SinkNode.h"

// TODO: This needs to be resolved in the build system 
// or the configuration 
#ifndef OCULUS_PRODUCTION_BUILD
	#include <hmd/openvr/OpenVRDevice.h>
#endif

#include "os/app/DreamAppMessage.h"
#include "os/app/DreamAppManager.h"

#include "os/module/DreamModuleManager.h"

#include "modules/InteractionEngine/InteractionEngine.h"
#include "modules/PhysicsEngine/PhysicsEngine.h"
#include "modules/PhysicsEngine/CollisionDetector.h"

#include "sense/SenseLeapMotion.h"

// TODO: Fix, no OGL allowed at this level 
#include "hal/ogl/OGLHand.h"

// TODO:
#include "logger/DreamLogger.h"            // for DreamLogger

#include "hal/ogl/OGLRenderingContext.h"  // for OpenGLRenderingContext

#include "pipeline/Pipeline.h"              // for Pipeline

#include "core/primitives/billboard.h"               // for billboard
#include "core/primitives/composite.h"               // for composite
#include "core/primitives/cylinder.h"                // for cylinder
#include "core/primitives/FlatContext.h"             // for FlatContext
#include "core/primitives/quad.h"                    // for quad
#include "core/primitives/skybox.h"                  // for skybox
#include "core/primitives/sphere.h"                  // for sphere
#include "core/primitives/volume.h"                  // for volume

#include "core/dimension/DimObj.h"                  // for DimObj
#include "core/dimension/DimPlane.h"                // for DimPlane
#include "core/dimension/DimRay.h"                  // for DimRay

#include "core/model/model.h"             // for model
#include "core/model/mesh.h"              // for mesh

#include "core/user/user.h"                    // for user

#include "core/camera/stereocamera.h"            // for stereocamera

#include "scene/ObjectStore.h"                  // for ObjectStore
#include "scene/ObjectStoreFactory.h"           // for ObjectStoreFactory, ObjectStoreFactory::TYPE, ObjectStoreFactory::TYPE::LIST

class SinkNode;

Sandbox::Sandbox() :
	m_pPathManager(nullptr),
	m_pCommandLineManager(nullptr),
	m_pOGLRenderingContext(nullptr),
	m_pSceneGraph(nullptr),
	m_pAuxSceneGraph(nullptr),
	m_pUISceneGraph(nullptr),
	m_pUIClippingSceneGraph(nullptr),
	m_pAuxUISceneGraph(nullptr),
	m_pPhysicsGraph(nullptr),
	m_pInteractionGraph(nullptr),
	m_pFlatSceneGraph(nullptr),
	m_pCloudController(nullptr),
	m_pHALImp(nullptr),
	m_pHMD(nullptr),
	m_fnUpdateCallback(nullptr),
	m_pSenseLeapMotion(nullptr),
	m_pPhysicsEngine(nullptr),
	m_pInteractionEngine(nullptr)
{
	// empty
}

Sandbox::~Sandbox() {
	if (m_pCloudController != nullptr) {
		delete m_pCloudController;
		m_pCloudController = nullptr;
	}

	if (m_pHALImp != nullptr) {
		delete m_pHALImp;
		m_pHALImp = nullptr;
	}

	if (m_pOGLRenderingContext != nullptr) {
		delete m_pOGLRenderingContext;
		m_pOGLRenderingContext = nullptr;
	}
}

RESULT Sandbox::SetSandboxConfiguration(Sandbox::configuration sandboxconf) {
	m_SandboxConfiguration = sandboxconf;
	return R_PASS;
}

InteractionEngineProxy *Sandbox::GetInteractionEngineProxy() {
	return m_pInteractionEngine->GetInteractionEngineProxy();
}

const Sandbox::configuration& Sandbox::GetSandboxConfiguration() {
	return m_SandboxConfiguration;
}

RESULT Sandbox::SetMouseIntersectObjects(bool fMouseIntersectObjects) {
	m_fMouseIntersectObjects = fMouseIntersectObjects;
	return R_PASS;
}

bool Sandbox::IsMouseIntersectObjects() {
	return m_fMouseIntersectObjects;
}

RESULT Sandbox::Notify(SenseKeyboardEvent *kbEvent) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT Sandbox::Notify(SenseTypingEvent *kbEvent) {
	return R_NOT_IMPLEMENTED_WARNING;
}

RESULT Sandbox::Notify(SenseMouseEvent *mEvent) {
	RESULT r = R_PASS;

	switch (mEvent->EventType) {
		case SENSE_MOUSE_MOVE: {
			// For object intersection testing
			//SenseMouse::PrintEvent(mEvent);

			if (m_fMouseIntersectObjects) {
				// Create ray
				// TODO: This will only work for non-HMD camera

				ray rayCamera = m_pCamera->GetRay(mEvent->xPos, mEvent->yPos);

				// intersect ray
				auto intersectedObjects = m_pSceneGraph->GetObjects(rayCamera);

				// TODO: Junk code:
				for (auto &pObject : m_pSceneGraph->GetObjects()) {

					DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
					if (pDimObj != nullptr) {
						pDimObj->SetVertexColor(color(COLOR_WHITE));
					}
				}

				for (auto &pObject : intersectedObjects) {
					DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
					pDimObj->SetVertexColor(color(COLOR_RED));
				}
			}
		} break;

		case SENSE_MOUSE_LEFT_DRAG_MOVE: {
			if (m_SandboxConfiguration.fMouseLook) {
				m_pSenseMouse->CaptureMouse();
				//m_pSenseMouse->CenterMousePosition();
				m_pCamera->RotateCameraByDiffXY(mEvent->dx, mEvent->dy);
			}
		} break;

		case SENSE_MOUSE_LEFT_BUTTON_UP: {
			if (m_SandboxConfiguration.fMouseLook) {
				m_pSenseMouse->ReleaseMouse();
			}
		} break;
	}

//Error:
	return r;
}

RESULT Sandbox::GetMouseRay(ray &rCast, double t){
	RESULT r = R_PASS;
	int mouseX = 0;
	int mouseY = 0;
	int pxWidth = 0;
	int pxHeight = 0;

	// Get mouse position
	CR(m_pSenseMouse->GetMousePosition(mouseX, mouseY));
	CR(GetSandboxWindowSize(pxWidth, pxHeight));

	if (mouseX >= 0 && mouseY >= 0 && mouseX <= pxWidth && mouseY <= pxHeight) {

		CN(m_pCamera);
		rCast = m_pCamera->GetRay(mouseX, mouseY, t);

		//DEBUG_LINEOUT("mouse: (%d, %d)", mouseX, mouseY);
		//rCast.Print();
	}

Error:
	return r;
}

RESULT Sandbox::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;



//Error:
	return r;
}

RESULT Sandbox::Notify(CollisionGroupEvent* gEvent) {
	RESULT r = R_PASS;

	for (auto &pObject : gEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);

		if (pDimObj != nullptr) {
			pDimObj->SetVertexColor(color(COLOR_WHITE));
		}
	}

	for (auto &pObject : gEvent->m_collisionGroup) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);

		if (pDimObj != nullptr) {
			pDimObj->SetVertexColor(color(COLOR_RED));
		}
	}


//Error:
	return r;
}

RESULT Sandbox::RegisterImpKeyboardEvents() {
	RESULT r = R_PASS;

	CR(RegisterSubscriber(SVK_TAB, this));

	//camera *pCamera = m_pHALImp->GetCamera();

	//CR(CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::Camera, pCamera));

	//CR(RegisterSubscriber(TIME_ELAPSED, pCamera));

	//CR(RegisterSubscriber(SVK_LEFT, pCamera));
	//CR(RegisterSubscriber(SVK_UP, pCamera));
	//CR(RegisterSubscriber(SVK_DOWN, pCamera));
	//CR(RegisterSubscriber(SVK_RIGHT, pCamera));

	//CR(RegisterSubscriber(SVK_SPACE, pCamera));

	/*
	for (int i = 0; i < 26; i++) {
		CR(RegisterSubscriber((SenseVirtualKey)('A' + i), pCamera));
	}
	*/

Error:
	return r;
}

RESULT Sandbox::RegisterImpMouseEvents() {
	RESULT r = R_PASS;

	CR(RegisterSubscriber(SENSE_MOUSE_MOVE, this));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_DRAG_MOVE, this));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_UP, this));

	//camera *pCamera = m_pOGLImp->GetCamera();

	// TODO: Should either be moved up to the sandbox or into the Imp itself
	//CR(RegisterSubscriber(SENSE_MOUSE_MOVE, m_pHALImp));
	/*
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_DRAG_MOVE, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_RIGHT_DRAG_MOVE, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_UP, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_DOWN, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON_DOWN, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON_UP, m_pHALImp));
	*/


Error:
	return r;
}

// TODO: shouldn't be this way ultimately
RESULT Sandbox::RegisterImpLeapMotionEvents() {
	RESULT r = R_PASS;

	hand *pLeftHand = new OGLHand(reinterpret_cast<OGLImp*>(m_pHALImp), HAND_TYPE::HAND_LEFT);
	hand *pRightHand = new OGLHand(reinterpret_cast<OGLImp*>(m_pHALImp), HAND_TYPE::HAND_RIGHT);

	std::shared_ptr<DimObj> pLeftHandSharedPtr(pLeftHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pLeftHandSharedPtr);

	std::shared_ptr<DimObj> pRightHandSharedPtr(pRightHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pRightHandSharedPtr);

	// TODO: broken for now
//	CR(m_pSenseLeapMotion->AttachHand(pLeftHand, HAND_TYPE::HAND_LEFT));
//	CR(m_pSenseLeapMotion->AttachHand(pRightHand, HAND_TYPE::HAND_RIGHT));

//Error:
	return r;
}

RESULT Sandbox::RegisterImpControllerEvents() {
	RESULT r = R_PASS;

	if (m_pHMD != nullptr) {
		hand *pLeftHand = new OGLHand(reinterpret_cast<OGLImp*>(m_pHALImp), HAND_TYPE::HAND_LEFT);
		hand *pRightHand = new OGLHand(reinterpret_cast<OGLImp*>(m_pHALImp), HAND_TYPE::HAND_RIGHT);

		CR(m_pHMD->AttachHand(pLeftHand, HAND_TYPE::HAND_LEFT));
		CR(m_pHMD->AttachHand(pRightHand, HAND_TYPE::HAND_RIGHT));

/*
		if (dynamic_cast<OVRHMD*>(m_pHMD) != nullptr) {
			AddObject(pLeftHand);
			AddObject(pRightHand);
		}
		//*/
	}
Error:
	return r;
}

hand *Sandbox::GetHand(HAND_TYPE handType) {

	if (m_pHMD != nullptr) {
		return m_pHMD->GetHand(handType);
	}

	if (m_pSenseLeapMotion != nullptr) {
		return m_pSenseLeapMotion->GetHand(handType);
	}

	return nullptr;
}


bool Sandbox::IsSandboxRunning() {
	return m_fRunning;
}

RESULT Sandbox::SetSandboxRunning(bool fRunning) {
	m_fRunning = fRunning;
	return R_SUCCESS;
}

inline PathManager* Sandbox::GetPathManager() {
	return m_pPathManager;
}

inline OGLRenderingContext *Sandbox::GetOGLRenderingContext() {
	return m_pOGLRenderingContext;
}

bool Sandbox::IsShuttingDown() {
	return m_fPendingShutdown;
}

RESULT Sandbox::PendShutdown() {
	m_fPendingShutdown = true;
	return R_PASS;
}

RESULT Sandbox::Shutdown() {
	RESULT r = R_SUCCESS;

	DOSLOG(INFO, "Begin sandbox shutdown.");

	if (m_pDreamAppManager != nullptr) {
		CR(m_pDreamAppManager->Shutdown());
		m_pDreamAppManager = nullptr;
	}
	DOSLOG(INFO, "App Manager Shutdown");

	if (m_pDreamModuleManager != nullptr) {
		CR(m_pDreamModuleManager->Shutdown());
		m_pDreamModuleManager = nullptr;
	}
	DOSLOG(INFO, "Module Manager Shutdown");

	if (m_pHMD != nullptr) {
		CR(m_pHMD->ReleaseHMD());
		delete m_pHMD;
		m_pHMD = nullptr;
	}
	DOSLOG(INFO, "Release HMD");

	// Implementation specific shutdown
	CR(ShutdownSandbox());
	DOSLOG(INFO, "Sandbox Shutdown complete");

Error:
	return r;
}

RESULT Sandbox::HMDShutdown() {
	RESULT r = R_PASS;

	//*
	DOSLOG(INFO, "OVRShutdown called");
	if (m_pHMD != nullptr) {
		DOSLOG(INFO, "Releasing HMD");
		CR(m_pHMD->ReleaseHMD());
		delete m_pHMD;
		m_pHMD = nullptr;
		DOSLOG(INFO, "HMD Released");
	}
	//*/

	PendShutdown();

Error:
	return r;
}

RESULT Sandbox::RunAppLoop() {
	RESULT r = R_PASS;

	// Launch main message loop
	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

#ifdef OCULUS_PRODUCTION_BUILD
	// TODO: This is a hack until async model loading can happen
	// Literally only doing it to pass oculus tests
	m_pHALImp->Render();
	SwapDisplayBuffers();
#endif

	SetSandboxRunning(true);

	// TODO: This should be moved to the sandbox
	while (IsSandboxRunning()) {
		CR(HandleMessages());	// Handle windows messages

		if (!IsSandboxRunning())
			break;

#ifdef CEF_ENABLED
		// Update Network
		CR(m_pCloudController->Update());
#endif

		// Module Manager
		CR(m_pDreamModuleManager->Update());

		// App Manager
		CR(m_pDreamAppManager->Update());

		// Update Callback
		if (m_fnUpdateCallback != nullptr) {
			r = m_fnUpdateCallback();
			CR(r);

			if(r == R_COMPLETE)
				return R_COMPLETE;
		}

		// Update the mouse
		// TODO: This is wrong architecture, this should
		// be parallel
		// TODO: Update Sense etc
		//m_pWin64Mouse->UpdateMousePosition();

		if (m_pHMD != nullptr) {
			CRM(m_pHMD->UpdateHMD(), "UpdateHMD failed in Sandbox");
		}

		// Update Scene
		//CR(m_pSceneGraph->UpdateScene());

		// TODO: Do these need to be wired up this way?
		// Why not just do an Update with retained graph

		// Do this before we set the program or any GPU specific things
		// since this gives objects the opportunity to do stuff on the GPU side
		// TODO: why
		OGLProgram::UpdateObjectStore(m_pUIClippingSceneGraph);
		OGLProgram::UpdateObjectStore(m_pUISceneGraph);
		OGLProgram::UpdateObjectStore(m_pAuxUISceneGraph);

		// TODO: MODULE
		// Update Physics
		CR(m_pPhysicsEngine->UpdateObjectStore(m_pPhysicsGraph));

		// Update Interaction Engine
		CR(m_pInteractionEngine->UpdateObjectStore(m_pInteractionGraph));
		CR(m_pInteractionEngine->UpdateAnimationQueue());

		// Update HMD
		if (m_pHMD != nullptr) {
			m_pHALImp->SetCameraOrientation(m_pHMD->GetHMDOrientation());
			m_pHALImp->SetCameraPositionDeviation(m_pHMD->GetHMDTrackerDeviation());
		}

		//m_pOGLImp->RenderStereo(m_pSceneGraph);
		//m_pOGLImp->Render(m_pSceneGraph);
		m_pHALImp->Render();

		// Swap buffers
		SwapDisplayBuffers();

		//DreamConsole::GetConsole()->OnFrameRendered();
#ifdef _DEBUG
		if (IsShuttingDown() || GetAsyncKeyState(VK_ESCAPE)) {
#else
		if (IsShuttingDown()) {
#endif
			Sleep(1000);
			Shutdown();
		}
	}

Error:
	DreamLogger::instance()->Flush();
	return r;
}

RESULT Sandbox::ResizeViewport(viewport newViewport) {
	RESULT r = R_PASS;

	// Resize the camera
	CR(m_pCamera->ResizeCamera(newViewport));

	// OpenGL Resize the view after the window had been resized
	CRM(m_pHALImp->Resize(m_viewport), "Failed to resize OpenGL Implemenation");

Error:
	return r;
}

RESULT Sandbox::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	// Set up command line manager
	m_pCommandLineManager = CommandLineManager::instance();
	CN(m_pCommandLineManager);

	CR(m_pCommandLineManager->RegisterParameter("hmd", "h", ""));
	CR(m_pCommandLineManager->RegisterParameter("environment", "env", "default"));
	
	CR(m_pCommandLineManager->RegisterParameter("leap", "lp", ""));
	CR(m_pCommandLineManager->RegisterParameter("namedpipe", "n", ""));

	//CR(m_pCommandLineManager->RegisterParameter("otk.id", "otk.id", "INVALIDONETIMEKEY"));
	//CR(m_pCommandLineManager->RegisterParameter("username", "u", "defaulttestuser@dreamos.com"));
	//CR(m_pCommandLineManager->RegisterParameter("password", "p", "Nightmare479!"));

	// This can attempt to connect to a given environment

#ifndef PRODUCTION_BUILD
	CR(m_pCommandLineManager->RegisterParameter("environment.path", "env.path", "default"));
	CR(m_pCommandLineManager->RegisterParameter("head.path", "head.path", "default"));
	CR(m_pCommandLineManager->RegisterParameter("lefthand.path", "lhand.path", "default"));
	CR(m_pCommandLineManager->RegisterParameter("righthand.path", "rhand.path", "default"));

	CR(m_pCommandLineManager->RegisterParameter("rtoken", "rt", ""));
#endif

	// For auto login, use '-l auto'
#if defined(_USE_TEST_APP) || defined(_UNIT_TESTING)
	CR(m_pCommandLineManager->RegisterParameter("login", "l", "no"));
#else
	CR(m_pCommandLineManager->RegisterParameter("login", "l", "auto"));
#endif

	CR(m_pCommandLineManager->InitializeFromCommandLine(argc, argv));

	// Set up Scene Graph
	//m_pSceneGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	m_pSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");

	m_pAuxSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pAuxSceneGraph, "Failed to allocate Scene Graph");

	m_pUISceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pUISceneGraph, "Failed to allocate UI Scene Graph");

	m_pAuxUISceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pAuxUISceneGraph, "Failed to allocate Aux UI Scene Graph");

	m_pUIClippingSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pUIClippingSceneGraph, "Failed to allocate UI Clipping Scene Graph");

	m_pBillboardSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pBillboardSceneGraph, "Failed to allocate UI Clipping Scene Graph");

	// This will prevent scene graph from being deleted when not connected
	// TODO: Attach to Sandbox somehow?
	CB(m_pSceneGraph->incRefCount());
	CB(m_pAuxSceneGraph->incRefCount());
	CB(m_pUISceneGraph->incRefCount());
	CB(m_pUIClippingSceneGraph->incRefCount());
	CB(m_pAuxUISceneGraph->incRefCount());

	// Set up flat graph
	m_pFlatSceneGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pFlatSceneGraph, "Failed to allocate Scene Graph");

	CRM(InitializeCamera(), "Failed to initialize Camera");
	CRM(InitializeHAL(), "Failed to initialize HAL");

	// TODO: Remove CMD line arg and use global config
	if ((m_pCommandLineManager->GetParameterValue("hmd").compare("") == 0) == false) {
		m_SandboxConfiguration.fUseHMD = false;
	}
	CRM(InitializeHMD(), "Failed to initialize HMD");
	DOSLOG(INFO, "HMD Initialized");

	// Set up the pipeline
	CR(SetUpHALPipeline(m_pHALImp->GetRenderPipelineHandle()));
	DOSLOG(INFO, "HAL Pipeline Setup Successful");

	// Generalize this module pattern
	if (m_SandboxConfiguration.fInitCloud) {
		CRM(InitializeCloudController(), "Failed to initialize cloud controller");
	}

	CRM(InitializeDreamModuleManager(), "Failed to initialize Dream Module Manager");

	// TODO: Time manager should be converted to a module
	CRM(InitializeTimeManagerModule(), "Failed to initialize time manager");

	if ((m_pCommandLineManager->GetParameterValue("namedpipe").compare("") == 0) == false) {
		m_SandboxConfiguration.fInitNamedPipe = false;
	}
	CRM(InitializeDreamAppManager(), "Failed to initialize app manager");
	DOSLOG(INFO, "Finished Initializing DreamAppManager");

	if ((m_pCommandLineManager->GetParameterValue("leap").compare("") == 0) == false) {
		m_SandboxConfiguration.fUseLeap = false;
	}

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");
	DOSLOG(INFO, "Finished Initializing Sandbox");

	// TODO: Make these into modules
	// TODO: These have dependencies potentially on previous modules
	// TODO: Need to create proper module loading / dependency system
	CRM(InitializePhysicsEngine(), "Failed to initialize physics engine");

	CRM(InitializeInteractionEngine(), "Failed to initialize interaction engine");


	// Auto Login Handling
	// This is done in DreamOS now
	//if (m_pCommandLineManager->GetParameterValue("login").compare("auto") == 0) {
	//	// auto login
	//	m_pCloudController->Start();
	//}

Error:
	return r;
}

// TODO: Module
RESULT Sandbox::InitializePhysicsEngine() {
	RESULT r = R_PASS;

	m_pPhysicsEngine = PhysicsEngine::MakePhysicsEngine();
	WCNM(m_pPhysicsEngine, "Physics Engine failed to initialize");

	// Set up physics graph
	m_pPhysicsGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pPhysicsGraph, "Failed to allocate Physics Graph");

	CRM(m_pPhysicsEngine->SetPhysicsGraph(m_pPhysicsGraph), "Failed to set physics object store");

	// Register OBJECT_GROUP_COLLISION
	// CR(m_pPhysicsEngine->RegisterSubscriber(OBJECT_GROUP_COLLISION, this));

Error:
	return r;
}

// TODO: Module
RESULT Sandbox::InitializeInteractionEngine() {
	RESULT r = R_PASS;

	m_pInteractionEngine = InteractionEngine::MakeEngine(this);
	WCNM(m_pInteractionEngine, "Interaction Engine failed to initialize");

	// Set up interaction graph
	m_pInteractionGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pInteractionGraph, "Failed to allocate interaction Graph");

	CRM(m_pInteractionEngine->SetInteractionGraph(m_pInteractionGraph), "Failed to set interaction object store");

	if (m_pHMD != nullptr) {
		SenseController *pSenseController = m_pHMD->GetSenseController();
		if (pSenseController != nullptr) {
			m_pInteractionEngine->RegisterSenseController(pSenseController);
		}
	}

Error:
	return r;
}

RESULT Sandbox::InitializeTimeManagerModule() {
	RESULT r = R_PASS;

	CNM(m_pDreamModuleManager, "Module manager is not running");

	DOSLOG(INFO, "Launching Time Manager Module");

	m_pTimeManagerModule = m_pDreamModuleManager->CreateRegisterAndStartModule<TimeManagerModule>(this);	
	CNM(m_pTimeManagerModule, "Failed to launch Time Manager Module");
	CVM(m_pTimeManagerModule, "Failed to validate Time Manager Module");

Error:
	return r;
}

RESULT Sandbox::InitializeDreamModuleManager() {
	RESULT r = R_PASS;

	// Initialize Time Manager
	m_pDreamModuleManager = std::make_unique<DreamModuleManager>(GetDreamOSHandle());

	CNM(m_pDreamModuleManager, "Failed to allocate Dream Module Manager");
	CVM(m_pDreamModuleManager, "Failed to validate Dream Module Manager");

Error:
	return r;
}

RESULT Sandbox::InitializeDreamAppManager() {
	RESULT r = R_PASS;

	// Initialize Time Manager
	m_pDreamAppManager = std::make_unique<DreamAppManager>(GetDreamOSHandle());

	CNM(m_pDreamAppManager, "Failed to allocate Dream App Manager");
	CVM(m_pDreamAppManager, "Failed to validate Dream App Manager");

Error:
	return r;
}

RESULT Sandbox::InitializeCamera() {
	RESULT r = R_PASS;

	//m_pCamera = std::make_shared<stereocamera>(point(0.0f, 0.0f, 5.0f), m_viewport);

	m_pCamera = DNode::MakeNode<CameraNode>(point(0.0f, 0.0f, 5.0f), m_viewport);
	CN(m_pCamera);

	// This will prevent camera from being deleted when not connected
	CB(m_pCamera->incRefCount());

Error:
	return r;
}

// Note: This needs to be done after GL set up
RESULT Sandbox::InitializeHMD() {
	RESULT r = R_PASS;

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	int pxWidth, pxHeight;
	GetSandboxWindowSize(pxWidth, pxHeight);

	if (GetSandboxConfiguration().fUseHMD) {
		//m_pHMD = HMDFactory::MakeHMD(HMD_OVR, this, m_pHALImp, pxWidth, pxHeight);
		//m_pHMD = HMDFactory::MakeHMD(HMD_OPENVR, this, m_pHALImp, pxWidth, pxHeight);
		m_pHMD = HMDFactory::MakeHMD(
			m_SandboxConfiguration.hmdType,
			this,
			m_pHALImp,
			pxWidth,
			pxHeight,
			GetSandboxConfiguration().fHMDMirror	// disable / enable mirror
		);

		if (m_pHMD != nullptr) {
			CRM(m_pHALImp->SetHMD(m_pHMD), "Failed to initialize stereo frame buffers");
		}
	}

Error:
	return r;
}

RESULT Sandbox::InitializeHAL() {
	RESULT r = R_PASS;

	// Setup OpenGL and Resize Windows etc
	//CNM(m_hDC, "Can't start Sandbox with NULL Device Context");
	CNM(m_pCamera, "HAL depends on camera being set up");

	// Create and initialize OpenGL Imp
	// TODO: HAL factory pattern
	m_pHALImp = new OGLImp(m_pOGLRenderingContext);
	CNM(m_pHALImp, "Failed to create HAL Implementation");
	CVM(m_pHALImp, "HAL Implementation Invalid");

	CR(m_pHALImp->SetCamera(m_pCamera));

	CR(m_pHALImp->InitializeHAL());
	CR(m_pHALImp->InitializeRenderPipeline());

Error:
	return r;
}

// TODO: Move this up to DreamOS
RESULT Sandbox::SetUpHALPipeline(Pipeline* pRenderPipeline) {
	RESULT r = R_PASS;

	// TODO: Get from HMD if HMD is valid
	SinkNode* pDestSinkNode = nullptr;

	if (m_pHMD == nullptr) {
		pDestSinkNode = m_pHALImp->MakeSinkNode("display");
	}
	else {
		pDestSinkNode = (SinkNode*)(m_pHMD->GetHMDSinkNode());
	}
	CN(pDestSinkNode);

	CNM(pRenderPipeline, "Pipeline not initialized");
	CR(pRenderPipeline->SetDestinationSinkNode(pDestSinkNode));

	pDestSinkNode = pRenderPipeline->GetDestinationSinkNode();
	CNM(pDestSinkNode, "Destination sink node isn't set");

	CB(pDestSinkNode->incRefCount());

	// Add 3rd person sink node if set
	if (GetSandboxConfiguration().f3rdPersonCamera == true) {
		SinkNode *p3rdPersonSinkNode = nullptr;
		p3rdPersonSinkNode = m_pHALImp->MakeSinkNode("displaycamera");
		CNM(p3rdPersonSinkNode, "Failed to create 3rd person sink node");

		CNM(pRenderPipeline, "Pipeline not initialized");
		CR(pRenderPipeline->SetAuxiliarySinkNode(p3rdPersonSinkNode));

		p3rdPersonSinkNode = pRenderPipeline->GetAuxiliarySinkNode();
		CNM(p3rdPersonSinkNode, "Auxiliary sink node isn't set");

		CB(p3rdPersonSinkNode->incRefCount());
	}

Error:
	return r;
}

RESULT Sandbox::RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	r = m_pPhysicsEngine->RegisterObjectCollisionSubscriber(pVirtualObject, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}

RESULT Sandbox::RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	//r = m_pInteractionEngine->RegisterSubscriber(eventType, pInteractionSubscriber);
	CR(r);

Error:
	return r;
}

RESULT Sandbox::RegisterEventSubscriber(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	r = m_pInteractionEngine->RegisterSubscriber(pObject, eventType, pInteractionSubscriber);
	CR(r);

Error:
	return r;
}

RESULT Sandbox::UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pInteractionEngine->UnregisterSubscriber(pObject, eventType, pInteractionSubscriber);
}

RESULT Sandbox::UnregisterInteractionSubscriber(Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pInteractionEngine->UnregisterSubscriber(pInteractionSubscriber);
}

RESULT Sandbox::UnregisterInteractionObject(VirtualObj *pObject) {
	return m_pInteractionEngine->UnregisterSubscriber(pObject);
}

long Sandbox::GetTickCount() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

RESULT Sandbox::SetHALConfiguration(HALImp::HALConfiguration halconf) {
	return m_pHALImp->SetHALConfiguration(halconf);
}

const HALImp::HALConfiguration& Sandbox::GetHALConfiguration() {
	return m_pHALImp->GetHALConfiguration();
}



// Sandbox Factory Methods

// Sandbox Objects
std::shared_ptr<NamedPipeClient> Sandbox::MakeNamedPipeClient(std::wstring strPipename) {
	return nullptr;
}

std::shared_ptr<NamedPipeServer> Sandbox::MakeNamedPipeServer(std::wstring strPipename) {
	return nullptr;
}

RESULT Sandbox::AddObject(VirtualObj *pObject, PipelineType pipelineType) {
	RESULT r = R_PASS;

	if (static_cast<int>(pipelineType & PipelineType::MAIN) != 0) {
		CR(m_pSceneGraph->PushObject(pObject));
	}
	if (static_cast<int>(pipelineType & PipelineType::AUX) != 0) {
		CR(m_pAuxSceneGraph->PushObject(pObject));
	}

Error:
	return r;
}

// This adds the object to the physics graph (otherwise it will not get integrated / operated on)
RESULT Sandbox::AddPhysicsObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pPhysicsGraph->PushObject(pObject));

Error:
	return r;
}

// This adds the object to the interaction graph (otherwise it will not be included in event handling)
RESULT Sandbox::AddObjectToInteractionGraph(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pInteractionGraph->PushObject(pObject));

Error:
	return r;
}

RESULT Sandbox::RemoveObjectFromInteractionGraph(VirtualObj *pObject) {
	return m_pInteractionGraph->RemoveObject(pObject);
}

RESULT Sandbox::AddInteractionObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pInteractionEngine->AddInteractionObject(pObject));

Error:
	return r;
}

RESULT Sandbox::AddObjectToUIGraph(VirtualObj *pObject, PipelineType pipelineType) {
	RESULT r = R_PASS;

	if (static_cast<int>(pipelineType & PipelineType::MAIN) != 0) {
		CR(m_pUISceneGraph->PushObject(pObject));
	}
	if (static_cast<int>(pipelineType & PipelineType::AUX) != 0) {
		CR(m_pAuxUISceneGraph->PushObject(pObject));
	}

Error:
	return r;
}

RESULT Sandbox::AddObjectToUIClippingGraph(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pUIClippingSceneGraph->PushObject(pObject));

Error:
	return r;
}

RESULT Sandbox::RemoveObjectFromUIGraph(VirtualObj *pObject) {
	return m_pUISceneGraph->RemoveObject(pObject);
}

RESULT Sandbox::RemoveObjectFromUIClippingGraph(VirtualObj *pObject) {
	return m_pUIClippingSceneGraph->RemoveObject(pObject);
}

RESULT Sandbox::RemoveObjectFromAuxUIGraph(VirtualObj *pObject) {
	return m_pAuxUISceneGraph->RemoveObject(pObject);
}

/*
RESULT SandboxApp::UpdateInteractionPrimitive(const ray &rCast) {
	RESULT r = R_PASS;

	CR(m_pInteractionEngine->UpdateInteractionPrimitive(rCast));

Error:
	return r;
}
*/

RESULT Sandbox::RemoveObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	DimObj *pObj = reinterpret_cast<DimObj*>(pObject);

	if (pObj != nullptr) {
		CR(m_pInteractionEngine->RemoveAnimationObject(pObj));
	}

	CR(m_pPhysicsGraph->RemoveObject(pObject));
	CR(m_pSceneGraph->RemoveObject(pObject));
	CR(m_pAuxSceneGraph->RemoveObject(pObject));
	CR(m_pUISceneGraph->RemoveObject(pObject));
	CR(m_pUIClippingSceneGraph->RemoveObject(pObject));
	CR(m_pAuxUISceneGraph->RemoveObject(pObject));

	CR(m_pInteractionEngine->RemoveObject(pObject, m_pInteractionGraph));
	CR(m_pInteractionGraph->RemoveObject(pObject));

Error:
	//if (pObject != nullptr) {
	//	delete pObject;
	//	pObject = nullptr;
	//}

	return r;
}

// This is the nuclear option - it will flush all objects out
RESULT Sandbox::RemoveAllObjects() {
	RESULT r = R_PASS;

	// removes all animations
	CR(m_pInteractionEngine->RemoveAllObjects());

	CR(m_pPhysicsGraph->RemoveAllObjects());
	CR(m_pSceneGraph->RemoveAllObjects());
	CR(m_pAuxSceneGraph->RemoveAllObjects());
	CR(m_pUISceneGraph->RemoveAllObjects());
	CR(m_pUIClippingSceneGraph->RemoveAllObjects());
	CR(m_pAuxUISceneGraph->RemoveAllObjects());
	CR(m_pInteractionGraph->RemoveAllObjects());

Error:
	return r;
}

RESULT Sandbox::SetGravityAcceleration(double acceleration) {
	RESULT r = R_PASS;

	CR(m_pPhysicsEngine->SetGravityAccelration(acceleration));

Error:
	return r;
}

RESULT Sandbox::SetGravityState(bool fEnabled) {
	RESULT r = R_PASS;

	CR(m_pPhysicsEngine->SetGravityState(fEnabled));

Error:
	return r;
}

RESULT Sandbox::InitializeObject(DimObj *pDimObj) {
	return m_pHALImp->InitializeObject(pDimObj);
}

RESULT Sandbox::InitializeTexture(texture *pTexture) {
	return m_pHALImp->InitializeTexture(pTexture);
}

DimObj *Sandbox::MakeObject(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;
	DimObj *pDimObj = nullptr;

	if (pPrimParams->GetPrimitiveType() == PRIMITIVE_TYPE::MODEL) {
		CNM(m_pDreamOSHandle, "Async model loading not supported without a DreamOS handle in Sandbox");

		pDimObj = ModelFactory::MakeModel(m_pDreamOSHandle, pPrimParams, fInitialize);
		CN(pDimObj);
	}
	else {
		pDimObj = m_pHALImp->MakeObject(pPrimParams, fInitialize);
		CN(pDimObj);
	}

Success:
	return pDimObj;

Error:
	if (pDimObj != nullptr) {
		delete pDimObj;
		pDimObj = nullptr;
	}

	return nullptr;
}

texture *Sandbox::MakeTexture(PrimParams *pPrimParams, bool fInitialize) {
	RESULT r = R_PASS;
	texture *pTexture = nullptr;

	pTexture = m_pHALImp->MakeTexture(pPrimParams, fInitialize);
	CN(pTexture);

Success:
	return pTexture;

Error:
	if (pTexture != nullptr) {
		delete pTexture;
		pTexture = nullptr;
	}

	return nullptr;
}

FlatContext* Sandbox::AddFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	FlatContext* pFlatContext = m_pHALImp->MakeFlatContext(width, height, channels);
	//CR(m_pFlatSceneGraph->PushObject(pFlatContext));
	CN(pFlatContext);

	CR(AddObject(pFlatContext));

	return pFlatContext;

Error:
	if (pFlatContext != nullptr) {
		delete pFlatContext;
		pFlatContext = nullptr;
	}

	return nullptr;
}

RESULT Sandbox::RenderToTexture(FlatContext* pContext) {
	RESULT r = R_PASS;

	CR(m_pHALImp->RenderToTexture(pContext, m_pCamera));

Error:
	return r;
}

light* Sandbox::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pHALImp->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

light* Sandbox::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	RESULT r = R_PASS;

	light *pLight = MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
	CN(pLight);

	CR(AddObject(pLight));

//Success:
	return pLight;

Error:
	if (pLight != nullptr) {
		delete pLight;
		pLight = nullptr;
	}
	return nullptr;
}

quad* Sandbox::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	return m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
}

quad* Sandbox::AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) {
	RESULT r = R_PASS;

	quad *pQuad = m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
	CN(pQuad);

	CR(AddObject(pQuad));

//Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		delete pQuad;
		pQuad = nullptr;
	}

	return nullptr;
}

cylinder* Sandbox::MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	return m_pHALImp->MakeCylinder(radius, height, numAngularDivisions, numVerticalDivisions);
}

cylinder* Sandbox::AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	RESULT r = R_PASS;

	cylinder *pCylinder = m_pHALImp->MakeCylinder(radius, height, numAngularDivisions, numVerticalDivisions);
	CN(pCylinder);

	CR(AddObject(pCylinder));

	//Success:
	return pCylinder;

Error:
	if (pCylinder != nullptr) {
		delete pCylinder;
		pCylinder = nullptr;
	}
	return nullptr;
}

DimRay* Sandbox::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional);
}

DimRay* Sandbox::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	RESULT r = R_PASS;
	DimRay* pRay = m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional);
	CN(pRay);

	CR(AddObject(pRay));

	//Success:
	return pRay;

Error:
	if (pRay != nullptr) {
		delete pRay;
		pRay = nullptr;
	}
	return nullptr;
}

DimPlane* Sandbox::MakePlane(point ptOrigin, vector vNormal) {
	return m_pHALImp->MakePlane(ptOrigin, vNormal);
}

DimPlane* Sandbox::AddPlane(point ptOrigin, vector vNormal) {
	RESULT r = R_PASS;
	DimPlane* pPlane = MakePlane(ptOrigin, vNormal);
	CN(pPlane);

	CR(AddObject(pPlane));

	//Success:
	return pPlane;

Error:
	if (pPlane != nullptr) {
		delete pPlane;
		pPlane = nullptr;
	}
	return nullptr;
}

sphere* Sandbox::MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

sphere* Sandbox::AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	RESULT r = R_PASS;

	sphere *pSphere = m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions, c);
	CN(pSphere);

	CR(AddObject(pSphere));

//Success:
	return pSphere;

Error:
	if (pSphere != nullptr) {
		delete pSphere;
		pSphere = nullptr;
	}
	return nullptr;
}

volume* Sandbox::MakeVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pHALImp->MakeVolume(width, length, height, fTriangleBased);
}

volume* Sandbox::MakeVolume(double side, bool fTriangleBased) {
	return m_pHALImp->MakeVolume(side, fTriangleBased);
}

volume* Sandbox::AddVolume(double width, double length, double height, bool fTriangleBased) {
	RESULT r = R_PASS;

	volume *pVolume = MakeVolume(width, length, height, fTriangleBased);
	CN(pVolume);

	CR(AddObject(pVolume));

//Success:
	return pVolume;

Error:
	if (pVolume != nullptr) {
		delete pVolume;
		pVolume = nullptr;
	}
	return nullptr;
}

volume* Sandbox::AddVolume(double side, bool fTriangleBased) {
	return AddVolume(side, side, side, fTriangleBased);
}

text* Sandbox::AddText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = MakeText(pFont, pLayout, margin, textFlags);
	CN(pText);

	CR(AddObject(pText));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, pLayout, margin, textFlags);

	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = MakeText(pFont, strContent, lineHeightM, textFlags);
	CN(pText);

	CR(AddObject(pText));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, strContent, lineHeightM, textFlags);

	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	RESULT r = R_PASS;

	text *pText = MakeText(pFont, strContent, width, height, textFlags);
	CN(pText);

	CR(AddObject(pText));

	//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, strContent, width, height, textFlags);

	//*
	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}
	//*/
	//CR(r);

	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}


text* Sandbox::MakeText(std::shared_ptr<font> pFont, const std::string &strContent, double width, double height, bool fBillboard) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, strContent, width, height, true, fBillboard);

	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::AddText(std::shared_ptr<font> pFont, const std::string &strContent, double width, double height, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = MakeText(pFont, strContent, width, height, fBillboard);
	CN(pText);

	CR(AddObject(pText));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string &strContent, double width, double height, bool fBillboard) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, pFontTexture, strContent, width, height, true, fBillboard);

	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}

	return nullptr;
}

text* Sandbox::AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string &strContent, double width, double height, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = MakeText(pFont, pFontTexture, strContent, width, height, fBillboard);
	CN(pText);

	CR(AddObject(pText));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
	return nullptr;
}

text* Sandbox::AddText(const std::wstring & fontName, const std::string &strContent, double width, double height, bool fBillboard) {
	RESULT r = R_PASS;

	text *pText = m_pHALImp->MakeText(fontName, strContent, width, height, fBillboard);
	CN(pText);

	if (pText->IsRenderToQuad()) {
		CR(pText->RenderToQuad());
	}

	CR(AddObject(pText));

//Success:
	return pText;

Error:
	if (pText != nullptr) {
		delete pText;
		pText = nullptr;
	}
	return nullptr;
}

texture* Sandbox::MakeTexture(const texture &srcTexture) {
	return m_pHALImp->MakeTexture(srcTexture);
}

texture* Sandbox::MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
	return m_pHALImp->MakeTexture(type, width, height, pixelFormat, channels, pBuffer, pBuffer_n);
}

texture* Sandbox::MakeTexture(texture::type type, const wchar_t *pszFilename) {
	return m_pHALImp->MakeTexture(type, pszFilename);
}

texture* Sandbox::MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n) {
	return m_pHALImp->MakeTextureFromFileBuffer(type, pBuffer, pBuffer_n);
}

cubemap* Sandbox::MakeCubemap(const std::wstring &wstrCubemapName) {
	return m_pHALImp->MakeCubemap(wstrCubemapName);
}

skybox* Sandbox::MakeSkybox() {
	return m_pHALImp->MakeSkybox();
}

skybox *Sandbox::AddSkybox() {
	RESULT r = R_PASS;

	skybox *pSkybox = MakeSkybox();
	CN(pSkybox);

	CR(AddObject(pSkybox));

//Success:
	return pSkybox;

Error:
	if (pSkybox != nullptr) {
		delete pSkybox;
		pSkybox = nullptr;
	}
	return nullptr;
}

/*
model* SandboxApp::MakeModel(wchar_t *pszModelName) {
	return m_pHALImp->MakeModel(pszModelName);
}

model *SandboxApp::AddModel(wchar_t *pszModelName) {
	RESULT r = R_PASS;

	model* pModel = MakeModel(pszModelName);
	CN(pModel);

	CR(AddObject(pModel));

//Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}
	return nullptr;
}
*/

mesh* Sandbox::MakeMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	mesh* pMesh = m_pHALImp->MakeMesh(vertices);
	CN(pMesh);

	return pMesh;

Error:
	if (pMesh != nullptr) {
		delete pMesh;
		pMesh = nullptr;
	}
	return nullptr;
}

mesh* Sandbox::AddMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	mesh* pMesh = MakeMesh(vertices);
	CN(pMesh);

	CR(AddObject(pMesh));

	//Success:
	return pMesh;

Error:
	if (pMesh != nullptr) {
		delete pMesh;
		pMesh = nullptr;
	}
	return nullptr;
}

mesh* Sandbox::MakeMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	mesh* pMesh = m_pHALImp->MakeMesh(vertices, indices);
	CN(pMesh);

	return pMesh;

Error:
	if (pMesh != nullptr) {
		delete pMesh;
		pMesh = nullptr;
	}
	return nullptr;
}

mesh* Sandbox::AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	mesh* pMesh = MakeMesh(vertices, indices);
	CN(pMesh);

	CR(AddObject(pMesh));

	//Success:
	return pMesh;

Error:
	if (pMesh != nullptr) {
		delete pMesh;
		pMesh = nullptr;
	}
	return nullptr;
}

user *Sandbox::MakeUser() {
	RESULT r = R_PASS;

	user* pUser = m_pHALImp->MakeUser();
	CN(pUser);

	//Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		delete pUser;
		pUser = nullptr;
	}
	return nullptr;
}

user *Sandbox::AddUser() {
	RESULT r = R_PASS;

	user* pUser = MakeUser();
	CN(pUser);

	CR(AddObject(pUser));

	//Success:
	return pUser;

Error:
	if (pUser != nullptr) {
		delete pUser;
		pUser = nullptr;
	}
	return nullptr;
}

billboard *Sandbox::AddBillboard(point ptOrigin, float width, float height) {
	RESULT r = R_PASS;

	billboard *pBillboard = MakeBillboard(ptOrigin, width, height);
	CN(pBillboard);

	// billboards are always rendered by the billboard shader right now (also in aux with same scene graph)
	m_pBillboardSceneGraph->PushObject(pBillboard);

	return pBillboard;

Error:
	if (pBillboard != nullptr) {
		delete pBillboard;
		pBillboard = nullptr;
	}
	return nullptr;
}

billboard *Sandbox::MakeBillboard(point ptOrigin, float width, float height) {
	RESULT r = R_PASS;

	billboard *pBillboard = m_pHALImp->MakeBillboard(ptOrigin, width, height);
	CN(pBillboard);

	return pBillboard;

Error:
	if (pBillboard != nullptr) {
		delete pBillboard;
		pBillboard = nullptr;
	}
	return nullptr;
}

ProgramNode* Sandbox::MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags) {
	RESULT r = R_PASS;

	ProgramNode *pProgramNode = nullptr;

	CN(m_pHALImp);

	pProgramNode = m_pHALImp->MakeProgramNode(strNodeName, optFlags);
	CN(pProgramNode);

	return pProgramNode;

Error:
	if (pProgramNode != nullptr) {
		delete pProgramNode;
		pProgramNode = nullptr;
	}

	return nullptr;
}

HysteresisObject *Sandbox::MakeHysteresisObject(float onThreshold, float offThreshold, HysteresisObjectType objectType) {
	RESULT r = R_PASS;

	HysteresisObject *pObject = nullptr;

	switch (objectType) {
		case (SPHERE): {
			pObject = new HysteresisSphere(onThreshold, offThreshold);
		} break;
		case (CYLINDER): {
			pObject = new HysteresisCylinder(onThreshold, offThreshold);
		} break;
		case (PLANE): {
			pObject = new HysteresisPlane(onThreshold, offThreshold);
		} break;
	}

	CN(pObject);

	return pObject;

Error:
	if (pObject != nullptr) {
		delete pObject;
		pObject = nullptr;
	}
	return nullptr;
}

model* Sandbox::MakeModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	// TODO: Other bits (position, scale, rotation)

	model *pModel = ModelFactory::MakeModel(m_pHALImp, wstrModelFilename, ModelFactory::flags::NONE);
	CN(pModel);

// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

model* Sandbox::AddModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	model *pModel = MakeModel(wstrModelFilename, pTexture);
	CN(pModel);

	CR(AddObject(pModel));

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

model *Sandbox::MakeModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	RESULT r = R_PASS;

	// TODO: Other bits (position, scale, rotation)

	model *pModel = ModelFactory::MakeModel(m_pHALImp, wstrModelFilename, modelFactoryFlags);
	CN(pModel);

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

model *Sandbox::AddModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags) {
	RESULT r = R_PASS;

	model *pModel = MakeModel(wstrModelFilename, modelFactoryFlags);
	CN(pModel);

	CR(AddObject(pModel));

Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

composite* Sandbox::MakeComposite() {
	return m_pHALImp->MakeComposite();
}

composite* Sandbox::AddComposite() {
	RESULT r = R_PASS;

	composite* pComposite = MakeComposite();
	CN(pComposite);
	CR(AddObject(pComposite));

	//Success:
	return pComposite;

Error:
	if (pComposite != nullptr) {
		delete pComposite;
		pComposite = nullptr;
	}
	return nullptr;
}

RESULT Sandbox::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	RESULT r = R_PASS;

	CB((m_fnUpdateCallback == nullptr));
	m_fnUpdateCallback = fnUpdateCallback;

Error:
	return r;
}

RESULT Sandbox::UnregisterUpdateCallback() {
	RESULT r = R_PASS;

	CB((m_fnUpdateCallback != nullptr));
	m_fnUpdateCallback = nullptr;

Error:
	return r;
}

stereocamera* Sandbox::GetCamera() {
	return m_pCamera;
}

point Sandbox::GetCameraPosition() {
	return m_pHALImp->GetCamera()->GetPosition();
}

quaternion Sandbox::GetCameraOrientation() {
	return m_pHALImp->GetCamera()->GetWorldOrientation();
}

// Cloud Controller
RESULT Sandbox::RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver) {
	return m_pCloudController->RegisterPeerConnectionObserver(pPeerConnectionObserver);
}

RESULT Sandbox::RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver) {
	return m_pCloudController->RegisterEnvironmentObserver(pEnvironmentObserver);
}

RESULT Sandbox::RegisterUserObserver(CloudController::UserObserver *pUserObserver) {
	return m_pCloudController->RegisterUserObserver(pUserObserver);
}

RESULT Sandbox::BroadcastVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	return m_pCloudController->BroadcastVideoFrame(strVideoTrackLabel, pVideoFrameBuffer, pxWidth, pxHeight, channels);
}

RESULT Sandbox::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pCloudController->SendDataMessage(userID, pDataMessage);
}


RESULT Sandbox::BroadcastDataMessage(Message *pDataMessage) {
	return m_pCloudController->BroadcastDataMessage(pDataMessage);
}

RESULT Sandbox::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage, DreamAppMessage::flags messageFlags) {
	RESULT r = R_PASS;

	if ((messageFlags & DreamAppMessage::flags::SHARE_NETWORK) != DreamAppMessage::flags::NONE) {
		CN(pPeerConnection);
	}

	CN(pDreamAppMessage);

	CR(m_pDreamAppManager->HandleDreamAppMessage(pPeerConnection, pDreamAppMessage));

Error:
	return r;
}

RESULT Sandbox::BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage, DreamAppMessage::flags messageFlags) {
	RESULT r = R_PASS;

	CBM((m_pDreamAppManager->FindDreamAppWithName(pDreamAppMessage->GetDreamAppName())), "Cannot find dream app name %s", pDreamAppMessage->GetDreamAppName().c_str());

	if ((messageFlags & DreamAppMessage::flags::SHARE_NETWORK) != DreamAppMessage::flags::NONE) {
		CR(BroadcastDataMessage(pDreamAppMessage));
	}

	if ((messageFlags & DreamAppMessage::flags::SHARE_LOCAL) != DreamAppMessage::flags::NONE) {
		CR(HandleDreamAppMessage(nullptr, pDreamAppMessage, DreamAppMessage::flags::SHARE_LOCAL));
	}

Error:
	return r;
}

// TimeManager
RESULT Sandbox::RegisterSubscriber(TimeEventType timeEvent, Subscriber<TimeEvent>* pTimeSubscriber) {
	RESULT r = R_PASS;

	CR(m_pTimeManagerModule->RegisterSubscriber(timeEvent, pTimeSubscriber));

Error:
	return r;
}

// IO
RESULT Sandbox::RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	RESULT r = R_PASS;

	CR(((Publisher<SenseVirtualKey, SenseKeyboardEvent>*)m_pSenseKeyboard)->RegisterSubscriber(keyEvent, pKeyboardSubscriber));

Error:
	return r;
}

RESULT Sandbox::RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber) {
	RESULT r = R_PASS;

	CR(((Publisher<SenseTypingEventType, SenseTypingEvent>*)m_pSenseKeyboard)->RegisterSubscriber(typingEvent, pTypingSubscriber));

Error:
	return r;
}

RESULT Sandbox::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	RESULT r = R_PASS;

	CNM(m_pSenseMouse, "Mouse not initialized");
	CR(m_pSenseMouse->RegisterSubscriber(mouseEvent, pMouseSubscriber));

Error:
	return r;
}

RESULT Sandbox::RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber) {
	RESULT r = R_PASS;
	if (m_pHMD != nullptr) {
		SenseController *pSenseController = m_pHMD->GetSenseController();
		if (pSenseController != nullptr) {
			CR(pSenseController->RegisterSubscriber(controllerEvent, pControllerSubscriber));
		}
	}

Error:
	return r;
}

RESULT Sandbox::RegisterSubscriber(SenseGamepadEventType gamePadEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber) {
	RESULT r = R_PASS;

	CNM(m_pSenseGamepad, "Gamepad not initialized");
	CR(m_pSenseGamepad->RegisterSubscriber(gamePadEvent, pGamepadSubscriber));

Error:
	return r;
}

RESULT Sandbox::RegisterSubscriber(HMDEventType hmdEvent, Subscriber<HMDEvent>* pHMDEventSubscriber) {
	RESULT r = R_PASS;

	CNR(m_SandboxConfiguration.fUseHMD, R_SKIPPED);
	CNM(m_pHMD, "HMD not initialized");
	CR(m_pHMD->RegisterSubscriber(hmdEvent, pHMDEventSubscriber));

Error:
	return r;
}

RESULT Sandbox::UnregisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber) {
	RESULT r = R_PASS;
	if (m_pHMD != nullptr) {
		SenseController *pSenseController = m_pHMD->GetSenseController();
		if (pSenseController != nullptr) {
			CR(pSenseController->UnregisterSubscriber(controllerEvent, pControllerSubscriber));
		}
	}

Error:
	return r;
}

RESULT Sandbox::UnregisterSubscriber(SenseGamepadEventType gamePadEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber) {
	RESULT r = R_PASS;

	CNM(m_pSenseGamepad, "Gamepad not initialized");
	CR(m_pSenseGamepad->UnregisterSubscriber(gamePadEvent, pGamepadSubscriber));

Error:
	return r;
}

RESULT Sandbox::SetDreamOSHandle(DreamOS *pDreamOSHandle) {
	RESULT r = R_PASS;

	CN(pDreamOSHandle);
	m_pDreamOSHandle = pDreamOSHandle;

Error:
	return r;
}

DreamOS *Sandbox::GetDreamOSHandle() {
	return m_pDreamOSHandle;
}

std::wstring Sandbox::GetHardwareID() {
	return m_strHardwareID;
}

std::string Sandbox::GetHMDTypeString() {
	return m_pHMD->GetDeviceTypeString();
}
