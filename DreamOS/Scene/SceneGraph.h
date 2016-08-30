#ifndef SCENE_GRAPH_H_
#define SCENE_GRAPH_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/SceneGraph.h
// The Scene Graph contains all of the objects in a given
// scene and manages what is dispatched to the graphics pipeline
// or processed by the physics engine

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "SceneGraphStore.h"

#include "ObjectStoreFactory.h"

// The different types of stores should be added here 

class SceneGraph : public valid {
public:

	SceneGraph(OBJECT_STORE_TYPE type);
	
	SceneGraph();
	~SceneGraph();

	RESULT Reset();

	RESULT PushObject(VirtualObj *pObject);
	RESULT RemoveObject(VirtualObj *pObject);

	// Effectively a factory method to set up the object store
	RESULT InitializeSceneGraphStore(OBJECT_STORE_TYPE type);

	RESULT RemoveObjectByUID(UID uid);
	VirtualObj *FindObjectByUID(UID uid);

	SceneGraphStore *GetSceneGraphStore() { return m_pSceneGraphStore; }

	// TODO: Arch here
	RESULT UpdateScene();
	RESULT LoadScene();

protected:
	SceneGraphStore *m_pSceneGraphStore;
private:
	UID m_uid;
};

#endif // ! SCENE_GRAPH_H_