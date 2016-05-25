#ifndef OGL_FRAGMENT_SHADER_H_
#define OGL_FRAGMENT_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLFragmentShader.h
// This is a OGL fragment shader object

#include "OpenGLShader.h"

#include "Primitives/material.h"
#include "OGLMaterialBlock.h"

class OGLProgram;	// Declare OpenGLImp class
class OGLTexture;
class OGLObj;

//#define FRAGMENT_SHADER_COLOR_INDEX 0

class OGLFragmentShader : public OpenGLShader {
public:
	OGLFragmentShader(OGLProgram *pParentProgram);

	// This is handled by the parent class 
	//~OGLVertexShader(void);
	
	RESULT BindAttributes();
	RESULT GetAttributeLocationsFromShader();
	
	//RESULT BindUniformBlocks();
	//RESULT GetUniformLocationsFromShader();
	//RESULT InitializeUniformBlocks();
	//RESULT UpdateUniformBlockBuffers();

public:
	// TODO: Output Attributes for Frag shader?
	const char *GetColorAttributeName() { return "inF_vec3Color"; }
	
	// TODO: Push this to the OGLProgram layer
	const char *GetColorTextureUniformName() { return "u_textureColor"; }
	const char *GetBumpTextureUniformName() { return "u_textureBump"; }

	GLint GetColorIndex() { return m_ColorIndex; }

	GLint GetColorTextureUniformIndex() { return m_uniformColorTextureIndex; }
	GLint GetBumpTextureUniformIndex() { return m_uniformBumpTextureIndex; }
	
	RESULT SetTexture(OGLTexture *pTexture);
	
	RESULT SetColorTextureUniform(GLint textureNumber);
	RESULT SetBumpTextureUniform(GLint textureNumber);

	RESULT SetObjectTextures(OGLObj *pOGLObj);

private:
	GLint m_ColorIndex;
	GLint m_uniformColorTextureIndex;
	GLint m_uniformBumpTextureIndex;
};

#endif // ! OGL_FRAGMENT_SHADER_H_
