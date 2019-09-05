#ifndef FOG_PROGRAM_H 
#define FOG_PROGRAM_H

#include "core/ehm/EHM.h"

// Dream HAL
// dos/src/hal/FogProgram.h

#include "FogParams.h"

#include "core/types/DObject.h"

class color;

class FogProgram : public DObject {
public:
	RESULT SetFogParams(FogParams fogParams) {
		m_fogParams = fogParams;
		return R_PASS;
	}

	FogParams m_fogParams;
};

#endif // ! FOG_PROGRAM_H_
