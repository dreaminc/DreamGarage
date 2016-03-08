#ifndef SCENE_GRAPH_LIST_H_
#define SCENE_GRAPH_LIST_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/SceneGraphList.h
// A simple SceneGraphStore utilizing a linked list implementation

#include <list>

#include "SceneGraphStore.h"

class SceneGraphList : public SceneGraphStore {
public:
	SceneGraphList();
	~SceneGraphList();

public:
	RESULT ResetIterator();
	DimObj *GetNextObject();

	RESULT PushObject(DimObj *pObject);
	RESULT RemoveObject(DimObj *pObject);

	RESULT RemoveObjectByUID(UID uid);
	DimObj *FindObjectByUID(UID uid);
	DimObj *FindObject(DimObj *pObject);

private:
	std::list<DimObj*> m_objects;
	std::list<DimObj*>::iterator m_objectIterator;

};

#endif // ! SCENE_GRAPH_LIST_H_