#include "OGLCylinder.h"

OGLCylinder::OGLCylinder(OpenGLImp *pParentImp, double radius, double height, int numAngularDivisions, int numVerticalDivisions) :
	cylinder(radius, height, numAngularDivisions, numVerticalDivisions),
	OGLObj(pParentImp)
{
	// TODO: Implement valid and CV EHM
	RESULT r = OGLInitialize();
}

// Override this method when necessary by a child object
// Many objects will not need to though. 
RESULT OGLCylinder::Render() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	//glDrawElements(GL_POINTS, NumberVertices(), GL_UNSIGNED_INT, nullptr);
	//return r;

	GLint previousPolygonMode[2]{ 0 };
	glGetIntegerv(GL_POLYGON_MODE, previousPolygonMode);

	if (IsWireframe()) {
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

	if (IsWireframe()) {
		if (previousPolygonMode[1] != 0) {
			glPolygonMode(GL_FRONT, previousPolygonMode[0]);
			glPolygonMode(GL_BACK, previousPolygonMode[1]);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, previousPolygonMode[0]);
		}
	}

Error:
	return r;
}