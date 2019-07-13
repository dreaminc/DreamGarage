#ifndef ENVIRONMENT_PROGRAM_H_
#define ENVIRONMENT_PROGRAM_H_

#include "core/ehm/EHM.h"

// Dream HAL 
// dos/src/hal/EnvironmentProgram.h

#include "core/types/DObject.h"

class EnvironmentProgram : public DObject {
public:
	virtual RESULT SetIsAugmented(bool fAugmented) = 0;

};

#endif ENVIRONMENT_PROGRAM_H_