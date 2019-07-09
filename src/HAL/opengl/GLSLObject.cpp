#include "GLSLObject.h"

#include "OGLProgram.h"
#include "OpenGLImp.h"

GLSLObject::GLSLObject(OGLProgram *pParentProgram) :
	m_pParentProgram(pParentProgram)
{
	// empty
}

GLSLObject::~GLSLObject() {
	// empty
}

OpenGLImp *GLSLObject::GetParentOGLImplementation() {
	return m_pParentProgram->GetOGLImp();
}