#include "ObjectStoreImpFlatList.h"

ObjectStoreImpFlatList::ObjectStoreImpFlatList()
{
	ResetIterator();
}

ObjectStoreImpFlatList::~ObjectStoreImpFlatList()
{
	// empty
}

RESULT ObjectStoreImpFlatList::PushObject(VirtualObj *pObject)
{
	return ObjectStoreImpList::PushObject(pObject);
}
