#ifndef OGLPROFILER_H_
#define OGLPROFILER_H_

#include "HAL/opengl/OGLProgram.h"

// Dream OS
// DreamOS/HAL/opengl/OGLProfiler.h
// OGLProfiler renders the profiler to an OGL program

class OGLProfiler {
public:
	static void Render(OpenGLImp* pOGL, OGLProgram* pOGLProgram);
};

#endif // ! OGLPROFILER_H