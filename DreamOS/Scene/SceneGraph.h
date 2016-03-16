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

// The different types of stores should be added here 
enum SCENE_GRAPH_STORE_TYPE {
	SCENE_GRAPH_STORE_LIST,
	SCENE_GRAPH_STORE_INVALID
};

class SceneGraph : public valid {
public:

	SceneGraph(SCENE_GRAPH_STORE_TYPE type);
	
	SceneGraph();
	~SceneGraph();

	RESULT Reset();

	RESULT PushObject(DimObj *pObject);
	RESULT RemoveObject(DimObj *pObject);

	// Effectively a factory method to set up the object store
	RESULT InitializeSceneGraphStore(SCENE_GRAPH_STORE_TYPE type);

	RESULT RemoveObjectByUID(UID uid);
	DimObj *FindObjectByUID(UID uid);

	SceneGraphStore *GetSceneGraphStore() { return m_pSceneGraphStore; }

	// TODO: Arch here
	RESULT UpdateScene();
	RESULT LoadScene();

private:
	SceneGraphStore *m_pSceneGraphStore;
	UID m_uid;
};

#endif // ! SCENE_GRAPH_H_