#ifndef OGL_TRIANGLE_H_
#define OGL_TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLTriangle.h
// OpenGL Triangle Object

#include "OGLObj.h"
#include "Primitives/Triangle.h"
#include "OpenGLImp.h"

class OGLTriangle : public triangle, public OGLObj {
public:
	OGLTriangle(OpenGLImp *pParentImp) :
		triangle(1.0f)
	{

	}

	OGLTriangle(OpenGLImp *pParentImp, double side) :
		triangle(side)
	{

	}

	OGLTriangle(OpenGLImp *pParentImp, double height, double width) :
		triangle(height, width)
	{

	}

private:
	OpenGLImp *m_pParentImp;
	GLuint m_VAO;
};

#endif // ! OGL_TRIANGLE_H_
