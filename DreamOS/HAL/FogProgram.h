#ifndef FOG_PROGRAM_H 
#define FOG_PROGRAM_H

#include "./RESULT/EHM.h"
#include "Primitives/fogparams.h"

class color;

class FogProgram {
public:
	RESULT SetFogParams(fogparams fogParams) {
		m_fogParams = fogParams;
		return R_PASS;
	}

	fogparams m_fogParams;
};

#endif // ! FOG_PROGRAM_H_
