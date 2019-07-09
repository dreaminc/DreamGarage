#ifndef ENVIRONMENT_PROGRAM_H_
#define ENVIRONMENT_PROGRAM_H_

#include "./RESULT/EHM.h"

class EnvironmentProgram {
public:
	virtual RESULT SetIsAugmented(bool fAugmented) = 0;

};

#endif ENVIRONMENT_PROGRAM_H_