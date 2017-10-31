#include "OGLRay.h"
#include "OGLVolume.h"

OGLRay::OGLRay(OpenGLImp *pParentImp, point ptOrigin, vector vDirection, float step, bool fDirectional) :
	DimRay(ptOrigin, vDirection, step),
	OGLObj(pParentImp) {

	RESULT r = OGLInitialize();
}

RESULT OGLRay::Render() {
	RESULT r = R_PASS;
	
	DimObj *pDimObj = GetDimObj();
	
	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	int numLines = pDimObj->NumberIndices() / 2;

	for (int i = 0; i < numLines; i++) {
		void *pOffset = (void*)(sizeof(dimindex) * i * 2);
		glDrawElements(GL_LINES, pDimObj->NumberIndices(), GL_UNSIGNED_INT, pOffset);
	}

Error:
	return R_PASS;
}