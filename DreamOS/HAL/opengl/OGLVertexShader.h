#ifndef OGL_VERTEX_SHADER_H_
#define OGL_VERTEX_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLVertexShader.h
// This is a OGL vertex shader object

#include "OpenGLShader.h"

class OpenGLImp;	// Declare OpenGLImp class

#define VERTEX_SHADER_POSITION_INDEX 0
#define VERTEX_SHADER_COLOR_INDEX 1
#define VERTEX_SHADER_NORMAL_INDEX 2

class OGLVertexShader : public OpenGLShader {
public:
	OGLVertexShader(OpenGLImp *pParentImp);
	
	// This is handled by the parent class 
	//~OGLVertexShader(void);

public:
	// TODO: These could be generated, even within a macro
	const char *GetPositionAttributeName() {
		return "inV_vec4Position";
	}

	const char *GetColorAttributeName() {
		return "inV_vec4Color";
	}

	const char *GetNormalAttributeName() {
		return "inV_vec4Normal";
	}

	GLuint GetPositionIndex() {
		return VERTEX_SHADER_POSITION_INDEX;
	}

	GLuint GetColorIndex() {
		return VERTEX_SHADER_COLOR_INDEX;
	}

	GLuint GetNormalIndex() {
		return VERTEX_SHADER_NORMAL_INDEX;
	}

	RESULT BindAttributes();
	RESULT EnableVertexPositionAttribute();
	RESULT EnableVertexColorAttribute();
	RESULT EnableVertexNormalAttribute();
};

#endif // ! OGL_VERTEX_SHADER_H_
