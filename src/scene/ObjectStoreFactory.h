#ifndef OBJECT_STORE_FACTORY_H_
#define OBJECT_STORE_FACTORY_H_

#include "RESULT/EHM.h"
#include "Scene/ObjectStoreImp.h"
#include "Core/Types/Factory.h"

class ObjectStoreFactory : public Factory {
public:
	enum class TYPE {
		LIST,
		INVALID
	} ;

public:
	static ObjectStoreImp* MakeObjectStore(ObjectStoreFactory::TYPE type);
};

#endif // ! OBJECT_STORE_FACTORY_H_
