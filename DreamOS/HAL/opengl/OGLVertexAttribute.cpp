#include "OGLVertexAttribute.h"

#include "OpenGLImp.h"

OGLVertexAttribute::OGLVertexAttribute(OGLProgram *pParentProgram, const char *pszAttributeName) :
	GLSLObject(pParentProgram)
{
	m_strAttributeName.assign(pszAttributeName);
}

OGLVertexAttribute::~OGLVertexAttribute() {
	// empty
}

RESULT OGLVertexAttribute::EnableAttribute() {
	RESULT r = R_PASS;

	OpenGLImp *pParentImp = m_pParentProgram->GetOGLImp();
	CRM(pParentImp->glEnableVertexAtrribArray(m_attributeIndex), "Failed to enable %s attribute", m_strAttributeName.c_str());

Error:
	return r;
}

RESULT OGLVertexAttribute::BindAttribute() {
	RESULT r = R_PASS;
	
	OpenGLImp *pParentImp = m_pParentProgram->GetOGLImp();
	CRM(pParentImp->BindAttribLocation(m_attributeIndex, m_strAttributeName.c_str()), "Failed to bind %s attribute", m_strAttributeName.c_str());

Error:
	return r;
}