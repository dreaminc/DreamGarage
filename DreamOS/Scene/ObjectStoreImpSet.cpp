#include "ObjectStoreImpSet.h"

ObjectStoreImpSet::ObjectStoreImpSet() {

	ResetIterator();
}

ObjectStoreImpSet::~ObjectStoreImpSet() {
	// empty
}

RESULT ObjectStoreImpSet::ResetIterator() {
	m_objectIterator = m_objects.begin();
	return R_PASS;
}

VirtualObj *ObjectStoreImpSet::GetNextObject() {
	if (m_objectIterator == m_objects.end())
		return nullptr;

	VirtualObj *pVirtualObj = (*m_objectIterator);

	if (m_objectIterator != m_objects.end())
		m_objectIterator++;

	return pVirtualObj;
}

RESULT ObjectStoreImpSet::PushDimensionObject(DimObj *pDimObj) {
	return R_NOT_IMPLEMENTED;
}

RESULT ObjectStoreImpSet::PushObject(VirtualObj *pObject) {
	DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
	if (pObject != nullptr)
		return PushDimensionObject(pDimObj);
	
	return R_INVALID_OBJECT;
}

RESULT ObjectStoreImpSet::RemoveObject(VirtualObj *pObject) {
	for (std::set<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it) == pObject) {
			m_objects.erase(*it);
			return R_PASS;
		}
	return R_PASS;
}

RESULT ObjectStoreImpSet::RemoveObjectByUID(UID uid) {
	for (auto it = m_objects.begin(); it != m_objects.end(); )
		if ((*it)->getID() == uid) {
			m_objects.erase(*it);
			return R_PASS;
		}
	return R_NOT_FOUND;
}

VirtualObj *ObjectStoreImpSet::FindObjectByUID(UID uid) {
	for (std::set<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it)->getID() == uid) 
			return (*it);

	return nullptr;
}

VirtualObj *ObjectStoreImpSet::FindObject(VirtualObj *pObject) {
	for (std::set<VirtualObj*>::iterator it = m_objects.begin(); it != m_objects.end(); it++)
		if ((*it) == pObject)
			return (*it);

	return nullptr;
}