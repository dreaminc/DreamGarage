#include "SceneGraphList.h"

SceneGraphList::SceneGraphList() {
	ResetIterator();
}

SceneGraphList::~SceneGraphList() {
	// empty
}

RESULT SceneGraphList::ResetIterator() {
	m_objectIterator = m_objects.begin();
	return R_PASS;
}

VirtualObj *SceneGraphList::GetNextObject() {
	if (m_objectIterator == m_objects.end())
		return NULL;

	VirtualObj *pVirtualObj = (*m_objectIterator);

	if (m_objectIterator != m_objects.end())
		m_objectIterator++;

	return pVirtualObj;
}


RESULT SceneGraphList::PushDimensionObject(DimObj *pDimObj) {
	m_objects.push_back(pDimObj);
	return R_PASS;
}

RESULT SceneGraphList::PushLight(light *pLight) {
	m_lights.push_back(pLight);
	return R_PASS;
}

RESULT SceneGraphList::PushObject(VirtualObj *pObject) {
	
	light *pLight = dynamic_cast<light*>(pObject);
	if (pLight != NULL)
		return PushLight(pLight);
	
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
	if (pObject != NULL)
		return PushDimensionObject(pDimObj);
	
	return R_INVALID_OBJECT;
}

RESULT SceneGraphList::RemoveObject(VirtualObj *pObject) {
	m_objects.remove(pObject);
	return R_PASS;
}

RESULT SceneGraphList::RemoveObjectByUID(UID uid) {
	RESULT r = R_PASS;

	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
		if ((*it)->getID() == uid) {
			m_objects.remove(*it);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

// Note: This memory location is not guaranteed and needs to be collected each time
// Caller should create a copy if needed
RESULT SceneGraphList::GetLights(std::vector<light*>*& pLights) {
	RESULT r = R_PASS;

	pLights = &(m_lights);

Error:
	return r;
}

VirtualObj *SceneGraphList::FindObjectByUID(UID uid) {
	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it)->getID() == uid) 
			return (*it);

	return NULL;
}

VirtualObj *SceneGraphList::FindObject(VirtualObj *pObject) {
	for (std::list<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it) == pObject)
			return (*it);

	return NULL;
}