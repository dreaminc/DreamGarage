#include "SceneGraph.h"
#include "SceneGraphList.h"

SceneGraph::SceneGraph(SCENE_GRAPH_STORE_TYPE type) {
	RESULT r = R_PASS;

	CRM(InitializeSceneGraphStore(type), "Failed to initialize scene graph store");

	Validate();
	return;
Error:
	Invalidate();
	return;
}

SceneGraph::SceneGraph() :
	SceneGraph(SCENE_GRAPH_STORE_LIST)
{
	// empty 
}

SceneGraph::~SceneGraph() {
	// empty
}

RESULT SceneGraph::InitializeSceneGraphStore(SCENE_GRAPH_STORE_TYPE type) {
	RESULT r = R_PASS;

	CBM((m_pSceneGraphStore == NULL), "Scene Graph Store already initialized");
	CBM((type < SCENE_GRAPH_STORE_INVALID), "Scene graph type invalid");

	switch (type) {
		case SCENE_GRAPH_STORE_LIST: {
			m_pSceneGraphStore = new SceneGraphList();
			CNM(m_pSceneGraphStore, "Failed to allocate scene graph store list");
		} break;
	}

Error:
	return r;
}

RESULT SceneGraph::Reset() {
	return m_pSceneGraphStore->ResetIterator();
}

RESULT SceneGraph::PushObject(DimObj *pObject) {
	return m_pSceneGraphStore->PushObject(pObject);
}

RESULT SceneGraph::RemoveObject(DimObj *pObject) {
	RESULT r = R_NOT_IMPLEMENTED;

	CN(m_pSceneGraphStore);
	CR(m_pSceneGraphStore->RemoveObject(pObject));

Error:
	return r;
}

RESULT SceneGraph::RemoveObjectByUID(UID uid) {
	RESULT r = R_NOT_IMPLEMENTED;

	CN(m_pSceneGraphStore);
	CR(m_pSceneGraphStore->RemoveObjectByUID(uid));

Error:
	return r;
}

DimObj *SceneGraph::FindObjectByUID(UID uid) {
	RESULT r = R_PASS;

	CN(m_pSceneGraphStore);

	return m_pSceneGraphStore->FindObjectByUID(uid);
Error:
	return NULL;
}