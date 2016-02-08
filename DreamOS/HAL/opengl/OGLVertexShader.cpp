#include "OGLVertexShader.h"
#include "OpenGLImp.h"

OGLVertexShader::OGLVertexShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_VERTEX_SHADER)
{
	/* empty stub */
}

RESULT OGLVertexShader::BindAttributes() {
	RESULT r = R_PASS;

	CRM(m_pParentImp->BindAttribLocation(GetPositionIndex(), (char*)GetPositionAttributeName()), "Failed to bind %s to position attribute", GetPositionAttributeName());
	CRM(m_pParentImp->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());

	// TODO: Normal and Tex Coord etc

Error:
	return r;
}

RESULT OGLVertexShader::EnableVertexPositionAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetPositionIndex());
}

RESULT OGLVertexShader::EnableVertexColorAttribute() {
	return m_pParentImp->glEnableVertexAtrribArray(GetColorIndex());
}
