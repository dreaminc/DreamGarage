#ifndef OGL_TRIANGLE_H_
#define OGL_TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLTriangle.h
// OpenGL Triangle Object

#include "OGLObj.h"
#include "Primitives/Triangle.h"

class OGLTriangle : public triangle, public OGLObj {
public:
	OGLTriangle(OpenGLImp *pParentImp) :
		OGLObj(pParentImp),
		triangle(1.0f)
	{

	}

	OGLTriangle(OpenGLImp *pParentImp, double side) :
		OGLObj(pParentImp),
		triangle(side)
	{

	}

	OGLTriangle(OpenGLImp *pParentImp, double height, double width) :
		OGLObj(pParentImp),
		triangle(height, width)
	{

	}

	inline void *VertexData() {
		return (void*)m_vertices;
	}

	inline int VertexDataSize() {
		return (3 * sizeof(vertex));
	}

	RESULT Render() {
		RESULT r = R_PASS;

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		glDrawArrays(GL_TRIANGLES, 0, 3);

	Error:
		return r;
	}
};

#endif // ! OGL_TRIANGLE_H_
