#ifndef OGL_LIGHTS_BLOCK_H_
#define OGL_LIGHTS_BLOCK_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLLightsBlock.h
// OpenGL Lights Block - effectively responsible for passing the light data block to the OGLImp

#include "OGLUniformBlock.h"

#include "Primitives/light.h"
#include <vector>

#define MAX_TOTAL_LIGHTS 10
#define LIGHTS_UNIFORM_BLOCK_BINDING_POINT	0	// TODO: This should come from the OpenGL program

class OGLLightsBlock : public OGLUniformBlock {
public:
	OGLLightsBlock(OpenGLImp *pParentImp);
	~OGLLightsBlock();

	RESULT GetUniformBlockBuffer(void *&pUniformBufferData, GLsizeiptr *pUniformBufferData_n);
	
	RESULT AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
	RESULT AddLight(light *pLight);
	RESULT ClearLights();

	RESULT SetLights(std::vector<light*> *pLights);

private:

	// This is the equivalent to the shader
	// Since the light virtual object has other stuff in there
	// TODO? Maybe move to VirtualObj as the standard

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	struct LightBlockLight {
		GLint type;
		GLfloat power;
		GLfloat shine;
		GLfloat reserved2;

		point ptOrigin;
		vector colorDiffuse;
		color colorSpecular;
		vector vectorDirection;
	};
#pragma pack(pop)

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */
	struct LightBlock {
		//light lights[MAX_TOTAL_LIGHTS];
		LightBlockLight lights[MAX_TOTAL_LIGHTS];
		GLint numActiveLights;
	} m_LightBlock;
#pragma pack(pop)

};

#endif // ! OGL_LIGHTS_BLOCK_H_