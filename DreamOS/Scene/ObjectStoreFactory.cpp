#include "ObjectStoreFactory.h"

#include "SceneGraphList.h"
#include "SceneGraphFlatList.h"

SceneGraphStore* ObjectStoreFactory::MakeObjectStore(OBJECT_STORE_TYPE type) {
	RESULT r = R_PASS;
	SceneGraphStore* pObjectStore = nullptr;

	switch (type) {
	case OBJECT_STORE_LIST: {
		pObjectStore = new SceneGraphList();
		CNM(pObjectStore, "Failed to initialize object store!");
	} break;

	case OBJECT_STORE_FLAT: {
		pObjectStore = new SceneGraphFlatList();
		CNM(pObjectStore, "Failed to initialize object store!");
	} break;

	default: {
		pObjectStore = nullptr;
		DEBUG_LINEOUT("Object store type %d not supported", type);
	} break;
	}
	return pObjectStore;

Error:
	if (pObjectStore = nullptr) {
		delete pObjectStore;
		pObjectStore = nullptr;
	}
	return nullptr;
}