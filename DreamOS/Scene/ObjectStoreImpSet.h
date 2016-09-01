#ifndef OBJECT_STORE_IMP_SET_H_
#define OBJECT_STORE_IMP_SET_H_

#include "RESULT/EHM.h"
#include "ObjectStoreImp.h"
#include "Primitives/DimObj.h"

#include <set>

class ObjectStoreImpSet : public ObjectStoreImp {
public:
	ObjectStoreImpSet();
	~ObjectStoreImpSet();

public:
	RESULT ResetIterator();
	VirtualObj *GetNextObject();

	RESULT PushDimensionObject(DimObj *pDimObj);
	RESULT PushLight(light *pLight) { return R_NOT_IMPLEMENTED; };

	RESULT PushObject(VirtualObj *pObject);
	RESULT RemoveObject(VirtualObj *pObject);

	RESULT RemoveObjectByUID(UID uid);
	VirtualObj *FindObjectByUID(UID uid);
	VirtualObj *FindObject(VirtualObj *pObject);

	RESULT GetLights(std::vector<light*>*& pLights){ return R_NOT_IMPLEMENTED; };

	RESULT SetSkybox(skybox *pSkybox) { return R_NOT_IMPLEMENTED; };
	RESULT GetSkybox(skybox*& pSkybox) { return R_NOT_IMPLEMENTED; };

private:
	struct cmp {
		bool operator()(VirtualObj* const a, VirtualObj* const b) const {
			return a->GetOrigin().z() < b->GetOrigin().z();
		}
	};

	std::set<VirtualObj *, cmp> m_objects;

	std::set<VirtualObj *, cmp>::iterator m_objectIterator;
};





#endif // ! OBJECT_STORE_IMP_SET_H_