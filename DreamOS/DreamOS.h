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

#include "UI/UIKeyboard.h"

class DreamOS : 
	public Subscriber<CollisionObjectEvent>, 
	public valid 
{
	friend class CloudTestSuite;

	// TODO: this needs to be revisited
	friend class UIModule;
	friend class HALTestSuite;
	friend class UITestSuite;

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

public:
	InteractionEngineProxy *GetInteractionEngineProxy();
	CloudController *GetCloudController();

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
	std::shared_ptr<derivedAppType> LaunchDreamApp(void *pContext) {
		RESULT r = R_PASS;
		
		std::shared_ptr<derivedAppType> pDreamApp = m_pSandbox->m_pDreamAppManager->CreateRegisterAndStartApp<derivedAppType>(pContext);
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
	RESULT UpdateInteractionPrimitive(const ray &rCast);

	RESULT RemoveAllObjects();

	light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	light *MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);

	FlatContext *AddFlatContext(int width = 1024, int height = 1024, int channels = 4);
	RESULT RenderToTexture(FlatContext* pContext);

	quad *AddQuad(double width, double height, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr, vector vNormal = vector::jVector());

	sphere *AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	sphere *MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));

	text *AddText(std::shared_ptr<Font> pFont, const std::string& content, double size = 1.0f, bool isBillboard = false);
	text *MakeText(std::shared_ptr<Font> pFont, const std::string& content, double size = 1.0f, bool isBillboard = false);
	
	volume *MakeVolume(double side, bool fTriangleBased = true);
	volume *MakeVolume(double width, double length, double height, bool fTriangleBased = true);

	volume* AddVolume(double side, bool fTriangleBased = true);
	volume *AddVolume(double width, double length, double height, bool fTriangleBased = true);

	cylinder* AddCylinder(double radius, double height, int numAngularDivisions = 3, int numVerticalDivisions = 3);

	DimRay* AddRay(point ptOrigin, vector vDirection, float step = 1.0f, bool fDirectional = true);

	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, texture::PixelFormat format, int channels, void *pBuffer, int pBuffer_n);
	texture *MakeTextureFromFileBuffer(uint8_t *pBuffer, size_t pBuffer_n, texture::TEXTURE_TYPE type);

	skybox *AddSkybox();
	skybox *MakeSkybox();

	model *AddModel(wchar_t *pszModelName);
	model *MakeModel(wchar_t *pszModelName);
	composite *AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));
	composite *AddComposite();

	user *AddUser();

	Pipeline *GetRenderPipeline();

	stereocamera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	CameraNode* GetCameraNode() { return m_pSandbox->GetCameraNode(); }
	ObjectStoreNode* GetSceneGraphNode() { return m_pSandbox->GetSceneGraphNode(); }

	// Hands
	hand *GetHand(hand::HAND_TYPE handType);

protected:
	long GetTickCount();

	// Physics Engine
protected:
	RESULT RegisterObjectCollision(VirtualObj *pVirtualObject);

	virtual RESULT Notify(CollisionObjectEvent *oEvent) { return R_PASS; }
	
public:
	RESULT RegisterEventSubscriber(InteractionEventType eventType, Subscriber<InteractionObjectEvent>* pInteractionSubscriber);

	// Cloud Controller
protected:
	RESULT RegisterPeersUpdateCallback(HandlePeersUpdateCallback fnHandlePeersUpdateCallback);
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
};

#endif	// ! DREAM_OS_H_
