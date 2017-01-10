#include "ObjectStoreImpList.h"
#include "Primitives/ray.h"

ObjectStoreImpList::ObjectStoreImpList() :
	m_pSkybox(nullptr)
{
	ResetIterator();
}

ObjectStoreImpList::~ObjectStoreImpList() {
	// empty
}

RESULT ObjectStoreImpList::ResetIterator() {
	m_objectIterator = m_objects.begin();
	return R_PASS;
}

VirtualObj *ObjectStoreImpList::GetNextObject() {
	if (m_objectIterator == m_objects.end())
		return nullptr;

	VirtualObj *pVirtualObj = (*m_objectIterator);

	if (m_objectIterator != m_objects.end()) {
		m_objectIterator++;
	}

	return pVirtualObj;
}


RESULT ObjectStoreImpList::PushDimensionObject(DimObj *pDimObj) {
	m_objects.push_back(pDimObj);
	return R_PASS;
}

RESULT ObjectStoreImpList::PushLight(light *pLight) {
	m_lights.push_back(pLight);
	return R_PASS;
}

RESULT ObjectStoreImpList::PushObject(VirtualObj *pObject) {
	
	light *pLight = dynamic_cast<light*>(pObject);
	if (pLight != nullptr)
		return PushLight(pLight);

	skybox *pSkybox = dynamic_cast<skybox*>(pObject);
	if (pSkybox != nullptr)
		return SetSkybox(pSkybox);
	
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
	if (pObject != nullptr)
		return PushDimensionObject(pDimObj);
	
	return R_INVALID_OBJECT;
}

RESULT ObjectStoreImpList::RemoveObject(VirtualObj *pObject) {
	m_objects.remove(pObject);
	return R_PASS;
}

RESULT ObjectStoreImpList::RemoveObjectByUID(UID uid) {
	RESULT r = R_PASS;

	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
		if ((*it)->getID() == uid) {
			m_objects.remove(*it);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

RESULT ObjectStoreImpList::CommitObjects() {
	for (auto &obj : m_objects) {
		obj->CommitPendingTranslation();
		obj->CommitPendingImpulses();
	}
	return R_SUCCESS;
}

// Note: This memory location is not guaranteed and needs to be collected each time
// Caller should create a copy if needed
RESULT ObjectStoreImpList::GetLights(std::vector<light*>*& pLights) {
	RESULT r = R_PASS;

	pLights = &(m_lights);
//Error:
	return r;
}

RESULT ObjectStoreImpList::SetSkybox(skybox *pSkybox) {
	m_pSkybox = pSkybox;
	return R_PASS;
}

RESULT ObjectStoreImpList::GetSkybox(skybox*& pSkybox) {
	pSkybox = m_pSkybox;
	return R_PASS;
}

VirtualObj *ObjectStoreImpList::FindObjectByUID(UID uid) {
	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it)->getID() == uid) 
			return (*it);

	return nullptr;
}

VirtualObj *ObjectStoreImpList::FindObject(VirtualObj *pObject) {
	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it) == pObject)
			return (*it);

	return nullptr;
}

std::vector<VirtualObj*> ObjectStoreImpList::GetObjects() {
	std::vector<VirtualObj*> objects = { std::begin(m_objects), std::end(m_objects) };
	return objects;
}

std::vector<VirtualObj*> ObjectStoreImpList::GetObjects(const ray &rCast) {
	std::vector<VirtualObj*> intersectedObjects;

	for (auto &object: m_objects) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(object);
		
		if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr) {
			continue; 
		}

		if (pDimObj->GetBoundingVolume()->Intersect(rCast)) {
			intersectedObjects.push_back(pDimObj);
		}
	}

	return intersectedObjects;
}

std::vector<VirtualObj*> ObjectStoreImpList::GetObjects(DimObj *pDimObj) {
	std::vector<VirtualObj*> intersectedObjects;

	for (auto &pObject : m_objects) {
		DimObj *pDimObject = dynamic_cast<DimObj*>(pObject);

		// Don't intersect self
		if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr || pDimObj == pDimObject || pDimObject->GetBoundingVolume() == nullptr) {
			continue;
		}

		if (pDimObject->GetBoundingVolume()->Intersect(pDimObj->GetBoundingVolume().get())) {
			intersectedObjects.push_back(pDimObject);
		}
	}

	return intersectedObjects;
}

// TODO: This will return redundant groups right now
std::vector<std::vector<VirtualObj*>> ObjectStoreImpList::GetObjectCollisionGroups() {
	std::vector<std::vector<VirtualObj*>> collisionGroups;

	for (auto &object : m_objects) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(object);

		if (pDimObj == nullptr || pDimObj->GetBoundingVolume() == nullptr) {
			continue;
		}

		///*
		// Check that this object is not already in a collision group
		bool fObjectDuplicate = false;
		for (auto &group : collisionGroups) {
			for (auto &pObj : group) {
				if (pDimObj == pObj) {
					fObjectDuplicate = true;
					break;
				}
			}
			if (fObjectDuplicate)
				break;
		}
		// Object already in a collision group - so continue
		if (fObjectDuplicate)
			continue;
		//*/

		auto collisionGroup = GetObjects(pDimObj);

		if (collisionGroup.size() > 0) {
			collisionGroup.push_back(pDimObj);
			collisionGroups.push_back(collisionGroup);
		}

		// TODO: We need to add logic to remove double hits / ensure unique groups
	}

	return collisionGroups;
}