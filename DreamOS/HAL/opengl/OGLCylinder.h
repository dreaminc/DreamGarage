#ifndef OGL_CYLINDER_H_
#define OGL_CYLINDER_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLCylinder.h
// OpenGL Sphere Object

#include "OGLObj.h"
#include "Primitives/cylinder.h"

class OGLCylinder : public cylinder, public OGLObj {
protected:
	DimObj *GetDimObj() {
		return (DimObj*)this;
	}

public:
	OGLCylinder(OpenGLImp *pParentImp, double radius = 1.0f, double height = 1.0f, int numAngularDivisions = MIN_CYLINDER_DIVISIONS, int numVerticalDivisions = MIN_CYLINDER_DIVISIONS) :
		cylinder(radius, height, numAngularDivisions, numVerticalDivisions),
		OGLObj(pParentImp)
	{
		// TODO: Implement valid and CV EHM
		RESULT r = OGLInitialize();
	}

	// Override this method when necessary by a child object
	// Many objects will not need to though. 
	virtual RESULT Render() override {
		RESULT r = R_PASS;

		// TODO: Rethink this since it's in the critical path
		DimObj *pDimObj = GetDimObj();

		CR(m_pParentImp->glBindVertexArray(m_hVAO));
		CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
		CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

		//glDrawElements(GL_POINTS, NumberVertices(), GL_UNSIGNED_INT, nullptr);
		//return r;

		GLint previousPolygonMode;
		glGetIntegerv(GL_POLYGON_MODE, &previousPolygonMode);

		if (pDimObj->IsWireframe()) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		// Strips
		// TODO: quad strip?
		int indexCount = 0;
		int numTriangleStripVerts = 2 * (m_numAngularDivisions + 1);
		int numStrips = m_numVerticalDivisions + 2;

		for (int i = 0; i < numStrips; i++) {
			void *pOffset = (void*)(sizeof(dimindex) * indexCount);
			glDrawElements(GL_TRIANGLE_STRIP, numTriangleStripVerts, GL_UNSIGNED_INT, pOffset);
			indexCount += numTriangleStripVerts;
		}

		if (pDimObj->IsWireframe()) {
			glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode);
		}

	Error:
		return r;
	}
};

#endif // ! OGL_CYLINDER_H_
