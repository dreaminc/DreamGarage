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

	// Check if Dream is launching from a web browser url.
	// a url command from a webpage, to trigger the launch of Dream, woud start with 'dreamos:run' command line.
	// The following code splites the whitespaces of a single command line param in that case, into a list of commad line arguments.
	if ((argc > 1) && (std::string(argv[1]).substr(0, 11).compare("dreamos:run") == 0)) {
		//  Dream is launching from a webpage

		std::vector<std::string> args{argv[0]};
		int new_argc = 1;

		std::string cmdln = std::string(argv[1]); // The .exe location is the first argument

		std::string arg;
		std::stringstream ss(cmdln);

		while (ss >> arg)
		{
			OutputDebugStringA(arg.c_str());
			args.push_back(arg);
			new_argc++;
		}

		char** new_argv = new char*[new_argc];

		for (int i = 0; i < new_argc; i++)
		{
			new_argv[i] = new char;
			new_argv[i] = (char*)args[i].c_str();
		}

		CRM(m_pSandbox->Initialize(new_argc, (const char**)new_argv), "Failed to initialize Sandbox");
	}
	else
	{
		// Initialize the sandbox
		CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");
	}

	// Load the scene
	CRM(LoadScene(), "Failed to load scene");
	
	// Register the update callback
	CRM(RegisterUpdateCallback(std::bind(&DreamOS::Update, this)), "Failed to register DreamOS update callback");

Error:
	return r;
}

camera* DreamOS::GetCamera() {
	return m_pSandbox->GetCamera();
}

point DreamOS::GetCameraPosition() {
	return m_pSandbox->GetCameraPosition();
}

hand *DreamOS::GetHand(hand::HAND_TYPE handType) {
	return m_pSandbox->GetHand(handType);
}

quaternion DreamOS::GetCameraOrientation() {
	return m_pSandbox->GetCameraOrientation();
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

light* DreamOS::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

FlatContext* DreamOS::AddFlatContext()
{
	return m_pSandbox->AddFlatContext();
}

RESULT DreamOS::RenderToTexture(FlatContext *pContext) 
{
	return m_pSandbox->RenderToTexture(pContext);
}

sphere* DreamOS::AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pSandbox->AddSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

sphere* DreamOS::MakeSphere(float radius, int numAngularDivisions, int numVerticalDivisions, color c) {
	return m_pSandbox->MakeSphere(radius, numAngularDivisions, numVerticalDivisions, c);
}

volume* DreamOS::AddVolume(double side) {
	return m_pSandbox->AddVolume(side);
}

volume* DreamOS::AddVolume(double width, double length, double height) {
	return m_pSandbox->AddVolume(width, length, height);
}

volume* DreamOS::MakeVolume(double width, double length, double height) {
	return m_pSandbox->MakeVolume(width, length, height);
}

volume* DreamOS::MakeVolume(double side) {
	return m_pSandbox->AddVolume(side);
}
	
quad *DreamOS::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight) {
	return m_pSandbox->AddQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight);
}

text* DreamOS::AddText(const std::wstring& fontName, const std::string& content, double size, bool isBillboard)
{
	return m_pSandbox->AddText(fontName, content, size, isBillboard);
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
	
composite *DreamOS::AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale, vector vEulerRotation) {
	return m_pSandbox->AddModel(wstrOBJFilename, pTexture, ptPosition, scale, vEulerRotation);
}

user *DreamOS::AddUser() {
	return m_pSandbox->AddUser();
}

RESULT DreamOS::RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback) {
	return m_pSandbox->RegisterUpdateCallback(fnUpdateCallback);
}

RESULT DreamOS::UnregisterUpdateCallback() {
	return m_pSandbox->UnregisterUpdateCallback();
}

// Cloud Controller
RESULT DreamOS::RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback) {
	return m_pSandbox->RegisterDataMessageCallback(fnHandleDataMessageCallback);
}

RESULT DreamOS::RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback) {
	return m_pSandbox->RegisterHeadUpdateMessageCallback(fnHandleHeadUpdateMessageCallback);
}

RESULT DreamOS::RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback) {
	return m_pSandbox->RegisterHandUpdateMessageCallback(fnHandleHandUpdateMessageCallback);
}

RESULT DreamOS::SendDataMessage(long userID, Message *pDataMessage) {
	return m_pSandbox->SendDataMessage(userID, pDataMessage);
}

RESULT DreamOS::SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	return m_pSandbox->SendUpdateHeadMessage(userID, ptPosition, qOrientation, vVelocity, qAngularVelocity);
}

RESULT DreamOS::SendUpdateHandMessage(long userID, hand::HandState handState) {
	return m_pSandbox->SendUpdateHandMessage(userID, handState);
}

RESULT DreamOS::RegisterSubscriber(int keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	return m_pSandbox->RegisterSubscriber(keyEvent, pKeyboardSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	return m_pSandbox->RegisterSubscriber(mouseEvent, pMouseSubscriber);
}

long DreamOS::GetTickCount() {
	return m_pSandbox->GetTickCount();
}