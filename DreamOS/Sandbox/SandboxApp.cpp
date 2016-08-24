#include "SandboxApp.h"
#include "Cloud/CloudController.h"

SandboxApp::SandboxApp() :
	m_pPathManager(nullptr),
	m_pCommandLineManager(nullptr),
	m_pOpenGLRenderingContext(NULL),
	m_pSceneGraph(NULL),
	m_pCloudController(nullptr),
	m_pHALImp(nullptr),
	m_fnUpdateCallback(nullptr)
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

inline PathManager* SandboxApp::GetPathManager() {
	return m_pPathManager; 
}

inline OpenGLRenderingContext * SandboxApp::GetOpenGLRenderingContext() {
	return m_pOpenGLRenderingContext; 
}

RESULT SandboxApp::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	// Set up command line manager
	m_pCommandLineManager = CommandLineManager::instance();
	CN(m_pCommandLineManager);
	
	//CommandLineManager *pCommandLineManager = CommandLineManager::instance();
	CR(m_pCommandLineManager->RegisterParameter("ip", "i", "ec2-54-175-210-194.compute-1.amazonaws.com"));
	//CR(m_pCommandLineManager->RegisterParameter("ip", "i", "localhost"));
	CR(m_pCommandLineManager->RegisterParameter("port", "P", "8000"));
	CR(m_pCommandLineManager->RegisterParameter("username", "u", "dream@dreamos.com"));
	CR(m_pCommandLineManager->RegisterParameter("password", "p", "dreamy"));

	CR(m_pCommandLineManager->InitializeFromCommandLine(argc, argv));

	// Set up Scene Graph
	m_pSceneGraph = new SceneGraph();
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");

	CRM(InitializeHAL(), "Failed to initialize HAL");

	CRM(InitializeCloudController(), "Failed to initialize cloud controller");

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");

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

volume* SandboxApp::MakeVolume(double width, double length, double height) {
	return m_pHALImp->MakeVolume(width, length, height);
}

volume* SandboxApp::MakeVolume(double side) {
	return m_pHALImp->MakeVolume(side);
}

volume* SandboxApp::AddVolume(double width, double length, double height) {
	RESULT r = R_PASS;

	volume *pVolume = MakeVolume(width, length, height);
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

volume* SandboxApp::AddVolume(double side) {
	return AddVolume(side, side, side);
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

composite* SandboxApp::AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) {
	return m_pHALImp->LoadModel(m_pSceneGraph, wstrOBJFilename, pTexture, ptPosition, scale, rotateY);
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
	return m_pHALImp->GetCamera()->GetOrientation();
}

// TODO: This should move up to Sandbox
hand *SandboxApp::GetHand(hand::HAND_TYPE handType) {
	return nullptr;
}

// Cloud Controller
RESULT SandboxApp::RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback) {
	return m_pCloudController->RegisterHeadUpdateMessageCallback(fnHandleHeadUpdateMessageCallback);
}

RESULT SandboxApp::RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback) {
	return m_pCloudController->RegisterHandUpdateMessageCallback(fnHandleHandUpdateMessageCallback);
}

RESULT SandboxApp::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	return m_pCloudController->SendUpdateHeadMessage(userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);
}

RESULT SandboxApp::SendUpdateHandMessage(long userID, hand::HandState handState) {
	return m_pCloudController->SendUpdateHandMessage(userID, handState);
}