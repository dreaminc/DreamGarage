#ifndef SCENE_GRAPH_STORE_H_
#define SCENE_GRAPH_STORE_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/SceneGraphStore.h
// This abstract class simply represents 

#include "Primitives/VirtualObj.h"

#include <vector>
#include "Primitives/light.h"

class SceneGraphStore {
public:
	virtual RESULT ResetIterator() = 0;
	virtual VirtualObj *GetNextObject() = 0;

	virtual RESULT PushObject(VirtualObj *pObject) = 0;
	virtual RESULT RemoveObject(VirtualObj *pObject) = 0;

	virtual RESULT RemoveObjectByUID(UID uid) = 0;
	virtual VirtualObj *FindObjectByUID(UID uid) = 0;
	virtual VirtualObj *FindObject(VirtualObj *pObject) = 0;

	virtual RESULT GetLights(std::vector<light*>*& pLights) = 0;
};

#endif // ! SCENE_GRAPH_STORE_H_