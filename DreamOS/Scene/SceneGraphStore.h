#ifndef SCENE_GRAPH_STORE_H_
#define SCENE_GRAPH_STORE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/SceneGraphStore.h
// This abstract class simply represents 

#include "Primitives/DimObj.h"

class SceneGraphStore {
public:

	virtual RESULT ResetIterator() = 0;
	virtual DimObj *GetNextObject() = 0;

	virtual RESULT PushObject(DimObj *pObject) = 0;
	virtual RESULT RemoveObject(DimObj *pObject) = 0;

	virtual RESULT RemoveObjectByUID(UID uid) = 0;
	virtual DimObj *FindObjectByUID(UID uid) = 0;
	virtual DimObj *FindObject(DimObj *pObject) = 0;
};

#endif // ! SCENE_GRAPH_STORE_H_