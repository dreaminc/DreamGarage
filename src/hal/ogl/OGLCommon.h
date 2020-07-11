#ifndef OPEN_GL_COMMON_H_
#define OPEN_GL_COMMON_H_

#if defined(_WIN32)
	#include <windows.h>
	#include <gl/gl.h>
	#include <gl/glu.h>                               // Header File For The GLu32 Library
	#include "hal/ogl/gl/glext.h"
	//#include "hal/ogl/gl/glext.h"
	#include "hal/ogl/gl/glcorearb.h"
#elif defined(__ANDROID__)
	// EGL
	#include <EGL/egl.h>
	#include <EGL/eglext.h>
	#include <GLES3/gl3.h>
	#include <GLES3/gl3ext.h>
#elif defined(__APPLE__)
	#import <OpenGL/gl.h>
	#import <OpenGL/glu.h>
	#include <OpenGL/glext.h>

	//#include <OpenGL/wglext.h>
#endif

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "core/primitives/color.h"

GLenum GetOpenGLPixelFormat(PIXEL_FORMAT pixelFormat, int channels = 3);
GLint GetInternalOpenGLPixelFormat(PIXEL_FORMAT pixelFormat, int bitsPerPixel = 8, int channels = 4);

#endif // ! OPEN_GL_COMMON_H_
