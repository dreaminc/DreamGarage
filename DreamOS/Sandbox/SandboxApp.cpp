#include "SandboxApp.h"
#include "Cloud/CloudController.h"

#include "Cloud/Message/Message.h"

#include "Primitives/ray.h"

#include <HMD/OpenVR/OpenVRDevice.h>
#include <HMD/Oculus/OVR.h>

#include "DreamAppManager.h"
#include "DreamAppMessage.h"

#include "HAL/Pipeline/SinkNode.h"
#include "HAL/Pipeline/ProgramNode.h"

#include "Primitives/model/ModelFactory.h"

#include <HMD/HMDFactory.h>

SandboxApp::SandboxApp() :
	m_pPathManager(nullptr),
	m_pCommandLineManager(nullptr),
	m_pOpenGLRenderingContext(nullptr),
	m_pSceneGraph(nullptr),
	m_pUISceneGraph(nullptr),
	m_pUIClippingSceneGraph(nullptr),
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

SandboxApp::~SandboxApp() {
	if (m_pCloudController != nullptr) {
		delete m_pCloudController;
		m_pCloudController = nullptr;
	}

	if (m_pHALImp != nullptr) {
		delete m_pHALImp;
		m_pHALImp = nullptr;
	}

	if (m_pOpenGLRenderingContext != nullptr) {
		delete m_pOpenGLRenderingContext;
		m_pOpenGLRenderingContext = nullptr;
	}
}

RESULT SandboxApp::SetSandboxConfiguration(SandboxApp::configuration sandboxconf) {
	m_SandboxConfiguration = sandboxconf;
	return R_PASS;
}

InteractionEngineProxy *SandboxApp::GetInteractionEngineProxy() {
	return m_pInteractionEngine->GetInteractionEngineProxy();
}

const SandboxApp::configuration& SandboxApp::GetSandboxConfiguration() {
	return m_SandboxConfiguration;
}

RESULT SandboxApp::SetMouseIntersectObjects(bool fMouseIntersectObjects) {
	m_fMouseIntersectObjects = fMouseIntersectObjects;
	return R_PASS;
}

bool SandboxApp::IsMouseIntersectObjects() {
	return m_fMouseIntersectObjects;
}

RESULT SandboxApp::Notify(CmdPromptEvent *event) {
	RESULT r = R_PASS;

	if (event->GetArg(1).compare("intersect") == 0) {
		m_pHALImp->SetRenderReferenceGeometry(true);
		SetMouseIntersectObjects(!IsMouseIntersectObjects());
	}

	return r;
}

RESULT SandboxApp::Notify(SenseKeyboardEvent *kbEvent) {
	RESULT r = R_PASS;

	if (kbEvent->m_pSenseKeyboard) {
		SenseVirtualKey keyCode = kbEvent->KeyCode;

		if (kbEvent->KeyState) {
			if (m_pHALImp->IsRenderProfiler() == false) {
				if (keyCode == SVK_TAB) {
					// quick hack to enable dream console in production but only using several tab hits
#ifdef PRODUCTION_BUILD
					static int hits = 0;
					hits++;
					if (hits > 7) {
						m_pHALImp->SetRenderProfiler(true);
						DreamConsole::GetConsole()->SetInForeground(true);
					}
#else
					m_pHALImp->SetRenderProfiler(true);
					DreamConsole::GetConsole()->SetInForeground(true);
#endif // PRODUCTION_BUILD
				}
			}
			else {
				if (keyCode == SVK_TAB) {
					m_pHALImp->SetRenderProfiler(false);
					DreamConsole::GetConsole()->SetInForeground(false);
				}
			}
		}
	}

	//Error:
	return r;
}

RESULT SandboxApp::Notify(SenseTypingEvent *kbEvent) {
	return R_NOT_IMPLEMENTED;
}

RESULT SandboxApp::Notify(SenseMouseEvent *mEvent) {
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

RESULT SandboxApp::GetMouseRay(ray &rCast, double t){
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

RESULT SandboxApp::Notify(CollisionObjectEvent *oEvent) {
	RESULT r = R_PASS;



//Error:
	return r;
}

RESULT SandboxApp::Notify(CollisionGroupEvent* gEvent) {
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

RESULT SandboxApp::RegisterImpKeyboardEvents() {
	RESULT r = R_PASS;

	// Register Dream Console to keyboard events
	CR(RegisterSubscriber(SVK_ALL, DreamConsole::GetConsole()));
	CR(RegisterSubscriber(CHARACTER_TYPING, DreamConsole::GetConsole()));

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

RESULT SandboxApp::RegisterImpMouseEvents() {
	RESULT r = R_PASS;

	CR(RegisterSubscriber(SENSE_MOUSE_MOVE, this));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_DRAG_MOVE, this));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_UP, this));

	//camera *pCamera = m_pOpenGLImp->GetCamera();

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

// temp
#include "HAL/opengl/OGLHand.h"

// TODO: shouldn't be this way ultimately 
RESULT SandboxApp::RegisterImpLeapMotionEvents() {
	RESULT r = R_PASS;

	hand *pLeftHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp), HAND_TYPE::HAND_LEFT);
	hand *pRightHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp), HAND_TYPE::HAND_RIGHT);

	std::shared_ptr<DimObj> pLeftHandSharedPtr(pLeftHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pLeftHandSharedPtr);

	std::shared_ptr<DimObj> pRightHandSharedPtr(pRightHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pRightHandSharedPtr);

	pLeftHand->SetOriented(true);
	pRightHand->SetOriented(true);

	CR(m_pSenseLeapMotion->AttachHand(pLeftHand, HAND_TYPE::HAND_LEFT));
	CR(m_pSenseLeapMotion->AttachHand(pRightHand, HAND_TYPE::HAND_RIGHT));

Error:
	return r;
}

RESULT SandboxApp::RegisterImpControllerEvents() {
	RESULT r = R_PASS;

	if (m_pHMD != nullptr) {
		hand *pLeftHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp), HAND_TYPE::HAND_LEFT);
		hand *pRightHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp), HAND_TYPE::HAND_RIGHT);

		pLeftHand->SetOriented(false);
		pRightHand->SetOriented(false);

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

//hand *Windows64App::AttachHand

hand *SandboxApp::GetHand(HAND_TYPE handType) {
	if (m_pHMD != nullptr) {
		return m_pHMD->GetHand(handType);
	}

	if (m_pSenseLeapMotion != nullptr) {
		return m_pSenseLeapMotion->GetHand(handType);
	}
	
	return nullptr;
}


bool SandboxApp::IsSandboxRunning() {
	return m_fRunning;
}

RESULT SandboxApp::SetSandboxRunning(bool fRunning) {
	m_fRunning = fRunning;
	return R_SUCCESS;
}

inline PathManager* SandboxApp::GetPathManager() {
	return m_pPathManager; 
}

inline OpenGLRenderingContext * SandboxApp::GetOpenGLRenderingContext() {
	return m_pOpenGLRenderingContext; 
}

RESULT SandboxApp::Shutdown() {
	RESULT r = R_SUCCESS;

	if (m_pDreamAppManager != nullptr) {
		CR(m_pDreamAppManager->Shutdown());
		m_pDreamAppManager = nullptr;
	}

	// Implementation specific shutdown
	CR(ShutdownSandbox());

Error:
	return r;
}

RESULT SandboxApp::RunAppLoop() {
	RESULT r = R_PASS;

	// Launch main message loop
	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

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

		// Time Manager
		CR(m_pTimeManager->Update());

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
			m_pHMD->UpdateHMD();
		}

		// Update Scene 
		//CR(m_pSceneGraph->UpdateScene());

		// TODO: Do these need to be wired up this way?
		// Why not just do an Update with retained graph

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

		//m_pOpenGLImp->RenderStereo(m_pSceneGraph);
		//m_pOpenGLImp->Render(m_pSceneGraph);
		m_pHALImp->Render();

		// Swap buffers
		SwapDisplayBuffers();

		//DreamConsole::GetConsole()->OnFrameRendered();

		if (GetAsyncKeyState(VK_ESCAPE) && !DreamConsole::GetConsole()->IsInForeground()) {
			Shutdown();
		}
	}

Error:
	return r;
}

RESULT SandboxApp::ResizeViewport(viewport newViewport) {
	RESULT r = R_PASS;

	// Resize the camera
	CR(m_pCamera->ResizeCamera(newViewport));

	// OpenGL Resize the view after the window had been resized
	CRM(m_pHALImp->Resize(m_viewport), "Failed to resize OpenGL Implemenation");

Error:
	return r;
}

RESULT SandboxApp::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	// Set up command line manager
	m_pCommandLineManager = CommandLineManager::instance();
	CN(m_pCommandLineManager);
	
	// previous AWS server
	// CR(m_pCommandLineManager->RegisterParameter("api.ip", "api.ip", "http://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));
	// CR(m_pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "ws://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));

	CR(m_pCommandLineManager->RegisterParameter("www.ip", "www.ip", "https://www.develop.dreamos.com:443"));
	CR(m_pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.develop.dreamos.com:443"));
	CR(m_pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.develop.dreamos.com:443"));
	CR(m_pCommandLineManager->RegisterParameter("otk.id", "otk.id", "INVALIDONETIMEKEY"));

	CR(m_pCommandLineManager->RegisterParameter("username", "u", "defaulttestuser@dreamos.com"));
	CR(m_pCommandLineManager->RegisterParameter("password", "p", "Nightmare479!"));
	CR(m_pCommandLineManager->RegisterParameter("hmd", "h", ""));
	CR(m_pCommandLineManager->RegisterParameter("leap", "lp", ""));
	CR(m_pCommandLineManager->RegisterParameter("testval", "t", "1"));

	// This can attempt to connect to a given environment
	CR(m_pCommandLineManager->RegisterParameter("environment", "env", "default"));

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

	m_pUISceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pUISceneGraph, "Failed to allocate UI Scene Graph");
	
	m_pUIClippingSceneGraph = DNode::MakeNode<ObjectStoreNode>(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pUIClippingSceneGraph, "Failed to allocate UI Clipping Scene Graph");

	// This will prevent scene graph from being deleted when not connected
	// TODO: Attach to Sandbox somehow?
	CB(m_pSceneGraph->incRefCount());
	CB(m_pUISceneGraph->incRefCount());
	CB(m_pUIClippingSceneGraph->incRefCount());

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

	// Set up the pipeline
	CR(SetUpHALPipeline(m_pHALImp->GetRenderPipelineHandle()));

	// Generalize this module pattern
	if (m_SandboxConfiguration.fInitCloud) {
		CRM(InitializeCloudController(), "Failed to initialize cloud controller");
	}

	CRM(InitializeTimeManager(), "Failed to initialize time manager");
	CRM(InitializeDreamAppManager(), "Failed to initialize app manager");

	if ((m_pCommandLineManager->GetParameterValue("leap").compare("") == 0) == false) {
		m_SandboxConfiguration.fUseLeap = false;
	}

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");

	// TODO: These have dependencies potentially on previous modules
	// TODO: Need to create proper module loading / dependency system
	CRM(InitializePhysicsEngine(), "Failed to initialize physics engine");
	CRM(InitializeInteractionEngine(), "Failed to initialize interaction engine");

	CommandLineManager::instance()->ForEach([](const std::string& arg) {
		HUD_OUT(("arg :" + arg).c_str());
	});

	// Auto Login Handling
	// This is done in DreamOS now
	//if (m_pCommandLineManager->GetParameterValue("login").compare("auto") == 0) {
	//	// auto login
	//	m_pCloudController->Start();
	//}

	// Register with command prompt
	// TODO: This should be changed to a command pattern
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::Sandbox, this);

Error:
	return r;
}

RESULT SandboxApp::InitializePhysicsEngine() {
	RESULT r = R_PASS;

	m_pPhysicsEngine = PhysicsEngine::MakePhysicsEngine();
	CNMW(m_pPhysicsEngine, "Physics Engine failed to initialize");

	// Set up physics graph
	m_pPhysicsGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pPhysicsGraph, "Failed to allocate Physics Graph");

	CRM(m_pPhysicsEngine->SetPhysicsGraph(m_pPhysicsGraph), "Failed to set physics object store");

	// Register OBJECT_GROUP_COLLISION
	// CR(m_pPhysicsEngine->RegisterSubscriber(OBJECT_GROUP_COLLISION, this));

Error:
	return r;
}

RESULT SandboxApp::InitializeInteractionEngine() {
	RESULT r = R_PASS;

	m_pInteractionEngine = InteractionEngine::MakeEngine(this);
	CNMW(m_pInteractionEngine, "Interaction Engine failed to initialize");

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

RESULT SandboxApp::InitializeTimeManager() {
	RESULT r = R_PASS;

	// Initialize Time Manager
	m_pTimeManager = std::make_unique<TimeManager>();

	CNM(m_pTimeManager, "Failed to allocate Time Manager");
	CVM(m_pTimeManager, "Failed to validate Time Manager");

Error:
	return r;
}

RESULT SandboxApp::InitializeDreamAppManager() {
	RESULT r = R_PASS;

	// Initialize Time Manager
	m_pDreamAppManager = std::make_unique<DreamAppManager>(GetDreamOSHandle());

	CNM(m_pDreamAppManager, "Failed to allocate Dream App Manager");
	CVM(m_pDreamAppManager, "Failed to validate Dream App Manager");

Error:
	return r;
}

RESULT SandboxApp::InitializeCamera() {
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
RESULT SandboxApp::InitializeHMD() {
	RESULT r = R_PASS;

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	int pxWidth, pxHeight;
	GetSandboxWindowSize(pxWidth, pxHeight);

	if (GetSandboxConfiguration().fUseHMD) {
		//m_pHMD = HMDFactory::MakeHMD(HMD_OVR, this, m_pHALImp, pxWidth, pxHeight);
		//m_pHMD = HMDFactory::MakeHMD(HMD_OPENVR, this, m_pHALImp, pxWidth, pxHeight);
		m_pHMD = HMDFactory::MakeHMD(HMD_ANY_AVAILABLE, this, m_pHALImp, pxWidth, pxHeight);

		if (m_pHMD != nullptr) {
			CRM(m_pHALImp->SetHMD(m_pHMD), "Failed to initialize stereo frame buffers");
		}
	}

Error:
	return r;
}

RESULT SandboxApp::InitializeHAL() {
	RESULT r = R_PASS;

	// Setup OpenGL and Resize Windows etc
	//CNM(m_hDC, "Can't start Sandbox with NULL Device Context");
	CNM(m_pCamera, "HAL depends on camera being set up");

	// Create and initialize OpenGL Imp
	// TODO: HAL factory pattern
	m_pHALImp = new OpenGLImp(m_pOpenGLRenderingContext);
	CNM(m_pHALImp, "Failed to create HAL Implementation");
	CVM(m_pHALImp, "HAL Implementation Invalid");

	CR(m_pHALImp->SetCamera(m_pCamera));

	CR(m_pHALImp->InitializeHAL());
	CR(m_pHALImp->InitializeRenderPipeline());

Error:
	return r;
}

// TODO: Move this up to DreamOS
RESULT SandboxApp::SetUpHALPipeline(Pipeline* pRenderPipeline) {
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

Error:
	return r;
}

RESULT SandboxApp::RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber) {
	RESULT r = R_PASS;

	r = m_pPhysicsEngine->RegisterObjectCollisionSubscriber(pVirtualObject, pCollisionDetectorSubscriber);
	CR(r);

Error:
	return r;
}

RESULT SandboxApp::RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	//r = m_pInteractionEngine->RegisterSubscriber(eventType, pInteractionSubscriber);
	CR(r);

Error:
	return r;
}

RESULT SandboxApp::RegisterEventSubscriber(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	r = m_pInteractionEngine->RegisterSubscriber(pObject, eventType, pInteractionSubscriber);
	CR(r);

Error:
	return r;
}

RESULT SandboxApp::UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pInteractionEngine->UnregisterSubscriber(pObject, eventType, pInteractionSubscriber);
}

RESULT SandboxApp::UnregisterInteractionSubscriber(Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	return m_pInteractionEngine->UnregisterSubscriber(pInteractionSubscriber);
}

RESULT SandboxApp::UnregisterInteractionObject(VirtualObj *pObject) {
	return m_pInteractionEngine->UnregisterSubscriber(pObject);
}

long SandboxApp::GetTickCount() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

RESULT SandboxApp::SetHALConfiguration(HALImp::HALConfiguration halconf) {
	return m_pHALImp->SetHALConfiguration(halconf);
}

const HALImp::HALConfiguration& SandboxApp::GetHALConfiguration() {
	return m_pHALImp->GetHALConfiguration();
}

// Sandbox Factory Methods
RESULT SandboxApp::AddObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pSceneGraph->PushObject(pObject));

Error:
	return r;
}

// This adds the object to the physics graph (otherwise it will not get integrated / operated on)
RESULT SandboxApp::AddPhysicsObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pPhysicsGraph->PushObject(pObject));

Error:
	return r;
}

// This adds the object to the interaction graph (otherwise it will not be included in event handling)
RESULT SandboxApp::AddObjectToInteractionGraph(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pInteractionGraph->PushObject(pObject));

Error:
	return r;
}

RESULT SandboxApp::AddInteractionObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pInteractionEngine->AddInteractionObject(pObject));

Error:
	return r;
}

RESULT SandboxApp::AddObjectToUIGraph(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pUISceneGraph->PushObject(pObject));

Error:
	return r;
}

RESULT SandboxApp::AddObjectToUIClippingGraph(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pUIClippingSceneGraph->PushObject(pObject));

Error:
	return r;
}

/*
RESULT SandboxApp::UpdateInteractionPrimitive(const ray &rCast) {
	RESULT r = R_PASS;

	CR(m_pInteractionEngine->UpdateInteractionPrimitive(rCast));

Error:
	return r;
}
*/

RESULT SandboxApp::RemoveObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	DimObj *pObj = reinterpret_cast<DimObj*>(pObject);
	if (pObj != nullptr)
		CR(m_pInteractionEngine->RemoveAnimationObject(pObj));

	CR(m_pPhysicsGraph->RemoveObject(pObject));
	CR(m_pSceneGraph->RemoveObject(pObject));
	CR(m_pUISceneGraph->RemoveObject(pObject));
	CR(m_pUIClippingSceneGraph->RemoveObject(pObject));
	CR(m_pInteractionGraph->RemoveObject(pObject));

Error:
	return r;
}

// This is the nuclear option - it will flush all objects out
RESULT SandboxApp::RemoveAllObjects() {
	RESULT r = R_PASS;

	// removes all animations
	CR(m_pInteractionEngine->RemoveAllObjects()); 

	CR(m_pPhysicsGraph->RemoveAllObjects());
	CR(m_pSceneGraph->RemoveAllObjects());
	CR(m_pUISceneGraph->RemoveAllObjects());
	CR(m_pUIClippingSceneGraph->RemoveAllObjects());
	CR(m_pInteractionGraph->RemoveAllObjects());

Error:
	return r;
}

RESULT SandboxApp::SetGravityAcceleration(double acceleration) {
	RESULT r = R_PASS;

	CR(m_pPhysicsEngine->SetGravityAccelration(acceleration));

Error:
	return r;
}

RESULT SandboxApp::SetGravityState(bool fEnabled) {
	RESULT r = R_PASS;

	CR(m_pPhysicsEngine->SetGravityState(fEnabled));

Error:
	return r;
}

FlatContext* SandboxApp::AddFlatContext(int width, int height, int channels) {
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

RESULT SandboxApp::RenderToTexture(FlatContext* pContext) {
	RESULT r = R_PASS;
	
	CR(m_pHALImp->RenderToTexture(pContext, m_pCamera));

Error:
	return r;
}

light* SandboxApp::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pHALImp->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

light* SandboxApp::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
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

quad* SandboxApp::MakeQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	return m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
}

quad* SandboxApp::AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector()) {
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

cylinder* SandboxApp::MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	return m_pHALImp->MakeCylinder(radius, height, numAngularDivisions, numVerticalDivisions);
}

cylinder* SandboxApp::AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
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

DimRay* SandboxApp::MakeRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pHALImp->MakeRay(ptOrigin, vDirection, step, fDirectional); 
}

DimRay* SandboxApp::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
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



sphere* SandboxApp::MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

sphere* SandboxApp::AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
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

volume* SandboxApp::MakeVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pHALImp->MakeVolume(width, length, height, fTriangleBased);
}

volume* SandboxApp::MakeVolume(double side, bool fTriangleBased) {
	return m_pHALImp->MakeVolume(side, fTriangleBased);
}

volume* SandboxApp::AddVolume(double width, double length, double height, bool fTriangleBased) {
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

volume* SandboxApp::AddVolume(double side, bool fTriangleBased) {
	return AddVolume(side, side, side, fTriangleBased);
}

text* SandboxApp::AddText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
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

text* SandboxApp::MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags) {
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

text* SandboxApp::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
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

text* SandboxApp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM, text::flags textFlags) {
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

text* SandboxApp::AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
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

text* SandboxApp::MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width, double height, text::flags textFlags) {
	RESULT r = R_PASS;

	auto pText = m_pHALImp->MakeText(pFont, strContent, width, height, textFlags);

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


text* SandboxApp::MakeText(std::shared_ptr<font> pFont, const std::string &strContent, double width, double height, bool fBillboard) {
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

text* SandboxApp::AddText(std::shared_ptr<font> pFont, const std::string &strContent, double width, double height, bool fBillboard) {
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

text* SandboxApp::MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string &strContent, double width, double height, bool fBillboard) {
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

text* SandboxApp::AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string &strContent, double width, double height, bool fBillboard) {
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

text* SandboxApp::AddText(const std::wstring & fontName, const std::string &strContent, double width, double height, bool fBillboard) {
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

texture* SandboxApp::MakeTexture(const texture &srcTexture) {
	return m_pHALImp->MakeTexture(srcTexture);
}

texture* SandboxApp::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n) {
	return m_pHALImp->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_DIFFUSE, width, height, pixelFormat, channels, pBuffer, pBuffer_n);
}

texture* SandboxApp::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pHALImp->MakeTexture(pszFilename, type);
}

texture* SandboxApp::MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type) {
	return m_pHALImp->MakeTextureFromFileBuffer(pBuffer, pBuffer_n, type);
}

skybox* SandboxApp::MakeSkybox() {
	return m_pHALImp->MakeSkybox();
}

skybox *SandboxApp::AddSkybox() {
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

mesh* SandboxApp::AddMesh(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	mesh* pMesh = m_pHALImp->MakeMesh(vertices);
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

mesh* SandboxApp::AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	mesh* pMesh = m_pHALImp->MakeMesh(vertices, indices);
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

user *SandboxApp::MakeUser() {
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

user *SandboxApp::AddUser() {
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

model* SandboxApp::MakeModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	// TODO: Other bits (position, scale, rotation)

	model *pModel = ModelFactory::MakeModel(m_pHALImp, wstrModelFilename);
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

model* SandboxApp::AddModel(const std::wstring& wstrModelFilename, texture* pTexture) {
	RESULT r = R_PASS;

	model *pModel = MakeModel(wstrModelFilename, pTexture);
	CN(pModel);

	CR(AddObject(pModel));

// Success:
	return pModel;

Error:
	if (pModel != nullptr) {
		delete pModel;
		pModel = nullptr;
	}

	return nullptr;
}

composite* SandboxApp::MakeComposite() {
	return m_pHALImp->MakeComposite();
}

composite* SandboxApp::AddComposite() {
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

RESULT SandboxApp::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	RESULT r = R_PASS;

	CB((m_fnUpdateCallback == nullptr));
	m_fnUpdateCallback = fnUpdateCallback;

Error:
	return r;
}

RESULT SandboxApp::UnregisterUpdateCallback() {
	RESULT r = R_PASS;

	CB((m_fnUpdateCallback != nullptr));
	m_fnUpdateCallback = nullptr;

Error:
	return r;
}

stereocamera* SandboxApp::GetCamera() {
	return m_pCamera;
}

point SandboxApp::GetCameraPosition() {
	return m_pHALImp->GetCamera()->GetPosition();
}

quaternion SandboxApp::GetCameraOrientation() {
	return m_pHALImp->GetCamera()->GetWorldOrientation();
}

// Cloud Controller
RESULT SandboxApp::RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver) {
	return m_pCloudController->RegisterPeerConnectionObserver(pPeerConnectionObserver);
}

RESULT SandboxApp::RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver) {
	return m_pCloudController->RegisterEnvironmentObserver(pEnvironmentObserver);
}

RESULT SandboxApp::BroadcastVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels) {
	return m_pCloudController->BroadcastVideoFrame(pVideoFrameBuffer, pxWidth, pxHeight, channels);
}

RESULT SandboxApp::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pCloudController->SendDataMessage(userID, pDataMessage);
}


RESULT SandboxApp::BroadcastDataMessage(Message *pDataMessage) {
	return m_pCloudController->BroadcastDataMessage(pDataMessage);
}

RESULT SandboxApp::HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	CN(pPeerConnection);
	CN(pDreamAppMessage);

	CR(m_pDreamAppManager->HandleDreamAppMessage(pPeerConnection, pDreamAppMessage));

Error:
	return r;
}

RESULT SandboxApp::BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage) {
	RESULT r = R_PASS;

	CBM((m_pDreamAppManager->FindDreamAppWithName(pDreamAppMessage->GetDreamAppName())), "Cannot find dream app name %s", pDreamAppMessage->GetDreamAppName().c_str());

	CR(BroadcastDataMessage(pDreamAppMessage));

Error:
	return r;
}

// TimeManager
RESULT SandboxApp::RegisterSubscriber(TimeEventType timeEvent, Subscriber<TimeEvent>* pTimeSubscriber) {
	RESULT r = R_PASS;

	CR(m_pTimeManager->RegisterSubscriber(timeEvent, pTimeSubscriber));

Error:
	return r;
}

// IO
RESULT SandboxApp::RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	RESULT r = R_PASS;

	CR(((Publisher<SenseVirtualKey, SenseKeyboardEvent>*)m_pSenseKeyboard)->RegisterSubscriber(keyEvent, pKeyboardSubscriber));

Error:
	return r;
}

RESULT SandboxApp::RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber) {
	RESULT r = R_PASS;

	CR(((Publisher<SenseTypingEventType, SenseTypingEvent>*)m_pSenseKeyboard)->RegisterSubscriber(typingEvent, pTypingSubscriber));

Error:
	return r;
}

RESULT SandboxApp::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	RESULT r = R_PASS;

	CNM(m_pSenseMouse, "Mouse not initialized");
	CR(m_pSenseMouse->RegisterSubscriber(mouseEvent, pMouseSubscriber));

Error:
	return r;
}

RESULT SandboxApp::RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber) {
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

RESULT SandboxApp::SetDreamOSHandle(DreamOS *pDreamOSHandle) {
	RESULT r = R_PASS;

	CN(pDreamOSHandle);
	m_pDreamOSHandle = pDreamOSHandle;

Error:
	return r;
}

DreamOS *SandboxApp::GetDreamOSHandle() {
	return m_pDreamOSHandle;
}