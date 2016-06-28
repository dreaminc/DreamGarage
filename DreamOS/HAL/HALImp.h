#ifndef HAL_IMP_H_
#define HAL_IMP_H_

// Dream OS
// DreamOS/HAL/HALImp.h
// The HAL Implementation class  is the parent class for implementations
// such as the OpenGL implementation and ultimately native ones as well

#include "./RESULT/EHM.h"
#include "Primitives/Types/UID.h"
#include "Primitives/Subscriber.h"

#include "Sense/SenseKeyboard.h"
#include "Sense/SenseMouse.h"

#include "Primitives/light.h"
#include "Primitives/sphere.h"
#include "Primitives/volume.h"

class HALImp : public Subscriber<SenseKeyboardEvent>, public Subscriber<SenseMouseEvent> {
public:
	HALImp() {
		// empty stub
	}

	~HALImp() {
		// empty stub
	}

public:
	virtual light* MakeLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection) = 0;
	virtual sphere* MakeSphere(float radius = 1.0f, int numAngularDivisions = 3, int numVerticalDivisions = 3) = 0;
	virtual volume* MakeVolume(double side) = 0;

	/*
	virtual texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type) = 0;
	virtual skybox MakeSkybox() = 0;
	virtual model* MakeModel(const std::vector<vertex>& vertices) = 0;
	*/

private:
	UID m_uid;
};

#endif // ! HAL_IMP_H_