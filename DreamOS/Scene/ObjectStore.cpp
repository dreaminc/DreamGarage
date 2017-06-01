#include "ObjectStore.h"
#include "ObjectStoreImpList.h"

#include "Primitives/ray.h"

ObjectStore::ObjectStore(ObjectStoreFactory::TYPE type) {
	RESULT r = R_PASS;

	CRM(InitializeSceneGraphStore(type), "Failed to initialize scene graph store");

	Validate();
	return;
Error:
	Invalidate();
	return;
}

ObjectStore::ObjectStore() :
	ObjectStore(ObjectStoreFactory::TYPE::LIST)
{
	// empty 
}

ObjectStore::~ObjectStore() {
	// empty
}

RESULT ObjectStore::InitializeSceneGraphStore(ObjectStoreFactory::TYPE type) {
	RESULT r = R_PASS;

	CBM((m_pSceneGraphStore == NULL), "Scene Graph Store already initialized");

	m_pSceneGraphStore = ObjectStoreFactory::MakeObjectStore(type);

Error:
	return r;
}

RESULT ObjectStore::Reset() {
	return m_pSceneGraphStore->ResetIterator();
}

RESULT ObjectStore::PushObject(VirtualObj *pObject, bool fForce) {
	RESULT r = R_PASS;

	// Skip if already in store

	// Force will skip this allowing for duplicates or however the object store imp
	// handles duplicates 

	if (fForce == false) {
		CBR((m_pSceneGraphStore->FindObject(pObject) == nullptr), R_SKIPPED);
	}

	// Push otherwise
	CR(m_pSceneGraphStore->PushObject(pObject));

Error:
	return r;
}

RESULT ObjectStore::LockStore() {
	m_objectStoreLock.lock();
	return R_PASS;
}

bool ObjectStore::TryLockStore() {
	return m_objectStoreLock.try_lock();
}

RESULT ObjectStore::UnlockStore() {
	m_objectStoreLock.unlock();
	return R_PASS;
}

RESULT ObjectStore::RemoveObject(VirtualObj *pObject) {
	RESULT r = R_NOT_IMPLEMENTED;

	CN(m_pSceneGraphStore);
	
	CR(m_pSceneGraphStore->RemoveObject(pObject));

Error:
	return r;
}

VirtualObj* ObjectStore::FindObject(VirtualObj *pObject) {
	return m_pSceneGraphStore->FindObject(pObject);
}

RESULT ObjectStore::RemoveAllObjects() {
	RESULT r = R_NOT_IMPLEMENTED;	

	CR(m_pSceneGraphStore->RemoveAllObjects());

Error:
	return r;
}

RESULT ObjectStore::RemoveObjectByUID(UID uid) {
	RESULT r = R_NOT_IMPLEMENTED;

	CN(m_pSceneGraphStore);

	CR(m_pSceneGraphStore->RemoveObjectByUID(uid));

Error:
	return r;
}

VirtualObj *ObjectStore::FindObjectByUID(UID uid) {
	RESULT r = R_PASS;

	CN(m_pSceneGraphStore);

	return m_pSceneGraphStore->FindObjectByUID(uid);
Error:
	return NULL;
}

std::vector<VirtualObj*> ObjectStore::GetObjects() {
	return m_pSceneGraphStore->GetObjects();
}

std::vector<VirtualObj*> ObjectStore::GetObjects(ray rCast) {
	return m_pSceneGraphStore->GetObjects(rCast);
}

RESULT ObjectStore::GetMinMaxPoint(point *pPtMax, point *pPtMin) {
	return m_pSceneGraphStore->GetMinMaxPoint(pPtMax, pPtMin);
}

// TODO: This is holding the collide functionality here temporarily 
RESULT ObjectStore::UpdateScene() {
	return R_NOT_IMPLEMENTED;
}

RESULT ObjectStore::CommitObjects() {
	return m_pSceneGraphStore->CommitObjects();
}

