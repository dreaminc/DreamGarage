#ifndef OPEN_GL_RENDERING_CONTEXT
#define OPEN_GL_RENDERING_CONTEXT

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLRenderingContext.h

// The OpenGLRenderingContext is the object that the OpenGL Implementation
// interacts with and should encapsulate all of the platform specific code
// to get OpenGL a rendering context initialized and configured

// This class is the base class that should then be extended for the various platform
// targets

#include "hal/RenderingContext.h"

class Sandbox;

class OGLRenderingContext : public RenderingContext {
public:
	OGLRenderingContext() {
		// empty
	}

	~OGLRenderingContext() {
		// empty
	}
};

#endif // ! OPEN_GL_RENDERING_CONTEXT
