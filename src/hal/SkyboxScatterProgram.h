#ifndef SKYBOX_SCATTER_PROGRAM_H_
#define SKYBOX_SCATTER_PROGRAM_H_

#include "core/ehm/EHM.h"

// Dream HAL 
// dos/src/hal/SkyboxScatterProgram

#include "core/types/DObject.h"

// TODO: There might be better ways to do this now

class SkyboxScatterProgram : public DObject {
public:
	virtual RESULT SetSunDirection(vector vSunDirection) = 0;
};

#endif // ! SKYBOX_SCATTER_PROGRAM_H_