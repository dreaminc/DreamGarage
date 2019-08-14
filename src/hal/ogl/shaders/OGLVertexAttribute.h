#ifndef OGL_VERTEX_ATTRIBUTE_H_
#define OGL_VERTEX_ATTRIBUTE_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLVertexAttribute.h

// This is an OpenGL Vertex Attribute and should be owned by an OGLShader

#include <string>

#include "hal/ogl/OGLCommon.h"
#include "hal/ogl/GLSLObject.h"

class OGLVertexAttribute : public GLSLObject {
public:
	OGLVertexAttribute(OGLProgram *pParentProgram, const char *pszAttributeName, GLint attributeLocationIndex, GLint GLType);
	~OGLVertexAttribute();

	RESULT EnableAttribute();
	RESULT BindAttribute();

private:
	std::string m_strAttributeName;
	GLint m_attributeIndex;
	GLint m_GLType;
};

// Explicit specialization (instead of using templates)
class OGLVertexAttributeVector : public OGLVertexAttribute {};
class OGLVertexAttributePoint : public OGLVertexAttribute {};
class OGLVertexAttributeColor : public OGLVertexAttribute {};
class OGLVertexAttributeUVCoord : public OGLVertexAttribute {};

#endif // ! OGL_VERTEX_ATTRIBUTE_H_