#ifndef WIN64_OPEN_GL_RENDERING_CONTEXT
#define WIN64_OPEN_GL_RENDERING_CONTEXT

// Dream OS
// DreamOS/Sandbox/Windows/Win64OpenGLRenderingContext.h
// The Windows 64 specific rendering context for OpenGL

#include "RESULT/EHM.h"

#include "HAL/opengl/OpenGLRenderingContext.h"

// Header File For Windows
#include <windows.h>      

#include <gl\gl.h>
#include <gl\glu.h>                               // Header File For The GLu32 Library
#include <gl\glext.h>
#include <gl\wglext.h>

#include "Sandbox/Windows/Windows64App.h"

class Win64OpenGLRenderingContext : public OpenGLRenderingContext {
public:
	Win64OpenGLRenderingContext();
	~Win64OpenGLRenderingContext();

public:
	RESULT InitializeRenderingContext();
	RESULT InitializeRenderingContext(int versionMajor, int versionMinor);

	RESULT MakeCurrentContext();
	RESULT ReleaseCurrentContext();

private:
	HGLRC m_hglrc;								// OpenGL rendering context
};

#endif // ! WIN64_OPEN_GL_RENDERING_CONTEXT

