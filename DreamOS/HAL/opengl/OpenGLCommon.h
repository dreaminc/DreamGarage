#ifndef OPEN_GL_COMMON_H_
#define OPEN_GL_COMMON_H_

#if defined(_WIN32)
	#include <windows.h>

	#include <gl\gl.h>
	#include <gl\glu.h>                               // Header File For The GLu32 Library
	//#include <gl\glext.h>
	#include "HAL/opengl/GL/glext.h"
	#include "HAL/opengl/GL/glcorearb.h"
#elif defined(__APPLE__)
	#import <OpenGL/gl.h>
	#import <OpenGL/glu.h>
	#include <OpenGL/glext.h>

	//#include <OpenGL/wglext.h>
#endif

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#endif // ! OPEN_GL_COMMON_H_
