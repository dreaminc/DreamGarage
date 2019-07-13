#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include "RESULT/EHM.h"

#include <mutex>

// DREAM OS
// DreamOS/Dimension/Scene/ObjectStore.h
// The Scene Graph contains all of the objects in a given
// scene and manages what is dispatched to the graphics pipeline
// or processed by the physics engine

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "ObjectStoreImp.h"

#include "ObjectStoreFactory.h"

class ray;

// The different types of stores should be added here 

class ObjectStore : public valid {
public:

	ObjectStore(ObjectStoreFactory::TYPE type);
	
	ObjectStore();
	~ObjectStore();

	RESULT Reset();

	RESULT PushObject(VirtualObj *pObject, bool fForce = false);
	RESULT RemoveObject(VirtualObj *pObject);
	VirtualObj *FindObject(VirtualObj *pObject);
	RESULT RemoveAllObjects();

	// Effectively a factory method to set up the object store
	RESULT InitializeSceneGraphStore(ObjectStoreFactory::TYPE type);

	RESULT RemoveObjectByUID(UID uid);
	VirtualObj *FindObjectByUID(UID uid);

	ObjectStoreImp *GetSceneGraphStore() { 
		return m_pSceneGraphStore; 
	}

	std::vector<VirtualObj*> GetObjects();
	std::vector<VirtualObj*> GetObjects(ray rCast);

	// TODO: Arch here
	RESULT UpdateScene();
	RESULT LoadScene();

	RESULT CommitObjects();

	RESULT LockStore();
	RESULT UnlockStore();
	bool TryLockStore();


protected:
	ObjectStoreImp *m_pSceneGraphStore;

private:
	UID m_uid;

	//std::mutex m_objectStoreLock;
	std::recursive_mutex m_objectStoreLock;
};

#endif // ! SCENE_GRAPH_H_