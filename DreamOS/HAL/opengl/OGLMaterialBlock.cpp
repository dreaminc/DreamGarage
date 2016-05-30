#include "OGLMaterialBlock.h"
#include "OpenGLImp.h"

OGLMaterialBlock::OGLMaterialBlock(OGLProgram *pParentProgram, GLint dataSize, const char *pszName) :
	OGLUniformBlock(pParentProgram, dataSize, pszName)
{
	ClearMaterial();
	SetBindingPoint(MATERIAL_UNIFORM_BLOCK_BINDING_POINT);
}

OGLMaterialBlock::~OGLMaterialBlock() {
	// empty, everything is static
}

/*
RESULT OGLMaterialBlock::GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n) {
	RESULT r = R_PASS;

	pUniformBufferData = (void*)(&m_materialBlock);
	*pUniformBufferData_n = (GLsizeiptr)(sizeof(MaterialBlock));

Error:
	return r;
}
*/

RESULT OGLMaterialBlock::ClearMaterial() {
	RESULT r = R_PASS;

	MaterialBlock *pMaterialBlock = reinterpret_cast<MaterialBlock*>(m_pUniformBufferData);
	CN(pMaterialBlock);

	memset(pMaterialBlock, 0, sizeof(MaterialBlock));

Error:
	return r;
}

RESULT OGLMaterialBlock::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	MaterialBlock *pMaterialBlock = reinterpret_cast<MaterialBlock*>(m_pUniformBufferData);
	CN(pMaterialBlock);

	memcpy(pMaterialBlock, pMaterial, sizeof(MaterialBlock));

Error:
	return r;
}