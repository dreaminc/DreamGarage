#include "DreamOS.h"

#include "Logger/Logger.h"

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

	// Initialize logger
	Logger::InitializeLogger();

	// Create the Sandbox
	m_pSandbox = SandboxFactory::MakeSandbox(CORE_CONFIG_SANDBOX_PLATFORM);
	CNM(m_pSandbox, "Failed to create sandbox");
	CVM(m_pSandbox, "Sandbox is Invalid!");

	// Check if Dream is launching from a web browser url.
	// a url command from a webpage, to trigger the launch of Dream, woud start with 'dreamos:run' command line.
	// The following code splites the whitespaces of a single command line param in that case, into a list of commad line arguments.
	if ((argc > 1) && 
		((std::string(argv[1]).substr(0, 11).compare("dreamos:run") == 0) ||
		 (std::string(argv[1]).substr(0, 14).compare("dreamosdev:run") == 0))) {
		//  Dream is launching from a webpage
		
		LOG(INFO) << "Dream runs from a webpage ";

		// decide if to split args or not
		if ((std::string(argv[1]).compare("dreamos:run") != 0) &&
			(std::string(argv[1]).compare("dreamosdev:run") != 0)) {
			std::vector<std::string> args{ argv[0] };
			int new_argc = 1;

			std::string cmdln = std::string(argv[1]); // The .exe location is the first argument

			std::string arg;
			std::stringstream ss(cmdln);

			while (ss >> arg)
			{
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
		else {
			CRM(m_pSandbox->Initialize(argc, argv), "Failed to initialize Sandbox");
		}
	}
	else
	{
		LOG(INFO) << "Dream runs from exe";

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
	CR(m_pSandbox->RunAppLoop());

Error:
	return r;
}

RESULT DreamOS::Exit(RESULT exitcode) {
	DEBUG_LINEOUT("DREAM OS %s Exiting with 0x%x result", m_versionDreamOS.GetString().c_str(), exitcode);
	return exitcode;
}

RESULT DreamOS::GetMouseRay(ray &rCast, double t) {
	return m_pSandbox->GetMouseRay(rCast, t);
}

RESULT DreamOS::SetHALConfiguration(HALImp::HALConfiguration halconf) {
	return m_pSandbox->SetHALConfiguration(halconf);
}

const HALImp::HALConfiguration& DreamOS::GetHALConfiguration() {
	return m_pSandbox->GetHALConfiguration();
}

CloudController *DreamOS::GetCloudController() {
	return m_pSandbox->m_pCloudController;
}

// This is a pass-thru at the moment
RESULT DreamOS::AddPhysicsObject(VirtualObj *pObject) {
	return m_pSandbox->AddPhysicsObject(pObject);
}

// This is a pass-thru at the moment
RESULT DreamOS::AddInteractionObject(VirtualObj *pObject) {
	return m_pSandbox->AddInteractionObject(pObject);
}

RESULT DreamOS::UpdateInteractionPrimitive(const ray &rCast) {
	return m_pSandbox->UpdateInteractionPrimitive(rCast);
}

RESULT DreamOS::SetGravityAcceleration(double acceleration) {
	return m_pSandbox->SetGravityAcceleration(acceleration);
}

RESULT DreamOS::SetGravityState(bool fEnabled) {
	return m_pSandbox->SetGravityState(fEnabled);
}

RESULT DreamOS::RemoveAllObjects() {
	return m_pSandbox->RemoveAllObjects();
}

light* DreamOS::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->AddLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

light* DreamOS::MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	return m_pSandbox->MakeLight(type, intensity, ptOrigin, colorDiffuse, colorSpecular, vectorDirection);
}

FlatContext* DreamOS::AddFlatContext(int width, int height, int channels)
{
	return m_pSandbox->AddFlatContext(width, height, channels);
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

volume* DreamOS::AddVolume(double side, bool fTriangleBased) {
	return m_pSandbox->AddVolume(side, fTriangleBased);
}

volume* DreamOS::AddVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pSandbox->AddVolume(width, length, height, fTriangleBased);
}

cylinder* DreamOS::AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions) {
	return m_pSandbox->AddCylinder(radius, height, numAngularDivisions, numVerticalDivisions);
}

DimRay* DreamOS::AddRay(point ptOrigin, vector vDirection, float step, bool fDirectional) {
	return m_pSandbox->AddRay(ptOrigin, vDirection, step, fDirectional);
}

volume* DreamOS::MakeVolume(double width, double length, double height, bool fTriangleBased) {
	return m_pSandbox->MakeVolume(width, length, height, fTriangleBased);
}

volume* DreamOS::MakeVolume(double side, bool fTriangleBased) {
	return m_pSandbox->AddVolume(side, fTriangleBased);
}
	
quad *DreamOS::AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal) {
	return m_pSandbox->AddQuad(width, height, numHorizontalDivisions, numVerticalDivisions, pTextureHeight, vNormal);
}

text* DreamOS::AddText(const std::wstring& fontName, const std::string& content, double size, bool isBillboard) {
	return m_pSandbox->AddText(fontName, content, size, isBillboard);
}

texture* DreamOS::MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) {
	return m_pSandbox->MakeTexture(pszFilename, type);
}

texture* DreamOS::MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n) {
	return m_pSandbox->MakeTexture(type, width, height, format, channels, pBuffer, pBuffer_n);
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

composite *DreamOS::AddComposite() {
	return m_pSandbox->AddComposite();
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

// Physics Engine
RESULT DreamOS::RegisterObjectCollision(VirtualObj *pVirtualObject) {
	RESULT r = R_PASS;

	r = m_pSandbox->RegisterObjectAndSubscriber(pVirtualObject, this);
	CR(r);

Error:
	return r;
}

RESULT DreamOS::RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber) {
	RESULT r = R_PASS;

	r = m_pSandbox->RegisterEventSubscriber(eventType, pInteractionSubscriber);
	CR(r);

Error:
	return r;
}

// Cloud Controller
RESULT DreamOS::RegisterPeersUpdateCallback(HandlePeersUpdateCallback fnHandlePeersUpdateCallback) {
	return m_pSandbox->RegisterPeersUpdateCallback(fnHandlePeersUpdateCallback);
}

RESULT DreamOS::RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback) {
	return m_pSandbox->RegisterDataMessageCallback(fnHandleDataMessageCallback);
}

RESULT DreamOS::RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback) {
	return m_pSandbox->RegisterHeadUpdateMessageCallback(fnHandleHeadUpdateMessageCallback);
}

RESULT DreamOS::RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback) {
	return m_pSandbox->RegisterHandUpdateMessageCallback(fnHandleHandUpdateMessageCallback);
}

RESULT DreamOS::RegisterAudioDataCallback(HandleAudioDataCallback fnHandleAudioDataCallback) {
	return m_pSandbox->RegisterAudioDataCallback(fnHandleAudioDataCallback);
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

RESULT DreamOS::BroadcastDataMessage(Message *pDataMessage) {
	return m_pSandbox->BroadcastDataMessage(pDataMessage);
}

RESULT DreamOS::BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity, quaternion qAngularVelocity) {
	return m_pSandbox->BroadcastUpdateHeadMessage(ptPosition, qOrientation, vVelocity, qAngularVelocity);
}

RESULT DreamOS::BroadcastUpdateHandMessage(hand::HandState handState) {
	return m_pSandbox->BroadcastUpdateHandMessage(handState);
}

RESULT DreamOS::RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber) {
	return m_pSandbox->RegisterSubscriber(keyEvent, pKeyboardSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber) {
	return m_pSandbox->RegisterSubscriber(typingEvent, pTypingSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber) {
	return m_pSandbox->RegisterSubscriber(mouseEvent, pMouseSubscriber);
}

RESULT DreamOS::RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber) {
	return m_pSandbox->RegisterSubscriber(controllerEvent, pControllerSubscriber);
}

long DreamOS::GetTickCount() {
	return m_pSandbox->GetTickCount();
}