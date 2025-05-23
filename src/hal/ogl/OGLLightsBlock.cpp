#include "OGLLightsBlock.h"
#include "OGLImp.h"

OGLLightsBlock::OGLLightsBlock(OGLProgram *pParentProgram, GLint dataSize, GLint uniformLocationIndex, const char *pszName) :
	OGLUniformBlock(pParentProgram, dataSize, uniformLocationIndex, pszName)
{
	ClearLights();
	SetBindingPoint(LIGHTS_UNIFORM_BLOCK_BINDING_POINT);
}

OGLLightsBlock::~OGLLightsBlock() {
	// empty, everything is static
}

/*
RESULT OGLLightsBlock::GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n) {
	RESULT r = R_PASS;

	pUniformBufferData = (void*)(&m_LightBlock);
	*pUniformBufferData_n = (GLsizeiptr)(sizeof(LightBlock));

Error:
	return r;
}
*/

RESULT OGLLightsBlock::AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) {
	RESULT r = R_PASS;

	LightBlockLight newLight;
	memset(&newLight, 0, sizeof(LightBlockLight));

	LightBlock *pLightBlock = reinterpret_cast<LightBlock*>(m_pUniformBufferData);
	CN(pLightBlock);

	CBM((pLightBlock->numActiveLights < MAX_TOTAL_LIGHTS), "Cannot add more than %d lights", MAX_TOTAL_LIGHTS);

	newLight.type = type;
	newLight.power = intensity;

	newLight.ptOrigin = ptOrigin;
	newLight.colorDiffuse = colorDiffuse;
	newLight.colorSpecular = colorSpecular;
	newLight.vectorDirection = vectorDirection;

	//m_LightBlock.lights[m_LightBlock.numActiveLights++] = light(type, intensity, colorDiffuse, colorSpecular, ptOrigin, vectorDirection);
	pLightBlock->lights[pLightBlock->numActiveLights++] = newLight;

Error:
	return r;
}

RESULT OGLLightsBlock::AddLight(light *pLight) {
	RESULT r = R_PASS;

	CR(AddLight(
		pLight->GetLightType(),
		pLight->GetPower(),
		pLight->GetOrigin(),
		pLight->GetDiffuseColor(),
		pLight->GetSpecularColor(),
		pLight->GetLightDirection()
	));

Error:
	return r;
}

RESULT OGLLightsBlock::ClearLights() {
	RESULT r = R_PASS;

	LightBlock *pLightBlock = reinterpret_cast<LightBlock*>(m_pUniformBufferData);
	CN(pLightBlock);

	memset(pLightBlock, 0, sizeof(LightBlock));
	pLightBlock->numActiveLights = 0;	// for good measure

Error:
	return r;
}

RESULT OGLLightsBlock::SetLights(std::vector<light*> *pLights) {
	RESULT r = R_PASS;

	CR(ClearLights());

	//m_LightBlock.numActiveLights = pLights->size();
	//memcpy(m_LightBlock.lights, pLights->data(), m_LightBlock.numActiveLights * sizeof(light));
	
	for (auto it = pLights->begin(); it != pLights->end(); it++) {
		AddLight((*it));
	}

Error:
	return r;
}