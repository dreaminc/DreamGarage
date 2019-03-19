#include "OGLFogBlock.h"
#include "OpenGLImp.h"

OGLFogBlock::OGLFogBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName) :
	OGLUniformBlock(pParentProgram, dataSize, uniformLocationIndex, pszName)
{
	ClearFogParams();
	SetBindingPoint(FOG_UNIFORM_BLOCK_BINDING_POINT);
}

OGLFogBlock::~OGLFogBlock() {
	// empty, everything is static
}

RESULT OGLFogBlock::ClearFogParams() {
	RESULT r = R_PASS;

	FogBlock *pFogBlock = reinterpret_cast<FogBlock*>(m_pUniformBufferData);
	CN(pFogBlock);

	memset(pFogBlock, 0, sizeof(FogBlock));

Error:
	return r;
}

RESULT OGLFogBlock::SetFogParams(fogparams *fogParams) {
	RESULT r = R_PASS;

	FogBlock *pFogBlock = reinterpret_cast<FogBlock*>(m_pUniformBufferData);
	CN(pFogBlock);

	memcpy(pFogBlock, fogParams, sizeof(FogBlock));

Error:
	return r;
}
