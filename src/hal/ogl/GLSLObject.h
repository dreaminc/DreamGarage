#ifndef GLSL_OBJECT_H_
#define GLSL_OBJECT_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/GLSLObject.h

// GLSL Objects are objects that are owned by a parent OGL Program
// and used within it.  These include Vertex Attributes, Uniform Variables / Blocks etc

#include "OGLCommon.h"

#include "core/types/DObject.h"

class OpenGLImp;
class OGLProgram;

class GLSLObject : public DObject {
public:
	GLSLObject(OGLProgram *pParentProgram);
	~GLSLObject();

	OpenGLImp *GetParentOGLImplementation();

protected:
	OGLProgram *m_pParentProgram;
};

#endif // ! GLSL_OBJECT_H_