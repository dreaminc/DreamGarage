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

RESULT SceneGraph::PushObject(VirtualObj *pObject) {
	return m_pSceneGraphStore->PushObject(pObject);
}

RESULT SceneGraph::RemoveObject(VirtualObj *pObject) {
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

VirtualObj *SceneGraph::FindObjectByUID(UID uid) {
	RESULT r = R_PASS;

	CN(m_pSceneGraphStore);

	return m_pSceneGraphStore->FindObjectByUID(uid);
Error:
	return NULL;
}

// TODO: Perhaps pass this to a scene graph handler (like physics etc)
RESULT SceneGraph::UpdateScene() {
	RESULT r = R_PASS;
	
	/*
	Reset();
	SceneGraphStore *pObjectStore = GetSceneGraphStore();

	VirtualObj *pVirtualObj = NULL;
	while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {
		quaternion_precision factor = 0.05;
		quaternion_precision filter = 0.1;

		static quaternion_precision x = 0;
		static quaternion_precision y = 0;
		static quaternion_precision z = 0;

		//x = ((1.0f - filter) * x) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//y = ((1.0f - filter) * y) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//z = ((1.0f - filter) * z) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));

		pVirtualObj->RotateBy(x * factor, y * factor, z * factor);
	}
	//*/

	return r;
}

