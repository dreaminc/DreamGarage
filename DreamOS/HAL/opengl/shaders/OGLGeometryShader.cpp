#include "OGLGeometryShader.h"

#include "../OGLProgram.h"
#include "../OGLTexture.h"
#include "../OGLObj.h"

OGLGeometryShader::OGLGeometryShader(OGLProgram *pParentProgram) :
	OGLShader(pParentProgram, GL_GEOMETRY_SHADER)
{
	// empty
}

// TODO: Remove this?
// TODO: Do any of the shaders actually use these hard bound things, they shouldn't anymore!
RESULT OGLGeometryShader::BindAttributes() {
	RESULT r = R_PASS;

	//CRM(m_pParentProgram->BindAttribLocation(GetColorIndex(), (char*)GetColorAttributeName()), "Failed to bind %s to color attribute", GetColorAttributeName());
	CR(r);

Error:
	return r;
}

// TODO: Remove this?
RESULT OGLGeometryShader::GetAttributeLocationsFromShader() {
	RESULT r = R_PASS;

	GLuint oglProgramID = m_pParentProgram->GetOGLProgramIndex();
	OpenGLImp *pParentImp = GetParentOGLImplementation();

	//CRM(pParentImp->glGetAttribLocation(oglProgramID, GetColorAttributeName(), &m_ColorIndex), "Failed to acquire position GL location");
	CR(r);

Error:
	return r;
}

