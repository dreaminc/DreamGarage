#ifndef SCENE_GRAPH_FLAT_LIST_H_
#define SCENE_GRAPH_FLAT_LIST_H_

#include "RESULT/EHM.h"
#include "Scene/SceneGraphList.h"

class SceneGraphFlatList : public SceneGraphList {
public:
	SceneGraphFlatList();
	~SceneGraphFlatList();

public:

	RESULT PushObject(VirtualObj *pObject);

	RESULT PushLight(light *pLight) { return R_NOT_IMPLEMENTED; };
	RESULT GetLights(std::vector<light*>*& pLights) { return R_NOT_IMPLEMENTED; };

	RESULT SetSkybox(skybox *pSkybox) { return R_NOT_IMPLEMENTED; };
	RESULT GetSkybox(skybox*& pSkybox) { return R_NOT_IMPLEMENTED; };
};


#endif // ! SCENE_GRAPH_LIST_H_