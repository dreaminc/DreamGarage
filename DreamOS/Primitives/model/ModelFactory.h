#ifndef MODEL_FACTORY_H_
#define MODEL_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/ModelFactory.h

#include <string>

#include "Primitives/PrimParams.h"

class model;
class HALImp;

class ModelFactory  {
public:
	enum flags : uint32_t {
		NONE						= 0,
		PRETRANSFORM_VERTICES		= 1 << 0,
		FLIP_WINDING				= 1 << 1,
		INVALID						= 0xFFFFFFFF
	};

public:
	static model* MakeModel(HALImp *pParentImp, std::wstring wstrModelFilename, ModelFactory::flags modelFactoryFlags = ModelFactory::flags::NONE);
	static model* MakeModel(PrimParams *pPrimParams, bool fInitialize = false);
};

#endif // ! MODEL_FACTORY_H_