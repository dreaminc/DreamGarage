#ifndef OGL_TRIANGLE_H_
#define OGL_TRIANGLE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLTriangle.h
// OpenGL Triangle Object

#include "OGLObj.h"
#include "Primitives/Triangle.h"

class OGLTriangle : public triangle, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLTriangle(OpenGLImp *pParentImp) :
		triangle(1.0f),
		OGLObj(pParentImp)
	{
		//m_pDimTriangle = new triangle(1.0f);

		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLTriangle(OpenGLImp *pParentImp, double side) :
		triangle(side),
		OGLObj(pParentImp)
	{
		//m_pDimTriangle = new triangle(side);

		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLTriangle(OpenGLImp *pParentImp, double height, double width) :
		triangle(height, width),
		OGLObj(pParentImp)
	{
		//m_pDimTriangle = new triangle(height, width);

		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
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
