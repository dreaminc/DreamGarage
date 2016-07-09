#include "SandboxApp.h"

SandboxApp::SandboxApp() :
	m_pPathManager(NULL),
	m_pOpenGLRenderingContext(NULL),
	m_pSceneGraph(NULL),
	m_pCloudController(nullptr),
	m_pHALImp(nullptr),
	m_fnUpdateCallback(nullptr)
{
	// empty
}

SandboxApp::~SandboxApp() {
	// empty stub
}

inline PathManager * SandboxApp::GetPathManager() {
	return m_pPathManager; 
}

inline OpenGLRenderingContext * SandboxApp::GetOpenGLRenderingContext() {
	return m_pOpenGLRenderingContext; 
}

RESULT SandboxApp::Initialize() {
	RESULT r = R_PASS;

	m_pSceneGraph = new SceneGraph();
	CNM(m_pSceneGraph, "Failed to allocate Scene Graph");

	CRM(InitializeHAL(), "Failed to initialize HAL");

#ifdef CEF_ENABLED
	CRM(InitializeCloudController(), "Failed to initialize cloud controller");
#endif

	// TODO: Show this be replaced with individual initialization of each component?
	CRM(InitializeSandbox(), "Failed to initialize sandbox");

Error:
	return r;
}

// Sandbox Factory Methods
// TODO: This should all go up into the sandbox
RESULT SandboxApp::AddObject(VirtualObj *pObject) {
	RESULT r = R_PASS;

	CR(m_pSceneGraph->PushObject(pObject));

Error:
	return r;
}

light* SandboxApp::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	RESULT r = R_PASS;

	light *pLight = m_pHALImp->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
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

Success:
	return pQuad;

Error:
	if (pQuad != nullptr) {
		delete pQuad;
		pQuad = nullptr;
	}

	return nullptr;
}

sphere* SandboxApp::AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	RESULT r = R_PASS;

	sphere *pSphere = m_pHALImp->MakeSphere(radius, numAngularDivisions, numVerticalDivisions);
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

volume* SandboxApp::AddVolume(double side) {
	RESULT r = R_PASS;

	volume *pVolume = m_pHALImp->MakeVolume(side);
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

texture* SandboxApp::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pHALImp->MakeTexture(pszFilename, type);
}

skybox *SandboxApp::AddSkybox() {
	RESULT r = R_PASS;

	skybox *pSkybox = m_pHALImp->MakeSkybox();
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

// TODO: A lot of this should go into the model object itself
model *SandboxApp::AddModel(wchar_t *pszModelName) {
	RESULT r = R_PASS;

	model* pModel = m_pHALImp->MakeModel(pszModelName);
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

// TODO: Fix this
RESULT SandboxApp::AddModel(const std::wstring& strRootFolder, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) {
	return m_pHALImp->LoadModel(m_pSceneGraph, strRootFolder, wstrOBJFilename, pTexture, ptPosition, scale, rotateY);
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