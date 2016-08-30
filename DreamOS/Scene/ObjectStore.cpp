#include "ObjectStore.h"
#include "ObjectStoreImpList.h"

ObjectStore::ObjectStore(OBJECT_STORE_TYPE type) {
	RESULT r = R_PASS;

	CRM(InitializeSceneGraphStore(type), "Failed to initialize scene graph store");

	Validate();
	return;
Error:
	Invalidate();
	return;
}

ObjectStore::ObjectStore() :
	ObjectStore(OBJECT_STORE_LIST)
{
	// empty 
}

ObjectStore::~ObjectStore() {
	// empty
}

RESULT ObjectStore::InitializeSceneGraphStore(OBJECT_STORE_TYPE type) {
	RESULT r = R_PASS;

	CBM((m_pSceneGraphStore == NULL), "Scene Graph Store already initialized");

	m_pSceneGraphStore = ObjectStoreFactory::MakeObjectStore(type);

Error:
	return r;
}

RESULT ObjectStore::Reset() {
	return m_pSceneGraphStore->ResetIterator();
}

RESULT ObjectStore::PushObject(VirtualObj *pObject) {
	return m_pSceneGraphStore->PushObject(pObject);
}

RESULT ObjectStore::RemoveObject(VirtualObj *pObject) {
	RESULT r = R_NOT_IMPLEMENTED;

	CN(m_pSceneGraphStore);
	CR(m_pSceneGraphStore->RemoveObject(pObject));

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

// TODO: Perhaps pass this to a scene graph handler (like physics etc)
RESULT ObjectStore::UpdateScene() {
	RESULT r = R_PASS;
	
	/*
	Reset();
	ObjectStoreImp *pObjectStore = GetSceneGraphStore();

	VirtualObj *pVirtualObj = NULL;
	while ((pVirtualObj = pObjectStore->GetNextObject()) != NULL) {
		quaternion_precision factor = 0.05;
		quaternion_precision filter = 0.1;

		static quaternion_precision x = 0;
		static quaternion_precision y = 0;
		static quaternion_precision z = 0;

		//x = ((1.0f - filter) * x) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//y = ((1.0f - filter) * y) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));
		//z = ((1.0f - filter) * z) + filter * (static_cast <color_precision> (rand()) / static_cast <color_precision> (RAND_MAX));

		pVirtualObj->RotateBy(x * factor, y * factor, z * factor);
	}
	//*/

	return r;
}

