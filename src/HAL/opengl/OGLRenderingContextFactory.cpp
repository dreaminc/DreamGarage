#include "OpenGLRenderingContextFactory.h"

#if defined(_WIN32) 
	#if defined(_WIN64)
		#include "./Sandbox/Windows/Win64OpenGLRenderingContext.h"
	#else
		#include "./Sandbox/Windows/Win64OpenGLRenderingContext.h"
	#endif
#elif defined(__APPLE__)
	#include "./Sandbox/Windows/OSXOpenGLRenderingContext.h"
#elif defined(__linux__)
	#include "./Sandbox/Windows/LinuxOpenGLRenderingContext.h"
#endif

OpenGLRenderingContext* OpenGLRenderingContextFactory::MakeOpenGLRenderingContext(OPEN_GL_RC_TYPE type) {
	OpenGLRenderingContext *pOGLRC = NULL;

	switch (type) {
		case OPEN_GL_RC_WIN32: {
			#if defined(_WIN32)
					pOGLRC = new Win64OpenGLRenderingContext();
			#else
					pSandbox = NULL;
					DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
			#endif
		} break;

		case OPEN_GL_RC_OSX: {
			#if defined(__APPLE__)
					pOGLRC = new OSXOpenGLRenderingContext();
			#else
				pOGLRC = NULL;
				DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
			#endif
		} break;

		default: {
			pOGLRC = NULL;
			DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
		} break;
	}

	return pOGLRC;
}