#include "CollisionResolver.h"
#include "CollisionDetector.h"
#include "Primitives/DimObj.h"

#include "CollisionManifold.h"
#include "Primitives/BoundingVolume.h"

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
		std::shared_ptr<BoundingVolume> pBoundingVolumeA = nullptr;

		if (pDimObjA == nullptr || (pBoundingVolumeA = pDimObjA->GetBoundingVolume()) == nullptr) {
			continue;
		}

		for (auto &objB : oEvent->m_collisionGroup) {
			DimObj *pDimObjB = dynamic_cast<DimObj*>(objB);
			std::shared_ptr<BoundingVolume> pBoundingVolumeB = nullptr;

			if (pDimObjA == pDimObjB || pDimObjB == nullptr || (pBoundingVolumeB = pDimObjB->GetBoundingVolume()) == nullptr) {
				continue;
			}

			CR(ResolveCollision(pDimObjA, pDimObjB));
		}
	}

Error:
	return r;
}

RESULT CollisionResolver::ResolveCollision(DimObj *pDimObjA, DimObj *pDimObjB) {
	RESULT r = R_PASS;

	std::shared_ptr<BoundingVolume> pBoundingVolumeA = pDimObjA->GetBoundingVolume();
	std::shared_ptr<BoundingVolume> pBoundingVolumeB = pDimObjB->GetBoundingVolume();

	CollisionManifold manifold = pBoundingVolumeA->Collide(pBoundingVolumeB.get());

	if (manifold.NumContacts() > 0) {
		// Do that stuff
		int a = 5;
	}

//Error:
	return r;
}