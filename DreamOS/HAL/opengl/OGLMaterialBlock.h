#ifndef OGL_MATERIAL_BLOCK_H_
#define OGL_MATERIAL_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLMaterialBlock.h
// OpenGL Material Block - This is a block representing the current material

#include "OGLUniformBlock.h"

#include "Primitives/material.h"
#include <vector>

#define MATERIAL_UNIFORM_BLOCK_BINDING_POINT 1

class OGLMaterialBlock : public OGLUniformBlock {
public:
	OGLMaterialBlock(OGLProgram *pParentProgram);
	~OGLMaterialBlock();

	RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);

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
		float reserved2;
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
	} m_materialBlock;
#pragma pack(pop)
};

#endif // ! OGL_LIGHTS_BLOCK_H_