#include "OGLBillboard.h"

OGLBillboard::OGLBillboard(OpenGLImp *pParentImp, point ptOrigin, float width, float height, texture *pTexture) :
	billboard(ptOrigin,width,height,pTexture),
	OGLObj(pParentImp)
{
	RESULT r = OGLInitialize();
}

OGLBillboard::~OGLBillboard() {
	// empty
}

// override to specify GL_POIONT
RESULT OGLBillboard::Render() {
	RESULT r = R_PASS;

	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	glDrawElements(GL_POINT, 1, GL_UNSIGNED_INT, 0);
	
Error:
	return r;
}
