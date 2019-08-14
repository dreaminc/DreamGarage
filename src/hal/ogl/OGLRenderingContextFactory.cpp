#include "OGLRenderingContextFactory.h"

#if defined(_WIN32) 
	#if defined(_WIN64)
		#include "hal/ogl/win64/Win64OGLRenderingContext.h"
	#else
		#include "hal/ogl/win64/Win64OGLRenderingContext.h"
	#endif
#elif defined(__APPLE__)
	#include "hal/ogl/osx/OSXOGLRenderingContext.h"
#elif defined(__linux__)
	#include "hal/ogl/linux/OGLRenderingContext.h"
#endif

OGLRenderingContext* OGLRenderingContextFactory::MakeOGLRenderingContext(OPEN_GL_RC_TYPE type) {
	OGLRenderingContext *pOGLRC = NULL;

	switch (type) {
		case OPEN_GL_RC_WIN32: {
			#if defined(_WIN32)
					pOGLRC = new Win64OGLRenderingContext();
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