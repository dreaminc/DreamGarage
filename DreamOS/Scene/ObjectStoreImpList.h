#ifndef SCENE_GRAPH_LIST_H_
#define SCENE_GRAPH_LIST_H_

#include "RESULT/EHM.h"

// DREAM OS
// DreamOS/Dimension/Scene/ObjectStoreImpList.h
// A simple ObjectStoreImp utilizing a linked list implementation

#include <list>
#include "ObjectStoreImp.h"

#include "Primitives/DimObj.h"

class ObjectStoreImpList : public ObjectStoreImp {
public:
	ObjectStoreImpList();
	~ObjectStoreImpList();

public:
	RESULT ResetIterator();
	VirtualObj *GetNextObject();

	RESULT PushDimensionObject(DimObj *pDimObj);
	RESULT PushLight(light *pLight);

	RESULT PushObject(VirtualObj *pObject);
	RESULT RemoveObject(VirtualObj *pObject);

	RESULT RemoveObjectByUID(UID uid);
	VirtualObj *FindObjectByUID(UID uid);
	VirtualObj *FindObject(VirtualObj *pObject);

	RESULT GetLights(std::vector<light*>*& pLights);

	RESULT SetSkybox(skybox *pSkybox);
	RESULT GetSkybox(skybox*& pSkybox);

private:
	std::list<VirtualObj*> m_objects;
	
	std::vector<light*> m_lights;		

	skybox *m_pSkybox;

	std::list<VirtualObj*>::iterator m_objectIterator;

};

#endif // ! SCENE_GRAPH_LIST_H_