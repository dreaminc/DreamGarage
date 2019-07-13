#ifndef OPEN_GL_RENDERING_CONTEXT_FACTORY_H_
#define OPEN_GL_RENDERING_CONTEXT_FACTORY_H_

// DREAM OS
// DreamOS/Sandbox/SandboxFactory
// This allows creation of Sandbox without need of direct implementation

#include "OpenGLRenderingContext.h"

typedef enum {
	OPEN_GL_RC_WIN32,
	OPEN_GL_RC_OSX,
	OPEN_GL_RC_LINUX,
	OPEN_GL_RC_DREAMOS,	// Might not be necessary 
	OPEN_GL_RC_INVALID
} OPEN_GL_RC_TYPE;

class OpenGLRenderingContextFactory {
public:
	static OpenGLRenderingContext* MakeOpenGLRenderingContext(OPEN_GL_RC_TYPE type);
};

#endif // ! OPEN_GL_RENDERING_CONTEXT_FACTORY_H_