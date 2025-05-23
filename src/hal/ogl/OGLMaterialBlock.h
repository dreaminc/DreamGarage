#ifndef OGL_MATERIAL_BLOCK_H_
#define OGL_MATERIAL_BLOCK_H_

#include "core/ehm/EHM.h"

// Dream HAL OGL
// dos/src/hal/ogl/OGLMaterialBlock.h

// OpenGL Material Block - This is a block representing the current material

// TODO: This should not be at the HAL layer, but rather at the program/app/module

#include <vector>

#include "shaders/OGLUniformBlock.h"

#include "core/material/material.h"

#define MATERIAL_UNIFORM_BLOCK_BINDING_POINT 1

class OGLMaterialBlock : public OGLUniformBlock {
public:
	OGLMaterialBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName);
	~OGLMaterialBlock();

	//RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);

	RESULT ClearMaterial();
	RESULT SetMaterial(material *pMaterial);

private:

	// This is the equivalent to the shader
	// Since the light virtual object has other stuff in there
	// TODO? Maybe move to VirtualObj as the standard

/*
#pragma pack(push)  
#pragma pack(1)     
	struct MaterialBlock {
		float m_shine;
		float reserved1;
		float m_ambient;
		float reserved3;

		color m_colorAmbient;
		color m_colorDiffuse;
		color m_colorSpecular;
	} m_materialBlock;
#pragma pack(pop)
*/

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	struct MaterialBlock {
		material m_material;
	};
#pragma pack(pop)
};

#endif // ! OGL_LIGHTS_BLOCK_H_