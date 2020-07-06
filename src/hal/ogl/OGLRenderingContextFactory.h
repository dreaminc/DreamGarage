#ifndef OPEN_GL_RENDERING_CONTEXT_FACTORY_H_
#define OPEN_GL_RENDERING_CONTEXT_FACTORY_H_

// Dream HAL OGL
// dos/src/ogl/OGLRenderingContextFactory.h

// This allows creation of Sandbox without need of direct implementation

#include "OGLRenderingContext.h"

typedef enum {
	OPEN_GL_RC_WIN32,
	OPEN_GL_RC_OSX,
	OPEN_GL_RC_LINUX,
	OPEN_GL_RC_ANDROID,	
	OPEN_GL_RC_DREAMOS,	// Might not be necessary 
	OPEN_GL_RC_INVALID
} OPEN_GL_RC_TYPE;

class OGLRenderingContextFactory {
public:
	static OGLRenderingContext* MakeOGLRenderingContext(OPEN_GL_RC_TYPE type);
};

#endif // ! OPEN_GL_RENDERING_CONTEXT_FACTORY_H_