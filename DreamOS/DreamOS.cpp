#include "DreamOS.h"

DreamOS::DreamOS() :
	m_versionDreamOS(DREAM_OS_VERSION_MAJOR, DREAM_OS_VERSION_MINOR, DREAM_OS_VERSION_MINOR_MINOR),
	m_pSandbox(nullptr)
{
	RESULT r = R_PASS;

Success:
	Validate();
	return;

Error:
	Invalidate();
	return;
}

DreamOS::~DreamOS() {
	// empty
}

// This will construct and initialize all of the parts of the DreamOS client
RESULT DreamOS::Initialize() {
	RESULT r = R_PASS;

	srand(static_cast <unsigned> (time(0)));

	// Create the Sandbox
	m_pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM);
	CNM(m_pSandbox, "Failed to create sandbox");
	CVM(m_pSandbox, "Sandbox is Invalid!");

	// Initialize the sandbox
	CRM(m_pSandbox->Initialize(), "Failed to initialize Sandbox");

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");

	// Register the update callback
	CRM(RegisterUpdateCallback(std::bind(&DreamOS::Update, this)), "Failed to register DreamOS update callback");

Error:
	return r;
}

RESULT DreamOS::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DREAM OS %s Starting ...", m_versionDreamOS.GetString());

	// This will start the application
	CRM(m_pSandbox->Show(), "Failed to show sandbox window");

Error:
	return r;
}

RESULT DreamOS::Exit(RESULT exitcode) {
	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", m_versionDreamOS.GetString(), exitcode);
	return exitcode;
}


light* DreamOS::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->AddLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

light* DreamOS::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

sphere* DreamOS::AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	return m_pSandbox->AddSphere(radius, numAngularDivisions, numVerticalDivisions);
}

sphere* DreamOS::MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) {
	return m_pSandbox->MakeSphere(radius, numAngularDivisions, numVerticalDivisions);
}

volume* DreamOS::AddVolume(double width, double length, double height) {
	return m_pSandbox->AddVolume(width, length, height);
}

volume* DreamOS::MakeVolume(double width, double length, double height) {
	return m_pSandbox->MakeVolume(width, length, height);
}

volume* DreamOS::AddVolume(double side) {
	return m_pSandbox->AddVolume(side);
}

volume* DreamOS::MakeVolume(double side) {
	return m_pSandbox->MakeVolume(side);
}

texture* DreamOS::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pSandbox->MakeTexture(pszFilename, type);
}

skybox *DreamOS::AddSkybox() {
	return m_pSandbox->AddSkybox();
}

skybox *DreamOS::MakeSkybox() {
	return m_pSandbox->MakeSkybox();
}

model *DreamOS::AddModel(wchar_t *pszModelName) {
	return m_pSandbox->AddModel(pszModelName);
}

model *DreamOS::MakeModel(wchar_t *pszModelName) {
	return m_pSandbox->AddModel(pszModelName);
}

RESULT DreamOS::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	return m_pSandbox->RegisterUpdateCallback(fnUpdateCallback);
}

RESULT DreamOS::UnregisterUpdateCallback() {
	return m_pSandbox->UnregisterUpdateCallback();
}
