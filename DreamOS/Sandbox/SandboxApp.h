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

class SandboxApp : public valid {
public:
	SandboxApp();
	~SandboxApp();

public:
	virtual RESULT ShowSandbox() = 0;
	virtual RESULT ShutdownSandbox() = 0;
	virtual RESULT RecoverDisplayMode() = 0;		// Do all sandboxes need this ultimately? 

public:
	virtual RESULT InitializePathManager() = 0;
	virtual RESULT InitializeOpenGLRenderingContext() = 0;

public:
	PathManager *GetPathManager();
	OpenGLRenderingContext *GetOpenGLRenderingContext();

protected:
	PathManager *m_pPathManager;
	OpenGLRenderingContext *m_pOpenGLRenderingContext;
	SceneGraph *m_pSceneGraph;

private:
	UID m_uid;
};

#endif // ! SANDBOX_APP_H_
