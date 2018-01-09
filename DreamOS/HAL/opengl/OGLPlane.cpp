#include "OGLPlane.h"
#include "OGLVolume.h"

OGLPlane::OGLPlane(OpenGLImp *pParentImp, point ptOrigin, vector vNormal) :
	DimPlane(ptOrigin, vNormal),
	OGLObj(pParentImp) {

	RESULT r = OGLInitialize();
}

RESULT OGLPlane::Render() {

	// TODO: Force infinity! 

	return OGLObj::Render();
	/*
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	int numLines = NumberIndices() / 2;

	for (int i = 0; i < numLines; i++) {
		void *pOffset = (void*)(sizeof(dimindex) * i * 2);
		glDrawElements(GL_LINES, NumberIndices(), GL_UNSIGNED_INT, pOffset);
	}
	

Error:
	return R_PASS;
	*/
}