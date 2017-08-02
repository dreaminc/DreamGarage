#ifndef DREAM_OS_H_
#define DREAM_OS_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/DreamOS.h
// The DreamOS base class which a DreamOS "application" is derived from
// In the current context of use, this is referring to the application layer of the DreamOS
// client rather than a DreamOS app which will later be rolled out such that applications
// can be run within a client, making a transition to treating Dream as true operating system.
// In the short term, however, DreamOS will run in the context of an application on a given platform
// such as Android or Windows. 

#include "Primitives//Types/UID.h"
#include "Primitives/valid.h"
#include "Primitives/version.h"

#include "Sandbox/SandboxFactory.h"

#define DREAM_OS_VERSION_MAJOR 0
#define DREAM_OS_VERSION_MINOR 1
#define DREAM_OS_VERSION_MINOR_MINOR 0

#include "Primitives/light.h"
#include "Primitives/quad.h"
#include "Primitives/FlatContext.h"
#include "Primitives/sphere.h"
#include "Primitives/cylinder.h"
#include "Primitives/DimRay.h"
#include "Primitives/volume.h"
#include "Primitives/text.h"
#include "Primitives/texture.h"
#include "Primitives/skybox.h"
#include "Primitives/user.h"

#include "PhysicsEngine/PhysicsEngine.h"

#include "DreamAppManager.h"
#include "DreamPeer.h"

#include "UI/UIKeyboard.h"

class UIKeyboardLayout;
class DreamMessage;

class PeerStayAliveMessage;
class PeerAckMessage;
class PeerHandshakeMessage;


class DreamOS : 
	public Subscriber<CollisionObjectEvent>, 
	public valid,
	public CloudController::PeerConnectionObserver,
	public CloudController::EnvironmentObserver,
	public DreamPeer::DreamPeerObserver
{
	friend class CloudTestSuite;

	// TODO: this needs to be revisited
	friend class UIModule;
	friend class HALTestSuite;
	friend class UITestSuite;
	friend class InteractionEngineTestSuite;
	friend class UIViewTestSuite;
	friend class AnimationTestSuite;

public:
	DreamOS();
	~DreamOS();

	RESULT Initialize(int argc = 0, const char *argv[] = nullptr);
	RESULT Start();
	RESULT Exit(RESULT exitcode);

	virtual RESULT ConfigureSandbox() { return R_NOT_IMPLEMENTED; }
	virtual RESULT LoadScene() = 0;
	virtual RESULT SetupPipeline(Pipeline* pRenderPipeline) { return R_NOT_IMPLEMENTED; }
	virtual RESULT Update(void) = 0;

	RESULT GetMouseRay(ray &rCast, double t = 0.0f);

	HMD *GetHMD();


	// PeerConnectionObserver
	virtual RESULT OnNewPeerConnection(long userID, long peerUserID, bool fOfferor, PeerConnection* pPeerConnection) override;
	virtual RESULT OnDataMessage(PeerConnection* pPeerConnection, Message *pDreamMessage) override;
	virtual RESULT OnDataStringMessage(PeerConnection* pPeerConnection, const std::string& strDataChannelMessage) override;
	virtual RESULT OnAudioData(PeerConnection* pPeerConnection, const void* pAudioDataBuffer, int bitsPerSample, int samplingRate, size_t channels, size_t frames) = 0;
	virtual RESULT OnDataChannel(PeerConnection* pPeerConnection) override;
	virtual RESULT OnAudioChannel(PeerConnection* pPeerConnection) override;


	// EnvironmentObserver
	virtual RESULT OnEnvironmentAsset(std::shared_ptr<EnvironmentAsset> pEnvironmentAsset) override {
		return R_NOT_IMPLEMENTED;
	}

	// DreamPeer Observer
	virtual RESULT OnDreamPeerStateChange(DreamPeer* pDreamPeer) override;

	// Cloud Controller Hooks
	virtual RESULT OnNewDreamPeer(PeerConnection *pPeerConnection) = 0;
	virtual RESULT OnDreamMessage(PeerConnection* pPeerConnection, DreamMessage *pDreamMessage) = 0;

	// Peers
	RESULT HandlePeerHandshakeMessage(PeerConnection* pPeerConnection, PeerHandshakeMessage *pPeerHandshakeMessage);
	RESULT HandlePeerStayAliveMessage(PeerConnection* pPeerConnection, PeerStayAliveMessage *pPeerStayAliveMessage);
	RESULT HandlePeerAckMessage(PeerConnection* pPeerConnection, PeerAckMessage *pPeerAckMessage);

protected:
	std::shared_ptr<DreamPeer> CreateNewPeer(PeerConnection *pPeerConnection);
	std::shared_ptr<DreamPeer> FindPeer(long peerUserID);
	std::shared_ptr<DreamPeer> FindPeer(PeerConnection *pPeerConnection);
	RESULT RemovePeer(long peerUserID);
	RESULT RemovePeer(std::shared_ptr<DreamPeer> pDreamPeer);
	DreamPeer::state GetPeerState(long peerUserID);

private:
	std::map<long, std::shared_ptr<DreamPeer>> m_dreamPeers;

public:
	InteractionEngineProxy *GetInteractionEngineProxy();
	CloudController *GetCloudController();
	long GetUserID();

protected:
	RESULT SetHALConfiguration(HALImp::HALConfiguration halconf);
	const HALImp::HALConfiguration& GetHALConfiguration();

	// TODO: This is here temporarily, should be replaced by proper sandbox 
	// related functionality
	HALImp* GetHALImp();


	// Dream Apps
public:
	ControllerProxy* GetCloudControllerProxy(CLOUD_CONTROLLER_TYPE controllerType);

	// TODO: This is here because of template sillyness - but should be 
	// put into a .tpp file with an #include of said tpp file at the end
	// of the header
	template<class derivedAppType>
	std::shared_ptr<derivedAppType> LaunchDreamApp(void *pContext, bool fAddToScene = true) {
		RESULT r = R_PASS;
		
		std::shared_ptr<derivedAppType> pDreamApp = m_pSandbox->m_pDreamAppManager->CreateRegisterAndStartApp<derivedAppType>(pContext, fAddToScene);
		CNM(pDreamApp, "Failed to create app");

		return pDreamApp;

	Error:
		if (pDreamApp != nullptr) {
			pDreamApp = nullptr;
		}

		return nullptr;
	}

//protected:
public:
	// Keyboard
	RESULT InitializeKeyboard();

	// Physics
	RESULT AddPhysicsObject(VirtualObj *pObject);
	RESULT SetGravityAcceleration(double acceleration);
	RESULT SetGravityState(bool fEnabled);

	RESULT AddInteractionObject(VirtualObj *pObject);
	RESULT AddObjectToInteractionGraph(VirtualObj *pObject);
	RESULT AddAndRegisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	//RESULT UpdateInteractionPrimitive(const ray &rCast);

	RESULT CaptureObject(VirtualObj *pObject, VirtualObj *pInteractionObject, point ptContact, vector vDirection, float threshold);
	RESULT ReleaseObjects(VirtualObj *pInteractionObject);

	RESULT AddObjectToUIGraph(VirtualObj *pObject);

	RESULT RemoveObject(VirtualObj *pObject);
	RESULT RemoveAllObjects();

	light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	light *MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	FlatContext *AddFlatContext(int width = 1024, int height = 1024, int channels = 4);
	RESULT RenderToTexture(FlatContext* pContext);

	quad *AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());

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

	sphere *AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	sphere *MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));

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

	DimRay* AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);

	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n);
	texture *MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type);
	texture* MakeTexture(const texture &srcTexture);

	skybox *AddSkybox();
	skybox *MakeSkybox();

	model *AddModel(wchar_t *pszModelName);
	model *MakeModel(wchar_t *pszModelName);
	composite *AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));

	composite *AddComposite();
	composite *MakeComposite();

	user *AddUser();

	Pipeline *GetRenderPipeline();

	stereocamera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	CameraNode* GetCameraNode() { return m_pSandbox->GetCameraNode(); }
	ObjectStoreNode* GetSceneGraphNode() { return m_pSandbox->GetSceneGraphNode(); }
	ObjectStoreNode* GetUISceneGraphNode() { return m_pSandbox->GetUISceneGraphNode(); }

	// Hands
	hand *GetHand(hand::HAND_TYPE handType);

protected:
	long GetTickCount();

	// Physics Engine
protected:
	RESULT RegisterObjectCollision(VirtualObj *pVirtualObject);

	virtual RESULT Notify(CollisionObjectEvent *oEvent) { return R_PASS; }
	
public:
	RESULT RegisterEventSubscriber(VirtualObj* pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject);

	// Cloud Controller
protected:
	RESULT RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver);
	RESULT RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver);

	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT BroadcastDataMessage(Message *pDataMessage);

	// IO
//protected:
public:
	RESULT RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber);
	RESULT RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber);
	RESULT RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber);
	RESULT RegisterSubscriber(SenseControllerEventType controllerEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber);

protected:
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

protected:
	RESULT SetSandboxConfiguration(SandboxApp::configuration sandboxconf);
	const SandboxApp::configuration& GetSandboxConfiguration();

private:
	SandboxApp *m_pSandbox;

// System Applications
public:
	std::shared_ptr<UIKeyboard> GetKeyboard();

private:
	std::shared_ptr<UIKeyboard> m_pKeyboard;

private:
	version m_versionDreamOS;
	UID m_uid;

private:
	RESULT ReleaseFont(std::wstring wstrFontFileName);
	std::shared_ptr<font> GetFont(std::wstring wstrFontFileName);
	RESULT ClearFonts();

	std::map<std::wstring, std::shared_ptr<font>> m_fonts;
};

#endif	// ! DREAM_OS_H_
