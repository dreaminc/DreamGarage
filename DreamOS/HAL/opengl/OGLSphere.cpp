#include "OGLSphere.h"

// Pyramid inheritance throws a dominance warning which needs to be suppressed 
// until c++ adds a special keyword to deal with this issue, this is by design
#pragma warning(push)
#pragma warning(disable : 4250)

OGLSphere::OGLSphere(OpenGLImp *pParentImp, sphere::params *pSphereParams) :
	sphere(pSphereParams),
	OGLObj(pParentImp)
{
	//
}


OGLSphere::OGLSphere(OpenGLImp *pParentImp, float radius, int numAngularDivisions, int numVerticalDivisions, color c) :
	sphere(radius, numAngularDivisions, numVerticalDivisions, c),
	OGLObj(pParentImp)
{
	// 
}

OGLSphere::OGLSphere(OpenGLImp *pParentImp, BoundingSphere* pBoundingSphere, bool fTriangleBased) :
	sphere(pBoundingSphere, fTriangleBased),
	OGLObj(pParentImp)
{
	// 
}

RESULT OGLSphere::UpdateFromBoundingSphere(BoundingSphere* pBoundingSphere) {
	RESULT r = R_PASS;

	CR(sphere::UpdateFromBoundingSphere(pBoundingSphere));
	CR(UpdateOGLBuffers());

Error:
	return r;
}

// Override this method when necessary by a child object
// Many objects will not need to though. 
RESULT OGLSphere::Render() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	GLint previousPolygonMode[2]{ 0 };
	glGetIntegerv(GL_POLYGON_MODE, previousPolygonMode);

	if (IsWireframe()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// Strips
	// TODO: quad strip?
	int indexCount = 0;
	int numTriangleStripVerts = 2 * (m_params.numAngularDivisions + 1);
	int numStrips = m_params.numVerticalDivisions - 1;

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

#pragma warning(pop)