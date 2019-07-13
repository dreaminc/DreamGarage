#include "ObjectStoreFactory.h"

#include "ObjectStoreImpList.h"

ObjectStoreImp* ObjectStoreFactory::MakeObjectStore(ObjectStoreFactory::TYPE type) {
	RESULT r = R_PASS;
	ObjectStoreImp* pObjectStore = nullptr;

	switch (type) {
	case TYPE::LIST: {
		pObjectStore = new ObjectStoreImpList();
		CNM(pObjectStore, "Failed to initialize object store!");
	} break;

		case TYPE::INVALID:
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