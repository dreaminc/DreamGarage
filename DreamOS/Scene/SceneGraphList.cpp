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


RESULT SceneGraphList::PushObject(VirtualObj *pObject) {
	m_objects.push_back(pObject);
	return R_PASS;
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