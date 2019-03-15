#include "OGLFogBlock.h"
#include "OpenGLImp.h"

OGLFogBlock::OGLFogBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName) :
	OGLUniformBlock(pParentProgram, dataSize, uniformLocationIndex, pszName)
{
	ClearFogConfig();
	SetBindingPoint(FOG_UNIFORM_BLOCK_BINDING_POINT);
}

OGLFogBlock::~OGLFogBlock() {
	// empty, everything is static
}

RESULT OGLFogBlock::ClearFogConfig() {
	RESULT r = R_PASS;

	FogBlock *pFogBlock = reinterpret_cast<FogBlock*>(m_pUniformBufferData);
	CN(pFogBlock);

	memset(pFogBlock, 0, sizeof(FogBlock));

Error:
	return r;
}

RESULT OGLFogBlock::SetFogConfig(float startDistance, float endDistance, float density, vector color) {
	RESULT r = R_PASS;

	FogBlock *pNewConfig = new FogBlock();
	memset(pNewConfig, 0, sizeof(FogBlock));

	pNewConfig->startDistance = startDistance;
	pNewConfig->endDistance = endDistance;
	pNewConfig->density = density;
	pNewConfig->color = color;

	FogBlock *pFogBlock = reinterpret_cast<FogBlock*>(m_pUniformBufferData);
	CN(pFogBlock);

	memcpy(pFogBlock, pNewConfig, sizeof(FogBlock));

Error:
	return r;
}
