#include "SceneGraphFlatList.h"

SceneGraphFlatList::SceneGraphFlatList()
{
	ResetIterator();
}

SceneGraphFlatList::~SceneGraphFlatList()
{
	// empty
}

RESULT SceneGraphFlatList::PushObject(VirtualObj *pObject)
{
	return SceneGraphList::PushObject(pObject);
}
