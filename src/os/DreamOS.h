#ifndef DREAM_OS_H_
#define DREAM_OS_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/DreamOS.h
// The DreamOS base class which a DreamOS "application" is derived from
// In the current context of use, this is referring to the application layer of the DreamOS
// client rather than a DreamOS app which will later be rolled out such that applications
// can be run within a client, making a transition to treating Dream as true operating system.
// In the short term, however, DreamOS will run in the context of an application on a given platform
// such as Android or Windows.

#include "core/types/UID.h"
#include "core/types/valid.h"
#include "core/types/version.h"

#include "sandbox/SandboxFactory.h"

#define DREAM_OS_VERSION_MAJOR 0
#define DREAM_OS_VERSION_MINOR 1
#define DREAM_OS_VERSION_MINOR_MINOR 0

#include "core/primitives/light.h"
#include "core/primitives/quad.h"
#include "core/primitives/FlatContext.h"
#include "core/primitives/sphere.h"
#include "core/primitives/cylinder.h"
#include "core/primitives/volume.h"
#include "core/primitives/texture.h"
#include "core/primitives/skybox.h"
#include "core/primitives/billboard.h"

#include "core/user/user.h"
#include "core/text/text.h"
#include "core/dimension/DimRay.h"
#include "core/dimension/DimPlane.h"

#include "core/model/ModelFactory.h"

#include "core/hysteresis/HysteresisObject.h"

#include "module/DreamObjectModule.h"

#include "modules/PhysicsEngine/PhysicsEngine.h"
#include "modules/DreamSoundSystem/DreamSoundSystem.h"

// TODO: Remove DreamAppHandles
#include "app/DreamAppHandle.h"

// TODO: This should be removed - move to a different pattern for observers 
#include "apps/DreamPeerApp/DreamPeerApp.h"

//#include "DreamLogger/DreamLogger.h"

// Dream Modules

#include "UI/UIKeyboard.h"

class UIKeyboardLayout;
class DreamMessage;
class DreamAppMessage;

// Apps
// TODO: None of these should really be in here
class DreamPeerApp;
class DreamUserApp;
class DreamShareViewApp;
class DreamSettingsApp;
class DreamLoginApp;
class DreamFormApp;

class OGLProgram;

class NamedPipeClient;
class NamedPipeServer;

class PeerStayAliveMessage;
class PeerAckMessage;
class PeerHandshakeMessage;

class DreamAppManager;
class DreamModuleManager;

struct PrimParams;

#include "DreamVideoStreamSubscriber.h"

class DOSObserver {
public:
	virtual RESULT HandleDOSMessage(std::string& strMessage) = 0;
};

class Windows64Observer {
public:
	virtual RESULT HandleWindows64CopyData(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth) = 0;
};

class DreamOS :
	public Subscriber<CollisionObjectEvent>,
	public valid,
	public CloudController::PeerConnectionObserver,
	public CloudController::EnvironmentObserver,
	public CloudController::UserObserver,
	public DreamPeerApp::DreamPeerAppObserver,
	public DreamSoundSystem::observer
{
	friend class CloudTestSuite;
	friend class DreamAppBase;

	// TODO: this needs to be revisited
	friend class DreamTestSuite;
	friend class UIModule;
	friend class HALTestSuite;
	friend class PipelineTestSuite;
	friend class UITestSuite;
	friend class InteractionEngineTestSuite;
	friend class PhysicsEngineTestSuite;
	friend class UIViewTestSuite;
	friend class AnimationTestSuite;
	friend class DreamOSTestSuite;
	friend class CollisionTestSuite;
	friend class WebRTCTestSuite;
	friend class SoundTestSuite;
	friend class SandboxTestSuite;
	friend class MultiContentTestSuite;
	friend class DimensionTestSuite;

	friend class ModelFactory;

public:
	DreamVideoStreamSubscriber* m_pVideoStreamSubscriber = nullptr;
	PeerConnection *m_pVideoStreamPeerConnectionSource = nullptr;

	RESULT RegisterVideoStreamSubscriber(PeerConnection *pVideoSteamPeerConnectionSource, DreamVideoStreamSubscriber *pVideoStreamSubscriber);
	RESULT UnregisterVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber);
	bool IsRegisteredVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber);

	DreamVideoStreamSubscriber* m_pCameraVideoStreamSubscriber = nullptr;
	PeerConnection *m_pCameraVideoStreamPeerConnectionSource = nullptr;

	RESULT RegisterCameraVideoStreamSubscriber(PeerConnection *pVideoSteamPeerConnectionSource, DreamVideoStreamSubscriber *pVideoStreamSubscriber);
	RESULT UnregisterCameraVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber);
	bool IsRegisteredCameraVideoStreamSubscriber(DreamVideoStreamSubscriber *pVideoStreamSubscriber);

public:
	// Log (pass through to Dream logger)
	template <typename... Args>
	inline RESULT Log(DreamLogger::Level logLevel, const char* pszMessage, const Args&... args) {
		return DreamLogger::instance()->Log(logLevel, pszMessage, args...);
	}

	// Console
	// TODO:

public:
	DreamOS();
	~DreamOS();

	RESULT Initialize(int argc = 0, const char *argv[] = nullptr);
	RESULT Start();
	virtual RESULT Exit(RESULT exitcode);

	// These are used to set paths for path manager
	virtual bool UseInstallPath() { return false; }
	virtual std::wstring GetDreamFolderPath() { return std::wstring(L""); }
	virtual version GetDreamVersion() = 0;

	virtual RESULT ConfigureSandbox() { return R_NOT_IMPLEMENTED; }
	virtual RESULT LoadScene() = 0;
	virtual RESULT DidFinishLoading() { return R_NOT_IMPLEMENTED; }
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) { return R_NOT_IMPLEMENTED; }
	virtual RESULT Update(void) = 0;

	RESULT GetMouseRay(ray &rCast, double t = 0.0f);

	HMD *GetHMD();
	HWND GetDreamHWND();

	RESULT RecenterHMD();

	// PeerConnectionObserver
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnNewSocketConnection(int seatPosition) = 0;
	virtual RESULT OnPeerConnectionClosed(PeerConnection *pPeerConnection) override;
	virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDreamMessage) override;
	virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	virtual RESULT OnAudioData(const std::string &strAudioTrackLabel, PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	virtual RESULT OnVideoFrame(const std::string &strVideoTrackLabel, PeerConnection* pPeerConnection, uint8_t *pVideoFrameDataBuffer, int pxWidth, int pxHeight);
	virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) override;
	virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) override;

	// EnvironmentObserver
	// TODO: This should be encapsulated in it's own object
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnReceiveAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopSending(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopReceiving(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnShareAsset(std::shared_ptr<EnvironmentShare> pEnvironmentShare) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnCloseAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	// Environment observer virtual camera
	virtual RESULT OnOpenCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnCloseCamera(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnSendCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopSendingCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnReceiveCameraPlacement(long userID) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnStopReceivingCameraPlacement() override {
		return R_NOT_IMPLEMENTED;
	}

	// Environment observer forms
	virtual RESULT OnGetForm(std::string& strKey, std::string& strTitle, std::string& strURL) override {
		return R_NOT_IMPLEMENTED;
	}

	//User Observer
	virtual RESULT OnDreamVersion(version dreamVersion) {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnAPIConnectionCheck(bool fIsConnected) {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnGetSettings(point ptPosition, quaternion qOrientation, bool fIsSet) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnSetSettings() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnLogin() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnLogout() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnPendLogout() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnSwitchTeams() override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnFormURL(std::string& strKey, std::string& strTitle, std::string& strURL) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnAccessToken(bool fSuccess, std::string& strAccessToken) override {
		return R_NOT_IMPLEMENTED;
	}

	virtual RESULT OnGetTeam(bool fSuccess, int environmentId, int environmentModelId) override {
		return R_NOT_IMPLEMENTED;
	}

	// DreamPeer Observer
	virtual RESULT OnDreamPeerStateChange(DreamPeerApp* pDreamPeer) override;

	// DOSObserver
	RESULT RegisterDOSObserver(DOSObserver *pDOSObserver);
	RESULT UnregisterDOSObserver(DOSObserver *pDOSObserver);
	RESULT SendDOSMessage(std::string& strMessage);

	// Windows64Observer
	RESULT RegisterWindows64Observer(Windows64Observer *pWindows64Observer);
	RESULT UnregisterWindows64Observer(Windows64Observer *pWindows64Observer);
	RESULT OnDesktopFrame(unsigned long messageSize, void* pMessageData, int pxHeight, int pxWidth);

private:
	DOSObserver * m_pDOSObserver = nullptr;
	Windows64Observer *m_pWindows64Observer = nullptr;

public:
	// Cloud Controller Hooks
	virtual RESULT OnNewDreamPeer(DreamPeerApp *pDreamPeer) = 0;
	virtual RESULT OnDreamPeerConnectionClosed(std::shared_ptr<DreamPeerApp> pDreamPeer) = 0;
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) = 0;

	// Handle the Dream App Messages
	RESULT OnDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

	// Peers
	RESULT HandlePeerHandshakeMessage(PeerConnection* pPeerConnection, PeerHandshakeMessage *pPeerHandshakeMessage);
	RESULT HandlePeerStayAliveMessage(PeerConnection* pPeerConnection, PeerStayAliveMessage *pPeerStayAliveMessage);
	RESULT HandlePeerAckMessage(PeerConnection* pPeerConnection, PeerAckMessage *pPeerAckMessage);

	WebRTCPeerConnectionProxy *GetWebRTCPeerConnectionProxy(PeerConnection* pPeerConnection);

protected:
	std::shared_ptr<DreamPeerApp> CreateNewPeer(PeerConnection *pPeerConnection);
	std::shared_ptr<DreamPeerApp> FindPeer(long peerUserID);
	std::shared_ptr<DreamPeerApp> FindPeer(PeerConnection *pPeerConnection);
	RESULT RemovePeer(long peerUserID);
	RESULT RemovePeer(std::shared_ptr<DreamPeerApp> pDreamPeer);
	RESULT ClearPeers();

	bool HasPeerApps();

	DreamPeerApp::state GetPeerState(long peerUserID);


	RESULT CheckDreamPeerAppStates();

public:
	virtual CameraNode *GetAuxCameraNode() {
		return nullptr;
	}

	virtual bool IsCameraInUse() {
		return false;
	}

	RESULT UpdateAllPeerLabelOrientations(camera *pCamera);

private:
	std::map<long, std::shared_ptr<DreamPeerApp>> m_dreamPeerApps;

public:
	InteractionEngineProxy *GetInteractionEngineProxy();
	CloudController *GetCloudController();
	std::shared_ptr<DreamSoundSystem> GetDreamSoundSystem();
	long GetUserID();
	long GetUserAvatarID();

protected:
	RESULT SetHALConfiguration(HALImp::HALConfiguration halconf);
	const HALImp::HALConfiguration& GetHALConfiguration();

	// TODO: This is here temporarily, should be replaced by proper sandbox
	// related functionality
	HALImp* GetHALImp();

	// Dream Apps
public:
	ControllerProxy* GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE controllerType);

	template<class derivedAppType>
	std::shared_ptr<derivedAppType> LaunchDreamApp(void *pContext, bool fAddToScene = true);

	template<class derivedAppType>
	RESULT ShutdownDreamApp(std::shared_ptr<derivedAppType> pDreamApp);

	DreamAppHandle* RequestCaptureAppUnique(std::string strAppName, DreamAppBase* pHoldingApp);
	DreamAppHandle* CaptureApp(UID uid, DreamAppBase* pHoldingApp);

	RESULT ReleaseApp(DreamAppHandle* pHandle, UID appUID, DreamAppBase* pHoldingApp);
	RESULT RequestReleaseAppUnique(DreamAppHandle* pHandle, DreamAppBase* pHoldingApp);

	std::vector<UID> GetAppUID(std::string strAppName);
	UID GetUniqueAppUID(std::string strAppName);

	std::shared_ptr<DreamAppBase> GetDreamAppFromUID(UID appUID);

	virtual RESULT MakePipeline(CameraNode* pCamera, OGLProgram* &pRenderNode, OGLProgram* &pEndNode, Sandbox::PipelineType pipelineType) = 0;

	//template<class derivedAppType>
	//RESULT ReleaseApp(DreamAppHandleBase* pAppHandle, DreamAppBase* pHoldingApp);

	//std::map<DreamAppHandleBase*, std::vector<DreamAppBase*>> m_capturedApps;

	// Dream Modules
public:
	template<class derivedModuleType>
	std::shared_ptr<derivedModuleType> LaunchDreamModule(void *pContext);

	template<class derivedModuleType>
	RESULT ShutdownDreamModule(std::shared_ptr<derivedModuleType> pDreamModule);

	std::vector<UID> GetModuleUID(std::string strName);
	UID GetUniqueModuleUID(std::string strName);

	std::shared_ptr<DreamModuleBase> GetDreamModuleFromUID(UID moduleUID);

public:
	// Keyboard
	RESULT InitializeKeyboard();
	RESULT InitializeDreamUserApp();
	RESULT InitializeCloudController();

	// Physics
	RESULT AddPhysicsObject(VirtualObj *pObject);
	RESULT SetGravityAcceleration(double acceleration);
	RESULT SetGravityState(bool fEnabled);

	// Sandbox level objects
	std::shared_ptr<NamedPipeClient> MakeNamedPipeClient(std::wstring strPipename);
	std::shared_ptr<NamedPipeServer> MakeNamedPipeServer(std::wstring strPipename);
	
	RESULT AddObject(VirtualObj *pObject, Sandbox::PipelineType pipelineType = (Sandbox::PipelineType::MAIN | Sandbox::PipelineType::AUX));
	RESULT AddInteractionObject(VirtualObj *pObject);
	RESULT AddObjectToInteractionGraph(VirtualObj *pObject);
	RESULT RemoveObjectFromInteractionGraph(VirtualObj *pObject);
	RESULT AddAndRegisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	//RESULT UpdateInteractionPrimitive(const ray &rCast);

	RESULT AddObjectToUIGraph(VirtualObj *pObject, Sandbox::PipelineType pipelineType = Sandbox::PipelineType::MAIN);
	RESULT AddObjectToUIClippingGraph(VirtualObj *pObject);

	RESULT RemoveObjectFromUIGraph(VirtualObj *pObject);
	RESULT RemoveObjectFromUIClippingGraph(VirtualObj *pObject);
	RESULT RemoveObjectFromAuxUIGraph(VirtualObj *pObject);

	RESULT RemoveObject(VirtualObj *pObject);
	RESULT RemoveAllObjects();

	light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	light *MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	FlatContext *AddFlatContext(int width = 1024, int height = 1024, int channels = 4);
	RESULT RenderToTexture(FlatContext* pContext);

	quad *AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	quad *MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());

	HysteresisObject *MakeHysteresisObject(float onThreshold, float offThreshold, HysteresisObjectType objectType);

	template<typename objType, typename... Targs>
	objType *Add(Targs... Fargs) {
		return m_pSandbox->TAddObject<objType>(Fargs...);
	}

	template<typename objType, typename... Targs>
	objType *Make(Targs... Fargs) {
		return m_pSandbox->TMakeObject<objType>(Fargs...);
	}

	template<typename objType>
	objType *Add() {
		return m_pSandbox->TAddObject<objType>();
	}

	template<typename objType>
	objType *Make() {
		return m_pSandbox->TMakeObject<objType>();
	}

	sphere *AddSphere(float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE));
	sphere *MakeSphere(float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE));

	text *AddText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);

	text *AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);

	text *AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, bool fBillboard = false);

	text *AddText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE);

	std::shared_ptr<font> MakeFont(std::wstring wstrFontFileName, bool fDistanceMap = false);

	volume *MakeVolume(double side, bool fTriangleBased = true);
	volume *MakeVolume(double width, double length, double height, bool fTriangleBased = true);

	volume* AddVolume(double side, bool fTriangleBased = true);
	volume *AddVolume(double width, double length, double height, bool fTriangleBased = true);

	cylinder* AddCylinder(double radius, double height, int numAngularDivisions = 3, int numVerticalDivisions = 3);

	DimRay* MakeRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	DimRay* AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	DimPlane* AddPlane(point ptOrigin = point(), vector vNormal = vector::jVector(1.0f));

	texture* MakeTexture(const texture &srcTexture);
	texture* MakeTexture(texture::type type, const wchar_t *pszFilename);
	texture* MakeTexture(texture::type type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n);
	texture *MakeTextureFromFileBuffer(texture::type type, uint8_t *pBuffer, size_t pBuffer_n);

	cubemap* MakeCubemap(const std::wstring &wstrCubemapName);

	skybox *AddSkybox();
	skybox *MakeSkybox();

	//model *AddModel(wchar_t *pszModelName);
	//model *MakeModel(wchar_t *pszModelName);

	model *MakeModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);
	model *AddModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);
	model *MakeModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags);
	model *AddModel(const std::wstring& wstrModelFilename, ModelFactory::flags modelFactoryFlags);

	composite *AddComposite();
	composite *MakeComposite();

	user *AddUser();
	user *MakeUser();

	billboard *AddBillboard(point ptOrigin, float width, float height);
	billboard *MakeBillboard(point ptOrigin, float width, float height);

	Pipeline *GetRenderPipeline();

	stereocamera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	CameraNode* GetCameraNode() { return m_pSandbox->GetCameraNode(); }
	ObjectStoreNode* GetSceneGraphNode() { return m_pSandbox->GetSceneGraphNode(); }
	ObjectStoreNode* GetAuxSceneGraphNode() { return m_pSandbox->GetAuxSceneGraphNode(); }
	ObjectStoreNode* GetUISceneGraphNode() { return m_pSandbox->GetUISceneGraphNode(); }
	ObjectStoreNode* GetUIClippingSceneGraphNode() { return m_pSandbox->GetUIClippingSceneGraphNode(); }
	ObjectStoreNode* GetAuxUISceneGraphNode() { return m_pSandbox->GetAuxUISceneGraphNode(); }
	ObjectStoreNode* GetBillboardSceneGraphNode() { return m_pSandbox->GetBillboardSceneGraphNode(); }

	// Hands
	hand *GetHand(HAND_TYPE handType);

	// Async Object 
	DimObj *MakeObject(PrimParams *pPrimParams, bool fInitialize = true);
	texture *MakeTexture(PrimParams *pPrimParams, bool fInitialize = true);
	RESULT InitializeObject(DimObj *pDimObj);
	RESULT InitializeTexture(texture *pTexture);

	RESULT MakeMesh(std::function<RESULT(DimObj*, void*)> fnOnObjectReady, const mesh::params &meshParams, void *pContext = nullptr);
	RESULT MakeModel(std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext = nullptr, const std::wstring& wstrModelFilename = L"", ModelFactory::flags modelFactoryFlags = ModelFactory::flags::NONE);
	RESULT MakeSphere(std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext = nullptr, float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE));
	RESULT MakeVolume(std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext = nullptr, double width = 1.0f, double length = 1.0f, double height = 1.0f, bool fTriangleBased = true);
	RESULT MakeQuad(std::function<RESULT(DimObj*, void*)> fnOnObjectReady, void *pContext = nullptr, double width = 1.0f, double height = 1.0f, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());
	RESULT LoadTexture(std::function<RESULT(texture*, void*)> fnOnTextureReady, void *pContext = nullptr, texture::type type = texture::type::TEXTURE_2D, const wchar_t *pszFilename = nullptr);


	// Shaders / Programs
	ProgramNode* MakeProgramNode(std::string strNodeName, PIPELINE_FLAGS optFlags = PIPELINE_FLAGS::NONE);

protected:
	long GetTickCount();

	// Physics Engine
protected:
	RESULT RegisterObjectCollision(VirtualObj *pVirtualObject);

	virtual RESULT Notify(CollisionObjectEvent *oEvent) { return R_PASS; }

public:
	RESULT RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT RegisterEventSubscriber(VirtualObj* pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject);
	RESULT UnregisterInteractionSubscriber(Subscriber<InteractionObjectEvent>* pInteractionSubscriber);

	// Cloud Controller
protected:
	RESULT RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver);
	RESULT RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver);
	RESULT RegisterUserObserver(CloudController::UserObserver *pUserObserver);

	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT BroadcastDataMessage(Message *pDataMessage);
	RESULT BroadcastVideoFrame(const std::string &strVideoTrackLabel, uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);

public:
	// Dream App Messaging
	RESULT BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage, DreamAppMessage::flags messageFlags = DreamAppMessage::flags::SHARE_NETWORK);

protected:
	// Sound 
	RESULT InitializeDreamSoundSystem();
	RESULT RegisterSoundSystemObserver(DreamSoundSystem::observer *pObserver);
	RESULT UnregisterSoundSystemObserver();

	// Object Module
	RESULT InitializeDreamObjectModule();

	// DreamSoundSystem::observer
	virtual RESULT OnAudioDataCaptured(int numFrames, SoundBuffer *pCaptureBuffer) override;

public:
	std::shared_ptr<SpatialSoundObject> AddSpatialSoundObject(point ptPosition, vector vEmitterDirection, vector vListenerDirection);
	std::shared_ptr<SoundFile> LoadSoundFile(const std::wstring &wstrFilename, SoundFile::type soundFileType);
	RESULT PlaySoundFile(std::shared_ptr<SoundFile> pSoundFile);
	RESULT LoopSoundFile(std::shared_ptr<SoundFile> pSoundFile);
	RESULT PushAudioPacketToMixdown(DreamSoundSystem::MIXDOWN_TARGET mixdownTarget, int numFrames, const AudioPacket &pendingAudioPacket);

	RESULT MuteDreamVCamAudio(bool fMute);

	// IO
//protected:
public:
	RESULT RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber);
	RESULT RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber);
	RESULT RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber);
	RESULT RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber);
	RESULT RegisterSubscriber(SenseGamepadEventType gamePadEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber);
	RESULT RegisterSubscriber(HMDEventType hmdEvent, Subscriber<HMDEvent>* pHMDEventSubscriber);

	RESULT UnregisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber);
	RESULT UnregisterSubscriber(SenseGamepadEventType gamePadEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber);

	RESULT SaveCredential(std::wstring wstrKey, std::string strValue, CredentialManager::type credType, bool fOverwrite);
	RESULT GetCredential(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType);
	RESULT RemoveCredential(std::wstring wstrKey, CredentialManager::type credType);

	virtual RESULT SaveCameraSettings(point ptPosition, quaternion qOrientation) = 0;

	bool IsSandboxInternetConnectionValid();

protected:
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

protected:
	RESULT SetSandboxConfiguration(Sandbox::configuration sandboxconf);

public:
	const Sandbox::configuration& GetSandboxConfiguration();
	std::wstring GetHardwareID();
	std::string GetHMDTypeString();

private:
	Sandbox *m_pSandbox = nullptr;

public:

	virtual bool IsSharing();
	virtual texture* GetSharedContentTexture();
	virtual texture* GetSharedContentPointerTexture();
	virtual texture* GetSharedCameraTexture();
	virtual RESULT SetSharedContentTexture(texture* pSharedTexture);
	virtual RESULT BroadcastSharedVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight);
	virtual RESULT BroadcastSharedAudioPacket(const AudioPacket &pendingAudioPacket);

	// TODO: This is temporary code for the mirror stage - 
	// in future should replace this with real functionality (that's generalized) 
	// or remove it entirely 
public:
	UIStageProgram* GetMirrorUIStageProgram() {
		return m_pUIMirrorProgramNode;
	}

protected:
	UIStageProgram* m_pUIMirrorProgramNode = nullptr;

// System Applications
private:
	std::shared_ptr<UIKeyboard> m_pKeyboard = nullptr;

	// currently used by DreamGarage to dismiss UI when being seated (temporary)
protected:

	std::shared_ptr<DreamUserApp> m_pDreamUserApp = nullptr;

	// TODO: All of these should go into DreamGarage
	std::shared_ptr<DreamShareViewApp> m_pDreamShareView = nullptr;
	std::shared_ptr<DreamSettingsApp> m_pDreamSettings = nullptr;
	std::shared_ptr<DreamLoginApp> m_pDreamLoginApp = nullptr;
	std::shared_ptr<DreamFormApp> m_pDreamGeneralForm = nullptr;

	// Modules
protected:
	std::shared_ptr<DreamSoundSystem> m_pDreamSoundSystem = nullptr;
	std::shared_ptr<DreamObjectModule> m_pDreamObjectModule = nullptr;

public:
	std::shared_ptr<UIKeyboard> GetKeyboardApp();
	std::shared_ptr<DreamUserApp> GetUserApp();

	virtual RESULT GetDefaultVCamPlacement(point& ptPosition, quaternion& qOrientation) {
		return R_NOT_IMPLEMENTED;
	};

private:
	version m_versionDreamOS;
	UID m_uid;

private:
	RESULT ReleaseFont(std::wstring wstrFontFileName);
	std::shared_ptr<font> GetFont(std::wstring wstrFontFileName);
	RESULT ClearFonts();

	std::map<std::wstring, std::shared_ptr<font>> m_fonts;
};

#include "DreamOS.tpp"

#endif	// ! DREAM_OS_H_
