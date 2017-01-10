#include "CollisionResolver.h"
#include "CollisionDetector.h"
#include "Primitives/DimObj.h"

#include "CollisionManifold.h"
#include "Primitives/BoundingVolume.h"

#include <algorithm>

CollisionResolver::CollisionResolver() {
	// empty
}

RESULT CollisionResolver::Initialize() {
	return R_PASS;
}

RESULT CollisionResolver::Notify(CollisionGroupEvent *oEvent) {
	RESULT r = R_PASS;

	if ((oEvent->m_collisionGroup.size() <= 1))
		return R_SKIPPED;

	// We have a group of objects that have collided 
	// Resolve the point of collision for each object with the other and update it's momentum accordingly
	std::list<VirtualObj*> affectedObjects;

	//for (auto &objA : oEvent->m_collisionGroup) {
	for(auto it = oEvent->m_collisionGroup.begin(); it != oEvent->m_collisionGroup.end(); it++) {
		// Resolve collision point for each object
		VirtualObj *objA = (*it);
		DimObj *pDimObjA = dynamic_cast<DimObj*>(objA);
		std::shared_ptr<BoundingVolume> pBoundingVolumeA = nullptr;

		if (pDimObjA == nullptr || (pBoundingVolumeA = pDimObjA->GetBoundingVolume()) == nullptr) {
			continue;
		}

		for (auto &objB : std::list<VirtualObj*>(it + 1, oEvent->m_collisionGroup.end())) {
			DimObj *pDimObjB = dynamic_cast<DimObj*>(objB);
			std::shared_ptr<BoundingVolume> pBoundingVolumeB = nullptr;

			if (pDimObjA == pDimObjB || pDimObjB == nullptr || (pBoundingVolumeB = pDimObjB->GetBoundingVolume()) == nullptr) {
				continue;
			}

			CR(ResolveCollision(pDimObjA, pDimObjB));

			// Flag these for later
			if ((std::find(affectedObjects.begin(), affectedObjects.end(), objA) == affectedObjects.end()))
				affectedObjects.push_back(objA);

			if ((std::find(affectedObjects.begin(), affectedObjects.end(), objB) == affectedObjects.end()))
				affectedObjects.push_back(objB);
		}
	}

	// Commit pending impulse changes
	for (auto &obj : affectedObjects) {
		obj->CommitPendingTranslation();
		obj->CommitPendingImpulses();
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
		// Resolve the penetration as well
		const double penetrationThreshold = 0.01f;		// Penetration percentage to correct

		double kgMassA = pDimObjA->GetMass();
		double kgMassB = pDimObjB->GetMass();
		double kgInverseMassA = pDimObjA->GetInverseMass();
		double kgInverseMassB = pDimObjB->GetInverseMass();
		double totalMass = kgMassA + kgMassB;

		if (manifold.MaxPenetrationDepth() > penetrationThreshold) {
			const double percentCorrection = 0.2f;		// Penetration percentage to correct
			vector vCorrection = manifold.GetNormal() * (std::max((percentCorrection - penetrationThreshold), (double)(0.0f)) / (kgInverseMassA + kgInverseMassB)) * percentCorrection;
			
			//pDimObjA->translate(vCorrection * -kgMassA);
			//pDimObjB->translate(vCorrection * kgMassB);
			//pDimObjA->AddPendingTranslation(vCorrection * -kgMassA);
			//pDimObjB->AddPendingTranslation(vCorrection * kgMassB);
			pDimObjA->AddPendingTranslation(vCorrection * -1.0f);
			pDimObjB->AddPendingTranslation(vCorrection * 1.0f);
		}

		// Resolve the impulses
		vector vVelocityBeforeA = pDimObjA->GetVelocity();
		vector vVelocityBeforeB = pDimObjB->GetVelocity();

		double restitutionConstant = 1.0f;	// TODO: put into object states, then use min
		vector vRelativeVelocity = vVelocityBeforeA - vVelocityBeforeB;
		double j = -(1.0f + restitutionConstant) * (vRelativeVelocity.dot(manifold.GetNormal()));
		j *= 1.0f / (kgInverseMassA + kgInverseMassB);

		vector vImpulseA = manifold.GetNormal() * (j);
		vector vImpulseB = manifold.GetNormal() * (-j);

		/*
		vector vImpulseA = manifold.GetNormal() * (-kgMassA / (kgInverseMassA + kgInverseMassB));
		vector vImpulseB = manifold.GetNormal() * (kgMassB / (kgInverseMassA + kgInverseMassB));

		vector vVelocityAfterA = vVelocityBeforeA * ((kgMassA - kgMassB) / totalMass) + vVelocityBeforeB * ((kgMassB * 2.0f) / totalMass);
		vector vVelocityAfterB = vVelocityBeforeB * ((kgMassA - kgMassB) / totalMass) + vVelocityBeforeA * ((kgMassA * 2.0f) / totalMass);

		pDimObjA->SetVelocity(vVelocityAfterA);
		pDimObjB->SetVelocity(vVelocityAfterB);
		*/

		pDimObjA->AddPendingImpulse(vImpulseA);
		pDimObjB->AddPendingImpulse(vImpulseB);
	}

//Error:
	return r;
}
