#ifndef SCENE_GRAPH_FLAT_LIST_H_
#define SCENE_GRAPH_FLAT_LIST_H_

#include "RESULT/EHM.h"
#include "Scene/ObjectStoreImpList.h"

class ObjectStoreImpFlatList : public ObjectStoreImpList {
public:
	ObjectStoreImpFlatList();
	~ObjectStoreImpFlatList();

public:

	RESULT PushObject(VirtualObj *pObject);

	RESULT PushLight(light *pLight) { return R_NOT_IMPLEMENTED; };
	RESULT GetLights(std::vector<light*>*& pLights) { return R_NOT_IMPLEMENTED; };

	RESULT SetSkybox(skybox *pSkybox) { return R_NOT_IMPLEMENTED; };
	RESULT GetSkybox(skybox*& pSkybox) { return R_NOT_IMPLEMENTED; };

	RESULT SetMaxZ(std::shared_ptr<DimObj> pObject);
	RESULT UpdateScene();

private:
	float m_minZ = 0.99f;
	float m_maxZ = -0.99f;
};

#endif // ! SCENE_GRAPH_LIST_H_