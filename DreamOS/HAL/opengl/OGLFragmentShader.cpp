#include "OGLFragmentShader.h"
#include "OpenGLImp.h"

OGLFragmentShader::OGLFragmentShader(OpenGLImp *pParentImp) :
	OpenGLShader(pParentImp, GL_FRAGMENT_SHADER)
{
	/* empty stub */
}

RESULT OGLFragmentShader::GetAttributeLocationsFromShader() {
	return R_NOT_IMPLEMENTED;
}

RESULT OGLFragmentShader::GetUniformLocationsFromShader() {
	return R_NOT_IMPLEMENTED;
}
