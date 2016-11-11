#include "CollisionDetector.h"

#include "Scene/ObjectStore.h"

CollisionDetector::CollisionDetector() {
	// empty
}

RESULT CollisionDetector::Initialize() {
	return R_PASS;
}

RESULT CollisionDetector::UpdateObjectStore(ObjectStore *pObjectStore) {
	RESULT r = R_PASS;

	for (auto &pObject : pObjectStore->GetObjects()) {
		DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);

		if (pDimObj != nullptr) {
			pDimObj->SetColor(color(COLOR_WHITE));
		}
	}

	for (auto &objCollisionGroup : pObjectStore->GetSceneGraphStore()->GetObjectCollisionGroups()) {
		// Handle collisions

		for (auto &pObject : objCollisionGroup) {
			DimObj *pDimObj = dynamic_cast<DimObj*>(pObject);
			pDimObj->SetColor(color(COLOR_RED));
		}
	}

	//Error:
	return r;
}