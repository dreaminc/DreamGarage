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

#include "Sandbox/PathManager.h"
#include "HAL/opengl/OpenGLRenderingContext.h"

#include "Scene/SceneGraph.h"

class CloudController;

class light; 
class sphere; 
class volume; 
class texture; 
class skybox;

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

public:
	virtual RESULT AddObject(VirtualObj *pObject) = 0;	// TODO: This may be unsafe
	virtual light* AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;
	virtual sphere* AddSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) = 0;
	virtual volume* AddVolume(double side) = 0;

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();

protected:
	PathManager *m_pPathManager;
	OpenGLRenderingContext *m_pOpenGLRenderingContext;		// TODO: fix it!
	SceneGraph *m_pSceneGraph;
	CloudController *m_pCloudController;

private:
	UID m_uid;
};

#endif // ! SANDBOX_APP_H_
