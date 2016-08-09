#include "DreamOS.h"

DreamOS::DreamOS() :
	m_versionDreamOS(DREAM_OS_VERSION_MAJOR, DREAM_OS_VERSION_MINOR, DREAM_OS_VERSION_MINOR_MINOR),
	m_pSandbox(nullptr)
{
	RESULT r = R_PASS;

//Success:
	Validate();
	return;

//Error:
//	Invalidate();
//	return;
}

DreamOS::~DreamOS() {
	// empty
}

// This will construct and initialize all of the parts of the DreamOS client
RESULT DreamOS::Initialize(int argc, const char *argv[]) {
	RESULT r = R_PASS;

	srand(static_cast <unsigned> (time(0)));

	// Create the Sandbox
	m_pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM);
	CNM(m_pSandbox, "Failed to create sandbox");
	CVM(m_pSandbox, "Sandbox is Invalid!");

	// Initialize the sandbox
	CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");

	// Register the update callback
	CRM(RegisterUpdateCallback(std::bind(&DreamOS::Update, this)), "Failed to register DreamOS update callback");

Error:
	return r;
}

RESULT DreamOS::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DREAM OS %s Starting ...", m_versionDreamOS.GetString().c_str());

	// This will start the application
	CRM(m_pSandbox->Show(), "Failed to show sandbox window");

Error:
	return r;
}

RESULT DreamOS::Exit(RESULT exitcode) {
	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", m_versionDreamOS.GetString().c_str(), exitcode);
	return exitcode;
}


light* DreamOS::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->AddLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

sphere* DreamOS::AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pSandbox->AddSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

quad *DreamOS::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight) {
	return m_pSandbox->AddQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight);
}

volume* DreamOS::AddVolume(double side) {
	return m_pSandbox->AddVolume(side);
}

texture* DreamOS::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pSandbox->MakeTexture(pszFilename, type);
}

skybox *DreamOS::AddSkybox() {
	return m_pSandbox->AddSkybox();
}

model *DreamOS::AddModel(wchar_t *pszModelName) {
	return m_pSandbox->AddModel(pszModelName);
}

RESULT DreamOS::AddModel(const std::wstring& strRootFolder, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, point_precision rotateY) {
	return m_pSandbox->AddModel(strRootFolder, wstrOBJFilename, pTexture, ptPosition, scale, rotateY);
}

RESULT DreamOS::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	return m_pSandbox->RegisterUpdateCallback(fnUpdateCallback);
}

RESULT DreamOS::UnregisterUpdateCallback() {
	return m_pSandbox->UnregisterUpdateCallback();
}
