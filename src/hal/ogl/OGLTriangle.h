#ifndef OGL_TRIANGLE_H_
#define OGL_TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLTriangle.h
// OpenGL Triangle Object

#include "OGLObj.h"
#include "Primitives/Triangle.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)
class OGLTriangle : public triangle, public OGLObj {
public:
	OGLTriangle(OpenGLImp *pParentImp);
	OGLTriangle(OpenGLImp *pParentImp, float side);
	OGLTriangle(OpenGLImp *pParentImp, float height, float width);
};
#pragma warning(pop)

#endif // ! OGL_TRIANGLE_H_
