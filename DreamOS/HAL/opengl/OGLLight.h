#ifndef OGL_LIGHT_H
#define OGL_LIGHT_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/HAL/OpenGL/OGLLight.h
// OpenGL Light Object - effectively responsible for passing the light data to the OGLImp

// Note: The architecture is different here than for DimObj/OGLObject as Lights are very specific 
// virtual objects being passed to specific targets on the GPU side

#include "Primitives/light.h"

class OGLLight : public light {
public:
	OGLLight() :
		light()
	{
		// empty
	}

	OGLLight(LIGHT_TYPE type, light_precision intensity, color colorDiffuse, color colorSpecular, point ptOrigin, vector vectorDirection) :
		light(type, intensity, colorDiffuse, colorSpecular, ptOrigin, vectorDirection)
	{
		// empty
	}

	~OGLLight() {
		// empty
	}
};

#endif // ! OGL_LIGHT_H_