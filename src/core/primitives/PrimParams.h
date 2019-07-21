#ifndef PRIM_PARAMS_H_
#define PRIM_PARAMS_H_

#include "core/ehm/EHM.h"

// DREAM OS
// DreamOS/Primitives/PrimParams.h
// PrimParams is a basic interface for primitive params that is used 
// for a more general way to create objects 

enum class PRIMITIVE_TYPE {
	SPHERE,
	VOLUME,
	QUAD,
	TEXTURE,
	MODEL,
	MESH,
	INVALID
};

struct PrimParams {
	virtual PRIMITIVE_TYPE GetPrimitiveType() = 0;
};

#endif // ! PRIM_PARAMS_H_