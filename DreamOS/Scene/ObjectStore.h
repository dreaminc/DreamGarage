#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/ObjectStore.h
// The Scene Graph contains all of the objects in a given
// scene and manages what is dispatched to the graphics pipeline
// or processed by the physics engine

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "ObjectStoreImp.h"

#include "ObjectStoreFactory.h"

// The different types of stores should be added here 

class ObjectStore : public valid {
public:

	ObjectStore(OBJECT_STORE_TYPE type);
	
	ObjectStore();
	~ObjectStore();

	RESULT Reset();

	RESULT PushObject(VirtualObj *pObject);
	RESULT RemoveObject(VirtualObj *pObject);

	// Effectively a factory method to set up the object store
	RESULT InitializeSceneGraphStore(OBJECT_STORE_TYPE type);

	RESULT RemoveObjectByUID(UID uid);
	VirtualObj *FindObjectByUID(UID uid);

	ObjectStoreImp *GetSceneGraphStore() { return m_pSceneGraphStore; }

	// TODO: Arch here
	RESULT UpdateScene();
	RESULT LoadScene();

protected:
	ObjectStoreImp *m_pSceneGraphStore;
private:
	UID m_uid;
};

#endif // ! SCENE_GRAPH_H_