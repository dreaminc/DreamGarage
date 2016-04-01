#ifndef OGL_FRAGMENT_SHADER_H_
#define OGL_FRAGMENT_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLFragmentShader.h
// This is a OGL fragment shader object

#include "OpenGLShader.h"

#include "Primitives/material.h"
#include "OGLMaterialBlock.h"

class OpenGLImp;	// Declare OpenGLImp class

#define FRAGMENT_SHADER_COLOR_INDEX 0

class OGLFragmentShader : public OpenGLShader {
public:
	OGLFragmentShader(OpenGLImp *pParentImp);

	// This is handled by the parent class 
	//~OGLVertexShader(void);

	RESULT GetAttributeLocationsFromShader();
	RESULT BindUniformBlocks();
	RESULT GetUniformLocationsFromShader();
	RESULT InitializeUniformBlocks();
	RESULT UpdateUniformBlockBuffers();

public:
	const char *GetColorAttributeName() { return "inF_vec3Color"; }
	const char *GetMaterialUniformBlockName() { return "ub_material"; }

	GLuint GetColorIndex() { return FRAGMENT_SHADER_COLOR_INDEX; }


	RESULT SetMaterial(material *pMaterial);

private:
	OGLMaterialBlock *m_pMaterialBlock;
};

#endif // ! OGL_FRAGMENT_SHADER_H_
