#include "OGLRay.h"
#include "OGLVolume.h"

OGLRay::OGLRay(OpenGLImp *pParentImp, point ptOrigin, vector vDirection, float step, bool fDirectional) :
	DimRay(ptOrigin, vDirection, step),
	OGLObj(pParentImp) {

	RESULT r = OGLInitialize();
	if (fDirectional) {
		//TODO: Nested object does not fit with usual pattern of primitives
		// move to native arrow tip implementation
		m_rayTip = std::shared_ptr<volume>(new OGLVolume(pParentImp, 0.01f, true));
		AddChild(m_rayTip);
		m_rayTip->SetPosition(point(ptOrigin + point(step * vDirection)));
	}
}

RESULT OGLRay::Render() {
	RESULT r = R_PASS;
	
	DimObj *pDimObj = GetDimObj();
	
	CR(m_pParentImp->glBindVertexArray(m_hVAO));
	CR(m_pParentImp->glBindBuffer(GL_ARRAY_BUFFER, m_hVBO));
	CR(m_pParentImp->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_hIBO));

	glDrawElements(GL_LINE_STRIP, pDimObj->NumberIndices(), GL_UNSIGNED_INT, NULL);

Error:
	return R_PASS;
}