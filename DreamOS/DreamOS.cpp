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

	// TODO: Initialize the sandbox

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");

Error:
	return r;
}

RESULT DreamOS::Start() {
	RESULT r = R_PASS;

	DEBUG_LINEOUT("DREAM OS %s Starting ...", m_versionDreamOS.GetString());

	// This will start the application
	CRM(m_pSandbox->ShowSandbox(), "Failed to show sandbox window");

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
