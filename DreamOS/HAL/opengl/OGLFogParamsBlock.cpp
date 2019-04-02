#include "OGLFogParamsBlock.h"
#include "OpenGLImp.h"

OGLFogParamsBlock::OGLFogParamsBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName) :
	OGLUniformBlock(pParentProgram, dataSize, uniformLocationIndex, pszName)
{
	ClearFogParams();
	SetBindingPoint(FOG_UNIFORM_BLOCK_BINDING_POINT);
}

OGLFogParamsBlock::~OGLFogParamsBlock() {
	// empty, everything is static
}

RESULT OGLFogParamsBlock::ClearFogParams() {
	RESULT r = R_PASS;

	FogParamsBlock *pFogParamsBlock = reinterpret_cast<FogParamsBlock*>(m_pUniformBufferData);
	CN(pFogParamsBlock);

	memset(pFogParamsBlock, 0, sizeof(FogParamsBlock));

Error:
	return r;
}

RESULT OGLFogParamsBlock::SetFogParams(const FogParams &fogParams) {
	RESULT r = R_PASS;

	FogParamsBlock *pFogParamsBlock = reinterpret_cast<FogParamsBlock*>(m_pUniformBufferData);
	CN(pFogParamsBlock);

	memcpy(pFogParamsBlock, const_cast<FogParams*>(&fogParams), sizeof(FogParamsBlock));

Error:
	return r;
}
