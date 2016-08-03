#ifndef OGL_QUAD_H_
#define OGL_QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLQuad.h
// OpenGL Quadrilateral Object

#include "OGLObj.h"
#include "Primitives/quad.h"

class OGLQuad : public quad, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLQuad(OpenGLImp *pParentImp) :
		quad(1.0f),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, quad&& q) :
		quad(q),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float side, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) :
		quad(side, numHorizontalDivisions, numVerticalDivisions, pTextureHeight),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, float height, float width, int numHorizontalDivisions = 1, int numVerticalDivisions = 1, texture *pTextureHeight = nullptr) :
		quad(height, width, numHorizontalDivisions, numVerticalDivisions, pTextureHeight),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	/*
	// Now handled in OGLObj
	RESULT Render() {
		RESULT r = R_PASS;

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		glDrawArrays(GL_TRIANGLES, 0, 3);

	Error:
		return r;
	}
	*/
};

#endif // ! OGL_QUAD_H_
