#ifndef GLSL_OBJECT_H_
#define GLSL_OBJECT

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/GLSLObject.h
// GLSL Objects are objects that are owned by a parent OGL Program
// and used within it.  These include Vertex Attributes, Uniform Variables / Blocks etc

#include "OpenGLCommon.h"
#include "OGLProgram.h"

class GLSLObject {
public:
	GLSLObject(OGLProgram *pParentProgram) :
		m_pParentProgram(pParentProgram)
	{
		// empty
	}

	~GLSLObject() {
		// empty
	}

private:
	OGLProgram *m_pParentProgram;
};

#endif // ! GLSL_OBJECT