#ifndef SANDBOX_APP_H_
#define SANDBOX_APP_H_

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"

#include "Primitives/valid.h"

// DREAM OS
// DreamOS/SandboxApp.h
// The Sandbox App is effectively a class to contain the common functions of running DreamOS in a Sandbox mode
// on a given platform.  A Sandbox implementation should effectively virtualize the host system as if it is running
// natively on the DreamBox

#include "HAL/HALImp.h"

#include "Sandbox/PathManager.h"
#include "Sandbox/CredentialManager.h"
#include "Sandbox/CommandLineManager.h"
#include "HAL/opengl/OpenGLRenderingContext.h"

#include "Scene/ObjectStoreNode.h"
#include "Scene/CameraNode.h"

#include <functional>

//class CloudController;
#include "Cloud/CloudController.h"

#include "PhysicsEngine/PhysicsEngine.h"
#include "InteractionEngine/InteractionEngine.h"

#include "Primitives/viewport.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"
#include "Sense/SenseLeapMotion.h"
#include "Sense/SenseController.h"
#include "Sense/SenseGamepadController.h"

class light; 
class quad;
class FlatContext;
class sphere; 
class cylinder;
class DimRay;
class DimPlane;
class volume; 
class texture; 
class skybox;
class model;
class user;
class Message;
class DreamAppMessage;

class UIKeyboardLayout;

class DreamAppManager;

class SandboxApp : 
	public Subscriber<SenseKeyboardEvent>, 
	public Subscriber<SenseTypingEvent>,
	public Subscriber<SenseMouseEvent>,
	public Subscriber<CollisionGroupEvent>, 
	public Subscriber<CollisionObjectEvent>,
	public valid 
{
	friend class DreamOS;

public:
	struct configuration {
		unsigned fUseHMD : 1;
		unsigned fUseLeap : 1;
		unsigned fMouseLook : 1;
		unsigned fUseGamepad : 1;
		unsigned fInitCloud : 1;
		unsigned fInitSound : 1;
	};

private:
	SandboxApp::configuration m_SandboxConfiguration;

public:
	SandboxApp();
	~SandboxApp();

	RESULT SetSandboxConfiguration(SandboxApp::configuration sandboxconf);
	const SandboxApp::configuration& GetSandboxConfiguration();

public:
	RESULT Initialize(int argc = 0, const char *argv[] = nullptr);
	RESULT RunAppLoop();
	RESULT Shutdown();

	virtual RESULT InitializeSandbox() = 0;
	virtual RESULT Show() = 0;
	
	virtual RESULT RecoverDisplayMode() = 0;		// Do all sandboxes need this ultimately? 
	virtual RESULT HandleMessages() = 0;
	virtual RESULT SwapDisplayBuffers() = 0;
	virtual RESULT ShutdownSandbox() = 0;

private:
	RESULT InitializePhysicsEngine();
	RESULT InitializeInteractionEngine();
	RESULT InitializeTimeManager();
	RESULT InitializeDreamAppManager();
	RESULT InitializeCamera();

protected:
	RESULT RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber);
	
	// Interaction Engine
	RESULT RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT RegisterEventSubscriber(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject, InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);
	RESULT UnregisterInteractionObject(VirtualObj *pObject);
	RESULT UnregisterInteractionSubscriber(Subscriber<InteractionObjectEvent>* pInteractionSubscriber);

public:
	enum class SANDBOX_WINDOW_POSITION {
		LEFT,
		RIGHT,
		TOP,
		BOTTOM,
		CENTER,
		INVALID
	};

	virtual RESULT SetSandboxWindowPosition(SANDBOX_WINDOW_POSITION sandboxWindowPosition) = 0;

public:
	virtual RESULT InitializePathManager(DreamOS *pDOSHandle) = 0;
	virtual RESULT InitializeCredentialManager() = 0;
	virtual RESULT InitializeOpenGLRenderingContext() = 0;
	virtual RESULT InitializeCloudController() = 0;
	virtual RESULT InitializeKeyboard() = 0;
	virtual RESULT InitializeMouse() = 0;
	virtual RESULT InitializeLeapMotion() = 0;
	virtual RESULT InitializeGamepad() = 0;
	virtual long GetTickCount();
	virtual RESULT GetStackTrace() = 0;
	virtual	RESULT GetSandboxWindowSize(int &width, int &height) = 0;
	virtual HWND GetWindowHandle() = 0;
	
	// HAL
	virtual RESULT InitializeHAL();
	RESULT SetUpHALPipeline(Pipeline* pRenderPipeline);		// TODO: this goes up to DreamOS soon

	// HMD
	RESULT InitializeHMD();

public:
	RESULT SetHALConfiguration(HALImp::HALConfiguration halconf);
	const HALImp::HALConfiguration& GetHALConfiguration();
	
private:
	// TODO: Move this up to sandbox
	bool m_fMouseIntersectObjects = false;
	RESULT SetMouseIntersectObjects(bool fMouseIntersectObjects);
	bool IsMouseIntersectObjects();

	RESULT Notify(SenseKeyboardEvent *kbEvent);
	RESULT Notify(SenseTypingEvent *kbEvent);
	RESULT Notify(SenseMouseEvent *mEvent);

	RESULT Notify(CollisionObjectEvent *oEvent);
	RESULT Notify(CollisionGroupEvent* gEvent);

protected:
	RESULT RegisterImpKeyboardEvents();
	RESULT RegisterImpMouseEvents();
	RESULT RegisterImpLeapMotionEvents();
	RESULT RegisterImpControllerEvents();

public:
	RESULT GetMouseRay(ray &rCast, double t = 0.0f);

public:
	// Physics
	RESULT AddPhysicsObject(VirtualObj *pObject);
	RESULT SetGravityAcceleration(double acceleration);
	RESULT SetGravityState(bool fEnabled);

	RESULT AddObjectToInteractionGraph(VirtualObj *pObject);
	RESULT RemoveObjectFromInteractionGraph(VirtualObj *pObject);
	RESULT AddInteractionObject(VirtualObj *pObject);
	//RESULT UpdateInteractionPrimitive(const ray &rCast);

	RESULT AddObjectToUIGraph(VirtualObj *pObject);
	RESULT AddObjectToUIClippingGraph(VirtualObj *pObject);

	RESULT RemoveObjectFromUIGraph(VirtualObj *pObject);
	RESULT RemoveObjectFromUIClippingGraph(VirtualObj *pObject);

	RESULT RemoveAllObjects();
	RESULT RemoveObject(VirtualObj *pObject);

	RESULT AddObject(VirtualObj *pObject);	
	FlatContext* AddFlatContext(int width, int height, int channels);
	RESULT RenderToTexture(FlatContext* pContext);

	light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	volume* MakeVolume(double side, bool fTriangleBased = true);
	volume* MakeVolume(double width, double length, double height, bool fTriangleBased = true);
	cylinder* MakeCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions);
	DimRay* MakeRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);
	skybox *MakeSkybox();

	virtual RESULT SetKeyValue(std::wstring wstrKey, std::string strField, CredentialManager::type credType, bool fOverwrite) = 0;
	virtual RESULT GetKeyValue(std::wstring wstrKey, std::string &strOut, CredentialManager::type credType) = 0;
	virtual RESULT RemoveKeyValue(std::wstring wstrKey, CredentialManager::type credType) = 0;

	light* AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	quad *AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal);
	quad *MakeQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());

	template<typename objType, typename... Targs>
	objType *TAddObject(Targs... Fargs) {
		RESULT r = R_PASS;

		objType *pObj = m_pHALImp->TMakeObject<objType>(Fargs...);
		CN(pObj);

		CR(AddObject(pObj));

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			delete pObj;
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType, typename... Targs>
	objType *TMakeObject(Targs... Fargs) {
		RESULT r = R_PASS;

		objType *pObj = m_pHALImp->TMakeObject<objType>(Fargs...);
		CN(pObj);

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			delete pObj;
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType>
	objType *TAddObject() {
		RESULT r = R_PASS;

		objType *pObj = m_pHALImp->TMakeObject();
		CN(pObj);

		CR(AddObject(pObj));

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			delete pObj;
			pObj = nullptr;
		}

		return nullptr;
	}

	template<typename objType>
	objType *TMakeObject() {
		RESULT r = R_PASS;

		objType *pObj = m_pHALImp->TMakeObject();
		CN(pObj);

		//Success:
		return pObj;

	Error:
		if (pObj != nullptr) {
			delete pObj;
			pObj = nullptr;
		}

		return nullptr;
	}

	sphere* AddSphere(float radius = 1.0f, int numAngularDivisions = 10, int numVerticalDivisions = 10, color c = color(COLOR_WHITE));

	volume* AddVolume(double side, bool fTriangleBased = true);
	volume* AddVolume(double width, double length, double height, bool fTriangleBased = true);

	cylinder* AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions);

	DimRay* AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);

	DimPlane* MakePlane(point ptOrigin = point(), vector vNormal = vector::jVector(1.0f));
	DimPlane* AddPlane(point ptOrigin = point(), vector vNormal = vector::jVector(1.0f));

	text *AddText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, UIKeyboardLayout *pLayout, double margin, text::flags textFlags = text::flags::NONE);

	text *AddText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double lineHeightM = 0.25f, text::flags textFlags = text::flags::NONE);

	text *AddText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);
	text *MakeText(std::shared_ptr<font> pFont, const std::string& strContent, double width = 1.0f, double height = 0.25f, text::flags textFlags = text::flags::NONE);

	text* MakeText(std::shared_ptr<font> pFont, const std::string& content, double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	text* MakeText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double width = 1.0f, double height = 0.25f, bool fBillboard = false);

	text* AddText(const std::wstring& fontName, const std::string& content, double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	text* AddText(std::shared_ptr<font> pFont, const std::string& content, double width = 1.0f, double height = 0.25f, bool fBillboard = false);
	text* AddText(std::shared_ptr<font> pFont, texture *pFontTexture, const std::string& content, double width = 1.0f, double height = 0.25f, bool fBillboard = false);

	texture* MakeTexture(const wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, PIXEL_FORMAT pixelFormat, int channels, void *pBuffer, int pBuffer_n);
	texture *MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type);
	texture* MakeTexture(const texture &srcTexture);

	skybox *AddSkybox();

	//model *AddModel(wchar_t *pszModelName);
	//model *MakeModel(wchar_t *pszModelName);

	mesh *AddMesh(const std::vector<vertex>& vertices);
	mesh *AddMesh(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	//composite* AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));
	model* MakeModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);
	model* AddModel(const std::wstring& wstrModelFilename, texture* pTexture = nullptr);

	composite* AddComposite();
	composite* MakeComposite();

	user *AddUser();
	user *MakeUser();

	// Cloud Controller 
public:
	RESULT RegisterPeerConnectionObserver(CloudController::PeerConnectionObserver *pPeerConnectionObserver);
	RESULT RegisterEnvironmentObserver(CloudController::EnvironmentObserver *pEnvironmentObserver);
	RESULT RegisterUserObserver(CloudController::UserObserver *pUserObserver);

	RESULT BroadcastVideoFrame(uint8_t *pVideoFrameBuffer, int pxWidth, int pxHeight, int channels);
	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT BroadcastDataMessage(Message *pDataMessage);

	RESULT BroadcastDreamAppMessage(DreamAppMessage *pDreamAppMessage);

	RESULT HandleDreamAppMessage(PeerConnection* pPeerConnection, DreamAppMessage *pDreamAppMessage);

	// IO
public:
	RESULT RegisterSubscriber(TimeEventType timeEvent, Subscriber<TimeEvent>* pTimeSubscriber);
	RESULT RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber);
	RESULT RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber);
	RESULT RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber);
	RESULT RegisterSubscriber(SenseControllerEventType mouseEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber);
	RESULT RegisterSubscriber(SenseGamepadEventType mouseEvent, Subscriber<SenseGamepadEvent>* pGamepadSubscriber);

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();
	RESULT ResizeViewport(viewport newViewport);

	stereocamera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	CameraNode* GetCameraNode() { return m_pCamera; }
	ObjectStoreNode* GetSceneGraphNode() { return m_pSceneGraph; }
	ObjectStoreNode* GetUISceneGraphNode() { return m_pUISceneGraph; }
	ObjectStoreNode* GetUIClippingSceneGraphNode() { return m_pUIClippingSceneGraph; }

	hand *GetHand(HAND_TYPE handType);

public:
	bool IsSandboxRunning();

protected:
	RESULT SetSandboxRunning(bool fRunning);

protected:
	viewport m_viewport;

	// TODO: Move to unique_ptr
	CommandLineManager *m_pCommandLineManager = nullptr;
	PathManager *m_pPathManager = nullptr;
	OpenGLRenderingContext *m_pOpenGLRenderingContext = nullptr;		// TODO: fix it!
	
	// TODO: Should these be in their respective "engine" objects?
	ObjectStore *m_pPhysicsGraph = nullptr;	
	ObjectStore *m_pInteractionGraph = nullptr;
	ObjectStore *m_pFlatSceneGraph = nullptr;

	//ObjectStore *m_pSceneGraph;
	ObjectStoreNode *m_pSceneGraph = nullptr;
	ObjectStoreNode *m_pUISceneGraph = nullptr;
	ObjectStoreNode *m_pUIClippingSceneGraph = nullptr;

	CloudController *m_pCloudController = nullptr;
	std::unique_ptr<PhysicsEngine> m_pPhysicsEngine = nullptr;
	std::unique_ptr<InteractionEngine> m_pInteractionEngine = nullptr;

	// TODO: Generalize to hands controller or something like that (should cover all of the various sensors)
	std::unique_ptr<SenseLeapMotion> m_pSenseLeapMotion = nullptr;
	SenseKeyboard *m_pSenseKeyboard = nullptr;
	SenseMouse *m_pSenseMouse = nullptr;
	SenseGamepadController *m_pSenseGamepad = nullptr;
	HMD *m_pHMD = nullptr;

	// TODO: Create a "manager manager" or a more generalized way to add these
	// All "managers" should be unique pointers 
	std::unique_ptr<TimeManager> m_pTimeManager = nullptr;
	std::unique_ptr<DreamAppManager> m_pDreamAppManager = nullptr;
	std::unique_ptr<CredentialManager> m_pCredentialManager = nullptr;

	// TODO: Generalize the implementation architecture - still pretty bogged down in Win32
	//OpenGLImp *m_pOpenGLImp;
	HALImp *m_pHALImp = nullptr;

	//std::shared_ptr<stereocamera> m_pCamera = nullptr;
	CameraNode *m_pCamera = nullptr;

public:
	InteractionEngineProxy *GetInteractionEngineProxy();

protected:
	RESULT SetDreamOSHandle(DreamOS *pDreamOSHandle);
	DreamOS *GetDreamOSHandle();
	DreamOS *m_pDreamOSHandle = nullptr;

protected:
	std::function<RESULT(void)> m_fnUpdateCallback;

private:
	bool m_fRunning = false;

private:
	UID m_uid;

public:
	std::wstring GetHardwareID();
	std::string GetHMDTypeString();

protected:
	std::wstring m_strHardwareID;
};

#endif // ! SANDBOX_APP_H_

