#ifndef FOG_PROGRAM_H 
#define FOG_PROGRAM_H

#include "./RESULT/EHM.h"
#include "Primitives/FogParams.h"

class color;

class FogProgram {
public:
	RESULT SetFogParams(FogParams fogParams) {
		m_fogParams = fogParams;
		return R_PASS;
	}

	FogParams m_fogParams;
};

#endif // ! FOG_PROGRAM_H_
