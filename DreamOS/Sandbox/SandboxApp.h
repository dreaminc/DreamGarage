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
#include "Sandbox/CommandLineManager.h"
#include "HAL/opengl/OpenGLRenderingContext.h"

#include "Scene/ObjectStore.h"

#include <functional>

//class CloudController;
#include "Cloud/CloudController.h"

#include "PhysicsEngine/PhysicsEngine.h"
#include "InteractionEngine/InteractionEngine.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"
#include "Sense/SenseLeapMotion.h"
#include "Sense/SenseController.h"

class light; 
class quad;
class FlatContext;
class sphere; 
class cylinder;
class DimRay;
class volume; 
class texture; 
class skybox;
class model;
class user;
class Message;

class DreamAppManager;

class SandboxApp : 
	public Subscriber<SenseKeyboardEvent>, 
	public Subscriber<SenseTypingEvent>,
	public Subscriber<SenseMouseEvent>,
	public Subscriber<CmdPromptEvent>, 
	public Subscriber<CollisionGroupEvent>, 
	public Subscriber<CollisionObjectEvent>,
	public valid 
{
	friend class DreamOS;

public:
	struct configuration {
		unsigned fUseHMD : 1;
		unsigned fUseLeap : 1;
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

protected:
	RESULT RegisterObjectAndSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber);
	RESULT RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);

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
	virtual RESULT InitializePathManager() = 0;
	virtual RESULT InitializeOpenGLRenderingContext() = 0;
	virtual RESULT InitializeCloudController() = 0;
	virtual RESULT InitializeHAL() = 0;
	virtual RESULT InitializeKeyboard() = 0;
	virtual RESULT InitializeMouse() = 0;
	virtual RESULT InitializeLeapMotion() = 0;
	virtual long GetTickCount();
	virtual	RESULT GetSandboxWindowSize(int &width, int &height) = 0;

public:
	RESULT SetHALConfiguration(HALImp::HALConfiguration halconf);
	const HALImp::HALConfiguration& GetHALConfiguration();
	
private:
	// TODO: Move this up to sandbox
	bool m_fMouseIntersectObjects = false;
	RESULT SetMouseIntersectObjects(bool fMouseIntersectObjects);
	bool IsMouseIntersectObjects();

	RESULT Notify(CmdPromptEvent *event);
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

	RESULT AddInteractionObject(VirtualObj *pObject);
	RESULT UpdateInteractionPrimitive(const ray &rCast);

	RESULT RemoveAllObjects();

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
	model *MakeModel(wchar_t *pszModelName);

	light* AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	quad *AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight, vector vNormal);

	sphere* AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));

	volume* AddVolume(double side, bool fTriangleBased = true);
	volume* AddVolume(double width, double length, double height, bool fTriangleBased = true);

	cylinder* AddCylinder(double radius, double height, int numAngularDivisions, int numVerticalDivisions);

	DimRay* AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);

	text* AddText(const std::wstring& fontName, const std::string& content, double size, bool isBillboard);
	text* AddText(std::shared_ptr<Font> pFont, const std::string& content, double size, bool isBillboard);

	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n);
	texture *MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type);

	skybox *AddSkybox();
	model *AddModel(wchar_t *pszModelName);
	model *AddModel(const std::vector<vertex>& vertices);
	model *AddModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	composite* AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));

	composite* AddComposite();

	user *AddUser();

	// Cloud Controller 
public:
	RESULT RegisterPeersUpdateCallback(HandlePeersUpdateCallback fnHandleEnvironmentConnectionCallback);
	RESULT RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback);
	RESULT RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback);
	RESULT RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback);
	RESULT RegisterAudioDataCallback(HandleAudioDataCallback fnHandleAudioDataCallback);

	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

	RESULT BroadcastDataMessage(Message *pDataMessage);
	RESULT BroadcastUpdateHeadMessage(point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT BroadcastUpdateHandMessage(hand::HandState handState);

	// IO
public:
	RESULT RegisterSubscriber(TimeEventType timeEvent, Subscriber<TimeEvent>* pTimeSubscriber);
	RESULT RegisterSubscriber(SenseVirtualKey keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber);
	RESULT RegisterSubscriber(SenseTypingEventType typingEvent, Subscriber<SenseTypingEvent>* pTypingSubscriber);
	RESULT RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber);
	RESULT RegisterSubscriber(SenseControllerEventType mouseEvent, Subscriber<SenseControllerEvent>* pControllerSubscriber);

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

	camera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	hand *GetHand(hand::HAND_TYPE handType);

public:
	bool IsSandboxRunning();

protected:
	RESULT SetSandboxRunning(bool fRunning);

protected:
	// TODO: Move to unique_ptr
	CommandLineManager *m_pCommandLineManager;
	PathManager *m_pPathManager;
	OpenGLRenderingContext *m_pOpenGLRenderingContext;		// TODO: fix it!
	
	// TODO: Should these be in their respective "engine" objects?
	ObjectStore *m_pPhysicsGraph;	
	ObjectStore *m_pInteractionGraph;
	ObjectStore *m_pSceneGraph;
	ObjectStore *m_pFlatSceneGraph;

	CloudController *m_pCloudController;
	std::unique_ptr<PhysicsEngine> m_pPhysicsEngine;
	std::unique_ptr<InteractionEngine> m_pInteractionEngine;

	// TODO: Generalize to hands controller or something like that (should cover all of the various sensors)
	std::unique_ptr<SenseLeapMotion> m_pSenseLeapMotion;
	SenseKeyboard *m_pSenseKeyboard;
	SenseMouse *m_pSenseMouse;
	HMD *m_pHMD;

	// TODO: Create a "manager manager" or a more generalized way to add these
	// All "managers" should be unique ptrs 
	std::unique_ptr<TimeManager> m_pTimeManager = nullptr;
	std::unique_ptr<DreamAppManager> m_pDreamAppManager = nullptr;

	// TODO: Generalize the implementation architecture - still pretty bogged down in Win32
	//OpenGLImp *m_pOpenGLImp;
	HALImp *m_pHALImp;

// temporary
public:
	RESULT PushAnimation(VirtualObj *pObj, point ptPosition, vector vScale, double duration);
	RESULT CancelAnimation(VirtualObj *pObj);

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
};

#endif // ! SANDBOX_APP_H_

