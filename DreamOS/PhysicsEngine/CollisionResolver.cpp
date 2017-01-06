#include "CollisionResolver.h"
#include "CollisionDetector.h"
#include "Primitives/DimObj.h"

CollisionResolver::CollisionResolver() {
	// empty
}

RESULT CollisionResolver::Initialize() {
	return R_PASS;
}

RESULT CollisionResolver::Notify(CollisionGroupEvent *oEvent) {
	RESULT r = R_PASS;

	// We have a group of objects that have collided 
	// Resolve the point of collision for each object with the other and update it's momentum accordingly
	for (auto &objA : oEvent->m_collisionGroup) {
		// Resolve collision point for each object
		DimObj *pDimObjA = dynamic_cast<DimObj*>(objA);

		if (pDimObjA == nullptr || pDimObjA->GetBoundingVolume() == nullptr) {
			continue;
		}

		for (auto &objB : oEvent->m_collisionGroup) {
			DimObj *pDimObjB = dynamic_cast<DimObj*>(objB);

			if (pDimObjA == pDimObjB || pDimObjB == nullptr || pDimObjB->GetBoundingVolume() == nullptr) {
				continue;
			}

			// TODO: Resolve collision
			int a = 5;
		}
	}

//Error:
	return r;
}