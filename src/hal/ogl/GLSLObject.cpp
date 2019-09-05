#include "GLSLObject.h"

#include "OGLProgram.h"
#include "OGLImp.h"

GLSLObject::GLSLObject(OGLProgram *pParentProgram) :
	m_pParentProgram(pParentProgram)
{
	// empty
}

GLSLObject::~GLSLObject() {
	// empty
}

OGLImp *GLSLObject::GetParentOGLImplementation() {
	return m_pParentProgram->GetOGLImp();
}