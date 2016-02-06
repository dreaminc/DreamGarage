#ifndef OGL_QUAD_H_
#define OGL_QUAD_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLQuad.h
// OpenGL Quadrilateral Object

#include "OGLObj.h"
#include "Primitives/quad.h"

class OGLQuad : public quad, public OGLObj {
public:
	OGLQuad(OpenGLImp *pParentImp) :
		quad(1.0f),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, double side) :
		quad(side),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	OGLQuad(OpenGLImp *pParentImp, double height, double width) :
		quad(height, width),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	// TODO: move this to DimObj
	inline vertex *VertexData() {
		return &m_vertices[0];
	}

	// TODO: move this to DimObj
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

#endif // ! OGL_QUAD_H_
