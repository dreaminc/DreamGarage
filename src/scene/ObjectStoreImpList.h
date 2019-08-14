#ifndef SCENE_GRAPH_LIST_H_
#define SCENE_GRAPH_LIST_H_

#include "core/ehm/EHM.h"

// Dream Scene
// dos/src/scene/ObjectStoreImpList.h

// A simple ObjectStoreImp utilizing a linked list implementation

#include <list>

#include "ObjectStoreImp.h"

#include "core/dimension/DimObj.h"

class ObjectStoreImpList : public ObjectStoreImp {
public:
	ObjectStoreImpList();
	~ObjectStoreImpList();

public:
	RESULT ResetIterator();
	VirtualObj *GetNextObject();

	RESULT PushDimensionObject(DimObj *pDimObj);
	RESULT PushLight(light *pLight);

	virtual RESULT PushObject(VirtualObj *pObject) override;
	virtual RESULT RemoveObject(VirtualObj *pObject) override;

	virtual RESULT RemoveAllObjects() override;
	virtual RESULT RemoveObjectByUID(UID uid) override;
	virtual VirtualObj *FindObjectByUID(UID uid) override;
	virtual VirtualObj *FindObject(VirtualObj *pObject) override;

	virtual RESULT CommitObjects() override;

	virtual RESULT GetLights(std::vector<light*>*& pLights) override;

	RESULT SetSkybox(skybox *pSkybox);
	RESULT GetSkybox(skybox*& pSkybox);

	virtual std::list<VirtualObj*> GetList() override;
	virtual std::vector<VirtualObj*> GetObjects() override;
	virtual std::vector<VirtualObj*> GetObjects(const ray &rCast) override;
	virtual std::vector<VirtualObj*> GetObjects(DimObj *pDimObj) override;
	virtual std::vector<std::vector<VirtualObj*>> GetObjectCollisionGroups() override;

protected:
	std::list<VirtualObj*> m_objects;
private:	
	std::vector<light*> m_lights;		

	skybox *m_pSkybox;

	std::list<VirtualObj*>::iterator m_objectIterator;

};

#endif // ! SCENE_GRAPH_LIST_H_