#ifndef OBJECT_STORE_FACTORY_H_
#define OBJECT_STORE_FACTORY_H_

#include "RESULT/EHM.h"
#include "Scene/ObjectStoreImp.h"
#include "Core/Types/Factory.h"


typedef enum {
	OBJECT_STORE_LIST,
	OBJECT_STORE_FLAT,
	OBJECT_STORE_INVALID
} OBJECT_STORE_TYPE;

class ObjectStoreFactory : public Factory {
public:
	static ObjectStoreImp* MakeObjectStore(OBJECT_STORE_TYPE type);
};

#endif // ! OBJECT_STORE_FACTORY_H_
