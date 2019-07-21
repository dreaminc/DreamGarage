#ifndef OBJECT_STORE_FACTORY_H_
#define OBJECT_STORE_FACTORY_H_

#include "core/ehm/EHM.h"

// Dream Scene
// dos/src/scene/ObjectStoreFactory.h

// A factory object to generate object stores

#include "scene/ObjectStoreImp.h"
#include "core/types/Factory.h"

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
