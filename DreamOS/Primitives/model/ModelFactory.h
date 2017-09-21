#ifndef MODEL_FACTORY_H_
#define MODEL_FACTORY_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Primitives/model/ModelFactory.h

#include <string>

class model;
class HALImp;

class ModelFactory  {
public:
	static model* MakeModel(HALImp *pParentImp, std::wstring wstrModelFilename);
};

#endif // ! MODEL_FACTORY_H_