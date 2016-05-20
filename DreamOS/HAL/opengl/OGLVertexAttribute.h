#ifndef OGL_VERTEX_ATTRIBUTE_H_
#define OGL_VERTEX_ATTRIBUTE_H_

#include "RESULT/EHM.h"

// Dream OS
// DreamOS/HAL/opengl/OGLVertexAttribute.h
// This is an OpenGL Vertex Attribute and should be owned by an OGLShader

#include "OpenGLCommon.h"
#include "GLSLObject.h"

#include <string>

class OGLVertexAttribute : public GLSLObject {
public:
	OGLVertexAttribute(OGLProgram *pParentProgram, const char *pszAttributeName);
	~OGLVertexAttribute();

	RESULT EnableAttribute();
	RESULT BindAttribute();

private:
	OGLProgram *m_pParentProgram;

	std::string m_strAttributeName;
	GLint m_attributeIndex;
};

#endif // ! OGL_VERTEX_ATTRIBUTE_H_