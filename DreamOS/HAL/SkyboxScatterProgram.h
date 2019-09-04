#ifndef SKYBOX_SCATTER_PROGRAM_H_
#define SKYBOX_SCATTER_PROGRAM_H_

#include "./RESULT/EHM.h"

class SkyboxScatterProgram {
public:
	virtual RESULT SetSunDirection(vector vSunDirection) = 0;
};

#endif // ! SKYBOX_SCATTER_PROGRAM_H_