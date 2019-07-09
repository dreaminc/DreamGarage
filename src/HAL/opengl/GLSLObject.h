#ifndef GLSL_OBJECT_H_
#define GLSL_OBJECT_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/GLSLObject.h
// GLSL Objects are objects that are owned by a parent OGL Program
// and used within it.  These include Vertex Attributes, Uniform Variables / Blocks etc

#include "OpenGLCommon.h"

class OpenGLImp;
class OGLProgram;

class GLSLObject {
public:
	GLSLObject(OGLProgram *pParentProgram);
	~GLSLObject();

	OpenGLImp *GetParentOGLImplementation();

protected:
	OGLProgram *m_pParentProgram;
};

#endif // ! GLSL_OBJECT_H_