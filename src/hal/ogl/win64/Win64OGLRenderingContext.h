#ifndef WIN64_OPEN_GL_RENDERING_CONTEXT
#define WIN64_OPEN_GL_RENDERING_CONTEXT

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/win64/Win64OpenGLRenderingContext.h

// The Windows 64 specific rendering context for OpenGL

#include <windows.h>      

#include "hal/ogl/OGLRenderingContext.h"

// Header File For Windows

#include <gl\gl.h>
#include <gl\glu.h>                               // Header File For The GLu32 Library
//#include <gl\glext.h>
//#include <gl\wglext.h>

#include "hal/ogl/gl/glext.h"
#include "hal/ogl/gl/wglext.h"

class Win64OGLRenderingContext : public OGLRenderingContext {
public:
	Win64OGLRenderingContext();
	~Win64OGLRenderingContext();

public:
	RESULT InitializeRenderingContext();
	//RESULT InitializeRenderingContext(int versionMajor, int versionMinor);
	RESULT InitializeRenderingContext(version versionOGL);

	RESULT MakeCurrentContext();
	RESULT ReleaseCurrentContext();

private:
	bool WGLExtensionSupported(const char* extension_name);

private:
	PIXELFORMATDESCRIPTOR m_pfd;
	HGLRC m_hglrc;								// OpenGL rendering context
};

#endif // ! WIN64_OPEN_GL_RENDERING_CONTEXT

