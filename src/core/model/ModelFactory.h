#ifndef MODEL_FACTORY_H_
#define MODEL_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream Core
// dos/src/core/model/ModelFactory.h

#include <string>

#include "core/primitives/PrimParams.h"

class model;
class HALImp;
class DreamOS;

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
	static model* MakeModel(DreamOS *pDOS, PrimParams *pPrimParams, bool fInitialize = false);
};

#endif // ! MODEL_FACTORY_H_