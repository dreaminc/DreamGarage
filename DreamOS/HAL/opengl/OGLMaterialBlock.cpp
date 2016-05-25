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

RESULT OGLMaterialBlock::GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n) {
	RESULT r = R_PASS;

	pUniformBufferData = (void*)(&m_materialBlock);
	*pUniformBufferData_n = (GLsizeiptr)(sizeof(MaterialBlock));

Error:
	return r;
}

RESULT OGLMaterialBlock::ClearMaterial() {
	memset(&m_materialBlock, 0, sizeof(MaterialBlock));
	return R_PASS;
}

RESULT OGLMaterialBlock::SetMaterial(material *pMaterial) {
	RESULT r = R_PASS;

	memcpy(&m_materialBlock, pMaterial, sizeof(MaterialBlock));

Error:
	return r;
}