#ifndef PRIM_PARAMS_H_
#define PRIM_PARAMS_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/PrimParams.h
// PrimParams is a basic interface for primitive params that is used 
// for a more general way to create objects 

enum class PRIMITIVE_TYPE {
	SPHERE,
	VOLUME,
	QUAD,
	TEXTURE,
	INVALID
};

struct PrimParams {
	virtual PRIMITIVE_TYPE GetPrimitiveType() = 0;
};

#endif // ! PRIM_PARAMS_H_