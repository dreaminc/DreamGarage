#include "OGLRenderingContextFactory.h"

#if defined(_WIN32) 
	#if defined(_WIN64)
		#include "hal/ogl/win64/Win64OGLRenderingContext.h"
	#else
		#include "hal/ogl/win64/Win64OGLRenderingContext.h"
	#endif
#elif defined(__ANDROID__)
	#include "hal/ogl/android/AndroidOGLRenderingContext.h"
#elif defined(__APPLE__)
	#include "hal/ogl/osx/OSXOGLRenderingContext.h"
#elif defined(__linux__)
	#include "hal/ogl/linux/OGLRenderingContext.h"
#endif

OGLRenderingContext* OGLRenderingContextFactory::MakeOGLRenderingContext(OPEN_GL_RC_TYPE type) {
	OGLRenderingContext *pOGLRC = nullptr;

	switch (type) {
		case OPEN_GL_RC_WIN32: {
			#if defined(_WIN32)
					pOGLRC = new Win64OGLRenderingContext();
			#else
				pOGLRC = nullptr;	
				DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
			#endif
		} break;

		case OPEN_GL_RC_ANDROID: {
			#if defined(__ANDROID__)
	`			pOGLRC = new AndroidOGLRenderingContext();
			#else
				pOGLRC = nullptr;
				DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
			#endif
		} break;

		case OPEN_GL_RC_OSX: {
			#if defined(__APPLE__)
					pOGLRC = new OSXOpenGLRenderingContext();
			#else
				pOGLRC = nullptr;
				DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
			#endif
		} break;

		default: {
			pOGLRC = nullptr;
			DEBUG_LINEOUT("OpenGL Rendering Context type %d not supported on this platform!", type);
		} break;
	}

	return pOGLRC;
}