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

DimObj *SceneGraphList::GetNextObject() {
	if (m_objectIterator == m_objects.end())
		return NULL;

	DimObj *pDimObj = (*m_objectIterator);

	if (m_objectIterator != m_objects.end())
		m_objectIterator++;

	return pDimObj;
}


RESULT SceneGraphList::PushObject(DimObj *pObject) {
	m_objects.push_back(pObject);
	return R_PASS;
}

RESULT SceneGraphList::RemoveObject(DimObj *pObject) {
	m_objects.remove(pObject);
	return R_PASS;
}

RESULT SceneGraphList::RemoveObjectByUID(UID uid) {
	RESULT r = R_PASS;

	for (std::list<DimObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++) {
		if ((*it)->getID() == uid) {
			m_objects.remove(*it);
			return R_PASS;
		}
	}

	return R_NOT_FOUND;
}

DimObj *SceneGraphList::FindObjectByUID(UID uid) {
	for (std::list<DimObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it)->getID() == uid) 
			return (*it);

	return NULL;
}

DimObj *SceneGraphList::FindObject(DimObj *pObject) {
	for (std::list<DimObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it) == pObject)
			return (*it);

	return NULL;
}