#ifndef OGL_GEOMETRY_SHADER_H_
#define OGL_GEOMETRY_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLGeometryShader.h
// This is a OGL fragment shader object

#include "OGLShader.h"

class OGLProgram;	// Declare OpenGLImp class
class OGLTexture;
class OGLObj;

//#define FRAGMENT_SHADER_COLOR_INDEX 0

class OGLGeometryShader : public OGLShader {
public:
	OGLGeometryShader(OGLProgram *pParentProgram);

	// This is handled by the parent class 
	//~OGLVertexShader(void);
	
	RESULT BindAttributes();
	RESULT GetAttributeLocationsFromShader();
};

#endif // ! OGL_GEOMETRY_SHADER_H_
