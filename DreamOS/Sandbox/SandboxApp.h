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

#include "Scene/SceneGraph.h"

#include <functional>

//class CloudController;
#include "Cloud/CloudController.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

class light; 
class quad;
class sphere; 
class volume; 
class texture; 
class skybox;
class model;
class user;
class Message;

class SandboxApp : public valid {
public:
	SandboxApp();
	~SandboxApp();

public:
	RESULT SandboxApp::Initialize(int argc = 0, const char *argv[] = nullptr);

	virtual RESULT InitializeSandbox() = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Shutdown() = 0;
	virtual RESULT RecoverDisplayMode() = 0;		// Do all sandboxes need this ultimately? 

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
	virtual long GetTickCount();

public:
	RESULT AddObject(VirtualObj *pObject);	// TODO: This may be unsafe

	light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	volume* MakeVolume(double width, double length, double height);
	volume* MakeVolume(double side);
	skybox *MakeSkybox();
	model *MakeModel(wchar_t *pszModelName);

	light* AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	quad *AddQuad(double width, double height, int numHorizontalDivisions, int numVerticalDivisions, texture *pTextureHeight);

	sphere* AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3, color c = color(COLOR_WHITE));
	volume* AddVolume(double width, double length, double height);

	volume* AddVolume(double side);

	text* AddText(const std::wstring& fontName, const std::string& content, double size, bool isBillboard);

	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	texture* MakeTexture(texture::TEXTURE_TYPE type, int width, int height, int channels, void *pBuffer, int pBuffer_n);

	skybox *AddSkybox();
	model *AddModel(wchar_t *pszModelName);
	model *AddModel(const std::vector<vertex>& vertices);
	model *AddModel(const std::vector<vertex>& vertices, const std::vector<dimindex>& indices);

	composite* AddModel(const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, vector vEulerRotation = vector(0.0f, 0.0f, 0.0f));
	user *AddUser();

	// Cloud Controller 
public:
	RESULT RegisterDataMessageCallback(HandleDataMessageCallback fnHandleDataMessageCallback);
	RESULT RegisterHeadUpdateMessageCallback(HandleHeadUpdateMessageCallback fnHandleHeadUpdateMessageCallback);
	RESULT RegisterHandUpdateMessageCallback(HandleHandUpdateMessageCallback fnHandleHandUpdateMessageCallback);

	RESULT SendDataMessage(long userID, Message *pDataMessage);
	RESULT SendUpdateHeadMessage(long userID, point ptPosition, quaternion qOrientation, vector vVelocity = vector(), quaternion qAngularVelocity = quaternion());
	RESULT SendUpdateHandMessage(long userID, hand::HandState handState);

	// IO
public:
	RESULT RegisterSubscriber(int keyEvent, Subscriber<SenseKeyboardEvent>* pKeyboardSubscriber);
	RESULT RegisterSubscriber(SenseMouseEventType mouseEvent, Subscriber<SenseMouseEvent>* pMouseSubscriber);

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

	camera* GetCamera();
	point GetCameraPosition();
	quaternion GetCameraOrientation();

	virtual hand *GetHand(hand::HAND_TYPE handType);

protected:
	// TODO: Move to unique_ptr
	CommandLineManager *m_pCommandLineManager;
	PathManager *m_pPathManager;
	OpenGLRenderingContext *m_pOpenGLRenderingContext;		// TODO: fix it!
	SceneGraph *m_pSceneGraph;
	CloudController *m_pCloudController;

	SenseKeyboard *m_pSenseKeyboard;
	SenseMouse *m_pSenseMouse;

	// TODO: Generalize the implementation architecture - still pretty bogged down in Win32
	//OpenGLImp *m_pOpenGLImp;
	HALImp *m_pHALImp;

protected:
	std::function<RESULT(void)> m_fnUpdateCallback;

private:
	UID m_uid;
};

#endif // ! SANDBOX_APP_H_
