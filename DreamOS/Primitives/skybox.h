#ifndef SKY_BOX_H_
#define SKY_BOX_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Primitives/skybox.h
// Skybox Primitive

#include "volume.h"

#define DEFAULT_SKYBOX_SIZE 100.0f;

class skybox : public volume {
public:
	skybox() :
		volume(DEFAULT_SKYBOX_SIZE)
	{
		// empty
	}
};

#endif	// ! SKY_BOX_H_
