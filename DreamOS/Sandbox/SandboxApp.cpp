#include "SandboxApp.h"
#include "Cloud/CloudController.h"

#include "Cloud/Message/Message.h"

#include "Primitives/ray.h"

SandboxApp::SandboxApp() :
	m_pPathManager(nullptr),
	m_pCommandLineManager(nullptr),
	m_pOpenGLRenderingContext(NULL),
	m_pSceneGraph(NULL),
	m_pCloudController(nullptr),
	m_pHALImp(nullptr),
	m_pHMD(nullptr),
	m_fnUpdateCallback(nullptr),
	m_pSenseLeapMotion(nullptr),
	m_pPhysicsEngine(nullptr)
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
				camera *pCamera = m_pHALImp->GetCamera();
				ray rayCamera = pCamera->GetRay(mEvent->xPos, mEvent->yPos);

				// intersect ray
				auto intersectedObjects = m_pSceneGraph->GetObjects(rayCamera);

				// TODO: Junk code:
				for (auto &pObject : m_pSceneGraph->GetObjects()) {

					DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
					if (pDimObj != nullptr) {
						pDimObj->SetColor(color(COLOR_WHITE));
					}
				}

				for (auto &pObject : intersectedObjects) {
					DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
					pDimObj->SetColor(color(COLOR_RED));
				}
			}
		} break;
	}

//Error:
	return r;
}

RESULT SandboxApp::RegisterImpKeyboardEvents() {
	RESULT r = R_PASS;

	// Register Dream Console to keyboard events
	CR(RegisterSubscriber(SK_ALL, DreamConsole::GetConsole()));

	camera *pCamera = m_pHALImp->GetCamera();

	CR(RegisterSubscriber(TIME_ELAPSED, pCamera));

	/*
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_LEFT, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_UP, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_DOWN, m_pOpenGLImp));
	CR(m_pWin64Keyboard->RegisterSubscriber(VK_RIGHT, m_pOpenGLImp));

	for (int i = 0; i < 26; i++) {
	CR(m_pWin64Keyboard->RegisterSubscriber((SK_SCAN_CODE)('A' + i), m_pOpenGLImp));
	}
	*/

	CR(RegisterSubscriber(VK_LEFT, pCamera));
	CR(RegisterSubscriber(VK_UP, pCamera));
	CR(RegisterSubscriber(VK_DOWN, pCamera));
	CR(RegisterSubscriber(VK_RIGHT, pCamera));

	CR(RegisterSubscriber(VK_SPACE, pCamera));

	for (int i = 0; i < 26; i++) {
		CR(RegisterSubscriber((SK_SCAN_CODE)('A' + i), pCamera));
	}

	CR(RegisterSubscriber((SK_SCAN_CODE)('F'), m_pHALImp));
	//CR(m_pWin64Keyboard->UnregisterSubscriber((SK_SCAN_CODE)('F'), pCamera));

Error:
	return r;
}

RESULT SandboxApp::RegisterImpMouseEvents() {
	RESULT r = R_PASS;

	CR(RegisterSubscriber(SENSE_MOUSE_MOVE, this));

	//camera *pCamera = m_pOpenGLImp->GetCamera();

	// TODO: Should either be moved up to the sandbox or into the Imp itself
	//CR(RegisterSubscriber(SENSE_MOUSE_MOVE, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_DRAG_MOVE, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_UP, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_LEFT_BUTTON_DOWN, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON_DOWN, m_pHALImp));
	CR(RegisterSubscriber(SENSE_MOUSE_RIGHT_BUTTON_UP, m_pHALImp));


Error:
	return r;
}

// temp
#include "HAL/opengl/OGLHand.h"

// TODO: shouldn't be this way ultimately 
RESULT SandboxApp::RegisterImpLeapMotionEvents() {
	RESULT r = R_PASS;

	hand *pLeftHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp));
	hand *pRightHand = new OGLHand(reinterpret_cast<OpenGLImp*>(m_pHALImp));

	std::shared_ptr<DimObj> pLeftHandSharedPtr(pLeftHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pLeftHandSharedPtr);

	std::shared_ptr<DimObj> pRightHandSharedPtr(pRightHand);
	m_pHALImp->GetCamera()->AddObjectToFrameOfReferenceComposite(pRightHandSharedPtr);

	CR(m_pSenseLeapMotion->AttachHand(pLeftHand, hand::HAND_LEFT));
	CR(m_pSenseLeapMotion->AttachHand(pRightHand, hand::HAND_RIGHT));

Error:
	return r;
}

inline PathManager* SandboxApp::GetPathManager() {
	return m_pPathManager; 
}

inline OpenGLRenderingContext * SandboxApp::GetOpenGLRenderingContext() {
	return m_pOpenGLRenderingContext; 
}

RESULT SandboxApp::RunAppLoop() {
	RESULT r = R_PASS;

	// Launch main message loop
	bool fQuit = false;

	CN(m_pHALImp);
	CR(m_pHALImp->MakeCurrentContext());

	// TODO: This should be moved to the sandbox
	while (!fQuit) {
		CR(HandleMessages());	// Handle windows messages

#ifdef CEF_ENABLED
		// Update Network
		CR(m_pCloudController->Update());
#endif

		// Time Manager
		CR(m_pTimeManager->Update());

		// Update Callback
		if (m_fnUpdateCallback != nullptr) {
			CR(m_fnUpdateCallback());
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

		CR(m_pPhysicsEngine->UpdateObjectStore(m_pSceneGraph));

		// Update HMD
		if (m_pHMD != nullptr) {
			m_pHALImp->SetCameraOrientation(m_pHMD->GetHMDOrientation());
			m_pHALImp->SetCameraPositionDeviation(m_pHMD->GetHMDTrackerDeviation());
		}

		//m_pOpenGLImp->RenderStereo(m_pSceneGraph);
		//m_pOpenGLImp->Render(m_pSceneGraph);

		///*
		// Send to the HMD
		// TODO reorganize Render functions
		// need to be re-architected so that the HMD functions are called after all of the 
		// GL functions per eye.
		if (m_pHMD != nullptr) {
			//m_pHALImp->RenderStereoFramebuffers(m_pSceneGraph);
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_LEFT);
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_RIGHT);
			m_pHMD->SubmitFrame();
			m_pHMD->RenderHMDMirror();
		}
		else {
			// Render Scene
			m_pHALImp->Render(m_pSceneGraph, m_pFlatSceneGraph, EYE_MONO);
		}
		//*/

		// Swap buffers
		SwapDisplayBuffers();

		DreamConsole::GetConsole()->OnFrameRendered();

		if (GetAsyncKeyState(VK_ESCAPE) && !DreamConsole::GetConsole()->IsInForeground()) {
			Shutdown();
			fQuit = true;
		}
	}

Error:
	return r;
}

RESULT SandboxApp::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	// Set up command line manager
	m_pCommandLineManager = CommandLineManager::instance();
	CN(m_pCommandLineManager);
	
	// previous AWS server
	//CR(m_pCommandLineManager->RegisterParameter("api.ip", "api.ip", "http://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));
	//CR(m_pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "ws://ec2-54-175-210-194.compute-1.amazonaws.com:8000"));

	CR(m_pCommandLineManager->RegisterParameter("api.ip", "api.ip", "https://api.develop.dreamos.com:443"));
	CR(m_pCommandLineManager->RegisterParameter("ws.ip", "ws.ip", "wss://ws.develop.dreamos.com:443"));

	CR(m_pCommandLineManager->RegisterParameter("username", "u", "DefaultTestUser@dreamos.com"));
	CR(m_pCommandLineManager->RegisterParameter("password", "p", "nightmare"));
	CR(m_pCommandLineManager->RegisterParameter("hmd", "h", ""));

	// For auto login, use '-l auto'
	CR(m_pCommandLineManager->RegisterParameter("login", "l", "no"));

	CR(m_pCommandLineManager->InitializeFromCommandLine(argc, argv));

	// Set up Scene Graph
	m_pSceneGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");

	m_pFlatSceneGraph = new ObjectStore(ObjectStoreFactory::TYPE::LIST);
	CNM(m_pFlatSceneGraph, "Failed to allocate Scene Graph");

	CRM(InitializeHAL(), "Failed to initialize HAL");

	CRM(InitializeCloudController(), "Failed to initialize cloud controller");

	// Initialize Physics Engine
	m_pPhysicsEngine = PhysicsEngine::MakePhysicsEngine();
	CNMW(m_pPhysicsEngine, "Physics Engine failed to initialize");

	m_fCheckHMD = (m_pCommandLineManager->GetParameterValue("hmd").compare("") == 0);

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");

	CommandLineManager::instance()->ForEach([](const std::string& arg) {
		HUD_OUT(("arg :" + arg).c_str());
	});

	// Auto Login Handling
	if (m_pCommandLineManager->GetParameterValue("login").compare("auto") == 0) {
		// auto login
		m_pCloudController->Start();
	}

	// Register with command prompt
	// TODO: This should be changed to a command pattern
	CmdPrompt::GetCmdPrompt()->RegisterMethod(CmdPrompt::method::Sandbox, this);

Error:
	return r;
}

long SandboxApp::GetTickCount() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

// Sandbox Factory Methods
// TODO: This should all go up into the sandbox
RESULT SandboxApp::AddObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pSceneGraph->PushObject(pObject));

Error:
	return r;
}

FlatContext* SandboxApp::AddFlatContext(int width, int height, int channels) {
	RESULT r = R_PASS;

	FlatContext* context = m_pHALImp->MakeFlatContext(width, height, channels);
	CR(m_pFlatSceneGraph->PushObject(context));

Error:
	return context;
}

RESULT SandboxApp::RenderToTexture(FlatContext* pContext) {
	RESULT r = R_PASS;
	
	CR(m_pHALImp->RenderToTexture(pContext));
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
	
quad* SandboxApp::AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) {
	RESULT r = R_PASS;

	quad *pQuad = m_pHALImp->MakeQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight);
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

text* SandboxApp::AddText(const std::wstring & fontName, const std::string & content, double size, bool isBillboard)
{
	RESULT r = R_PASS;

	text *pText = m_pHALImp->MakeText(fontName, content, size, isBillboard);
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

texture* SandboxApp::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n) {
	return m_pHALImp->MakeTexture(texture::TEXTURE_TYPE::TEXTURE_COLOR, width, height, channels, pBuffer, pBuffer_n);
}

texture* SandboxApp::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pHALImp->MakeTexture(pszFilename, type);
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

model *SandboxApp::AddModel(const std::vector<vertex>& vertices) {
	RESULT r = R_PASS;

	model* pModel = m_pHALImp->MakeModel(vertices);
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

model *SandboxApp::AddModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices) {
	RESULT r = R_PASS;

	model* pModel = m_pHALImp->MakeModel(vertices, indices);
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

user *SandboxApp::AddUser() {
	RESULT r = R_PASS;

	user* pUser = m_pHALImp->MakeUser();
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

composite* SandboxApp::AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	return m_pHALImp->LoadModel(m_pSceneGraph, wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation);
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

camera* SandboxApp::GetCamera() {
	return m_pHALImp->GetCamera();
}

point SandboxApp::GetCameraPosition() {
	return m_pHALImp->GetCamera()->GetPosition();
}

quaternion SandboxApp::GetCameraOrientation() {
	return m_pHALImp->GetCamera()->GetWorldOrientation();
}

// TODO: This should move up to Sandbox
hand *SandboxApp::GetHand(hand::HAND_TYPE handType) {
	return m_pSenseLeapMotion->GetHand(handType);
}

// Cloud Controller
RESULT SandboxApp::RegisterPeersUpdateCallback(HandlePeersUpdateCallback fnHandlePeersUpdateCallback) {
	return m_pCloudController->RegisterPeersUpdateCallback(fnHandlePeersUpdateCallback);
}

RESULT SandboxApp::RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback) {
	return m_pCloudController->RegisterDataMessageCallback(fnHandleDataMessageCallback);
}

RESULT SandboxApp::RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback) {
	return m_pCloudController->RegisterHeadUpdateMessageCallback(fnHandleHeadUpdateMessageCallback);
}

RESULT SandboxApp::RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback) {
	return m_pCloudController->RegisterHandUpdateMessageCallback(fnHandleHandUpdateMessageCallback);
}

RESULT SandboxApp::RegisterAudioDataCallback(HandleAudioDataCallback fnHandleAudioDataCallback) {
	return m_pCloudController->RegisterAudioDataCallback(fnHandleAudioDataCallback);
}

RESULT SandboxApp::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pCloudController->SendDataMessage(userID, pDataMessage);
}

RESULT SandboxApp::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	return m_pCloudController->SendUpdateHeadMessage(userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);
}

RESULT SandboxApp::SendUpdateHandMessage(long userID, hand::HandState handState) {
	return m_pCloudController->SendUpdateHandMessage(userID, handState);
}


RESULT SandboxApp::BroadcastDataMessage(Message *pDataMessage) {
	return m_pCloudController->BroadcastDataMessage(pDataMessage);
}

RESULT SandboxApp::BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	return m_pCloudController->BroadcastUpdateHeadMessage(ptPosition, qOrientation, vVelocity, qAngularVelocity);
}

RESULT SandboxApp::BroadcastUpdateHandMessage(hand::HandState handState) {
	return m_pCloudController->BroadcastUpdateHandMessage(handState);
}

// TimeManager
RESULT SandboxApp::RegisterSubscriber(TimeEventType timeEvent, Subscriber<TimeEvent>* pTimeSubscriber) {
	RESULT r = R_PASS;

	CR(m_pTimeManager->RegisterSubscriber(timeEvent, pTimeSubscriber));

Error:
	return r;
}

// IO
RESULT SandboxApp::RegisterSubscriber(int keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	RESULT r = R_PASS;

	CR(m_pSenseKeyboard->RegisterSubscriber(keyEvent, pKeyboardSubscriber));

Error:
	return r;
}

RESULT SandboxApp::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	RESULT r = R_PASS;

	CR(m_pSenseMouse->RegisterSubscriber(mouseEvent, pMouseSubscriber));

Error:
	return r;
}