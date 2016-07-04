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
#include "HAL/opengl/OpenGLRenderingContext.h"

#include "Scene/SceneGraph.h"

#include <functional>

class CloudController;

class light; 
class sphere; 
class volume; 
class texture; 
class skybox;
class model;

class SandboxApp : public valid {
public:
	SandboxApp();
	~SandboxApp();

public:
	RESULT Initialize();

	virtual RESULT InitializeSandbox() = 0;
	virtual RESULT Show() = 0;
	virtual RESULT Shutdown() = 0;
	virtual RESULT RecoverDisplayMode() = 0;		// Do all sandboxes need this ultimately? 

public:
	virtual RESULT InitializePathManager() = 0;
	virtual RESULT InitializeOpenGLRenderingContext() = 0;
	virtual RESULT InitializeCloudController() = 0;
	virtual RESULT InitializeHAL() = 0;

public:
	RESULT AddObject(VirtualObj *pObject);	// TODO: This may be unsafe
	light* AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	sphere* AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
	volume* AddVolume(double side);
	texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
	skybox *AddSkybox();
	model *AddModel(wchar_t *pszModelName);

	RESULT AddModel(const std::wstring& strRootFolder, const std::wstring& wstrOBJFilename, texture* pTexture, point ptPosition, point_precision scale = 1.0, point_precision rotateY = 0);

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();
	RESULT RegisterUpdateCallback(std::function<RESULT(void)> fnUpdateCallback);
	RESULT UnregisterUpdateCallback();

protected:
	PathManager *m_pPathManager;
	OpenGLRenderingContext *m_pOpenGLRenderingContext;		// TODO: fix it!
	SceneGraph *m_pSceneGraph;
	CloudController *m_pCloudController;

	// TODO: Generalize the implementation architecture - still pretty bogged down in Win32
	//OpenGLImp *m_pOpenGLImp;
	HALImp *m_pHALImp;

protected:
	std::function<RESULT(void)> m_fnUpdateCallback;

private:
	UID m_uid;
};

#endif // ! SANDBOX_APP_H_
