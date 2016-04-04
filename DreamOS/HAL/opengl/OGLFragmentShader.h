#ifndef OGL_FRAGMENT_SHADER_H_
#define OGL_FRAGMENT_SHADER_H_

// Dream OS
// DreamOS/HAL/opengl/OGLFragmentShader.h
// This is a OGL fragment shader object

#include "OpenGLShader.h"

#include "Primitives/material.h"
#include "OGLMaterialBlock.h"

class OpenGLImp;	// Declare OpenGLImp class
class OGLTexture;	

//#define FRAGMENT_SHADER_COLOR_INDEX 0

class OGLFragmentShader : public OpenGLShader {
public:
	OGLFragmentShader(OpenGLImp *pParentImp);

	// This is handled by the parent class 
	//~OGLVertexShader(void);
	
	RESULT BindAttributes();
	RESULT GetAttributeLocationsFromShader();
	RESULT BindUniformBlocks();
	RESULT GetUniformLocationsFromShader();
	RESULT InitializeUniformBlocks();
	RESULT UpdateUniformBlockBuffers();

public:
	const char *GetColorAttributeName() { return "inF_vec3Color"; }
	
	const char *GetTextureUniformName() { return "u_texture";  }
	const char *GetMaterialUniformBlockName() { return "ub_material"; }

	GLint GetColorIndex() { 
		return m_ColorIndex; 
	}

	GLint GetTextureUniformIndex() {
		return m_uniformTextureIndex;
	}
	
	RESULT SetTextureUniform(GLint textureNumber);

	RESULT SetTexture(OGLTexture *pTexture);
	RESULT SetMaterial(material *pMaterial);

private:
	GLint m_ColorIndex;
	GLint m_uniformTextureIndex;

	OGLMaterialBlock *m_pMaterialBlock;
};

#endif // ! OGL_FRAGMENT_SHADER_H_
