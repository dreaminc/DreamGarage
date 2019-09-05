#ifndef GLSL_OBJECT_H_
#define GLSL_OBJECT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/GLSLObject.h

// GLSL Objects are objects that are owned by a parent OGL Program
// and used within it.  These include Vertex Attributes, Uniform Variables / Blocks etc

#include "OGLCommon.h"

class OGLImp;
class OGLProgram;

class GLSLObject {
public:
	GLSLObject(OGLProgram *pParentProgram);
	~GLSLObject();

	OGLImp *GetParentOGLImplementation();

protected:
	OGLProgram *m_pParentProgram;
};

#endif // ! GLSL_OBJECT_H_