#ifndef OPEN_GL_RENDERING_CONTEXT
#define OPEN_GL_RENDERING_CONTEXT

// Dream OS
// DreamOS/HAL/opengl/OpenGLRenderingContext.h
// The OpenGLRenderingContext is the object that the OpenGL Implementation
// interacts with and should encapsulate all of the platform specific code
// to get OpenGL a rendering context initialized and configured

// This class is the base class that should then be extended for the various platform
// targets

#include "RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"
#include "Primitives/version.h"
//#include "Sandbox/SandboxApp.h"

class SandboxApp;

class OpenGLRenderingContext : public valid {
public:
	OpenGLRenderingContext() {
		// empty
	}

	~OpenGLRenderingContext() {
		// empty
	}

	virtual RESULT InitializeRenderingContext() = 0;
	//virtual RESULT InitializeRenderingContext(int versionMajor, int versionMinor) = 0;
	virtual RESULT InitializeRenderingContext(version versionOGL) = 0;
	virtual RESULT MakeCurrentContext() = 0;
	virtual RESULT ReleaseCurrentContext() = 0;

public:
	RESULT SetParentApp(SandboxApp *pParentApp) {
		m_pParentApp = pParentApp;
		return R_PASS;
	}

	SandboxApp *GetParentApp() { return m_pParentApp; }

protected:
	SandboxApp *m_pParentApp;		// Parent app

private:
	UID m_uid;
};

#endif // ! OPEN_GL_RENDERING_CONTEXT
