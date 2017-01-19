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

// TODO: We want all of the collision groups at once - fix collision group event
RESULT CollisionResolver::Notify(CollisionGroupEvent *oEvent) {
	RESULT r = R_PASS;

	if ((oEvent->m_collisionGroup.size() <= 1))
		return R_SKIPPED;

	// We have a group of objects that have collided 
	// Resolve the point of collision for each object with the other and update it's momentum accordingly
	//std::list<VirtualObj*> affectedObjects;
	std::list<CollisionManifold> collisions;

	for (auto it = oEvent->m_collisionGroup.begin(); it != oEvent->m_collisionGroup.end(); it++) {
		DimObj *pDimObjA = dynamic_cast<DimObj*>(*it);
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

			CollisionManifold manifold = pBoundingVolumeA->Collide(pBoundingVolumeB.get());
			
			if (manifold.NumContacts() > 0) {
				collisions.push_back(manifold);
			}
		}
	}

	for (auto &manifold : collisions) {
		CR(ResolveCollision(manifold));
	}

	/*
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
		//for (auto &objB : oEvent->m_collisionGroup) {
			DimObj *pDimObjB = dynamic_cast<DimObj*>(objB);
			std::shared_ptr<BoundingVolume> pBoundingVolumeB = nullptr;

			if (pDimObjA == pDimObjB || pDimObjB == nullptr || (pBoundingVolumeB = pDimObjB->GetBoundingVolume()) == nullptr) {
				continue;
			}

			CR(ResolveCollision(pDimObjA, pDimObjB));

			/*			
			// Flag these for later
			if ((std::find(affectedObjects.begin(), affectedObjects.end(), objA) == affectedObjects.end()))
				affectedObjects.push_back(objA);

			if ((std::find(affectedObjects.begin(), affectedObjects.end(), objB) == affectedObjects.end()))
				affectedObjects.push_back(objB);
			//*/
		//}

		/*
		// Commit pending impulse changes
		for (auto &obj : affectedObjects) {
			obj->CommitPendingTranslation();
			obj->CommitPendingImpulses();
		}
		affectedObjects.clear();
		//*/
	//}

	

Error:
	return r;
}

RESULT CollisionResolver::ResolveCollision(const CollisionManifold &manifold) {
	RESULT r = R_PASS;

		// Resolve the penetration as well
		const double penetrationThreshold = 0.01f;		// Penetration percentage to correct

		VirtualObj *pObjA = manifold.GetObjectA();
		VirtualObj *pObjB = manifold.GetObjectB();

		double kgMassA = pObjA->GetMass();
		double kgMassB = pObjB->GetMass();
		
		double kgInverseMassA = 1.0f / kgMassA;
		double kgInverseMassB = 1.0f / kgMassB;

		double totalMass = kgMassA + kgMassB;

		/*else {
			const double fudgeFactor = 0.01f;		// Penetration percentage to correct
			vector vCorrection = manifold.GetNormal() * fudgeFactor;
			//vector vCorrection = manifold.GetNormal() * manifold.MaxPenetrationDepth();//  *percentCorrection;

			//pDimObjA->translate(vCorrection * -kgMassA);
			//pDimObjB->translate(vCorrection * kgMassB);
			pObjA->AddPendingTranslation(vCorrection);
			pObjB->AddPendingTranslation(vCorrection);
		}*/

		// Resolve the impulses
		vector vAngularVelocityOfPointA = pObjA->GetVelocityOfPoint(manifold.GetContactPoint());
		vector vAngularVelocityOfPointB = pObjB->GetVelocityOfPoint(manifold.GetContactPoint());

		vAngularVelocityOfPointA.w() = 0.0f;
		vAngularVelocityOfPointB.w() = 0.0f;

		vector vVelocityBeforeA = pObjA->GetVelocity();
		vector vVelocityBeforeB = pObjB->GetVelocity();

		//vector vVelocityBeforeA = vAngularVelocityOfPointA;
		//vector vVelocityBeforeB = vAngularVelocityOfPointB;

		double restitutionConstant = 0.9f;	// TODO: put into object states, then use min
		vector vRelativeVelocity = vVelocityBeforeA - vVelocityBeforeB;
		point ptRefA = pObjA->GetPointRefCenterOfMass(manifold.GetContactPoint());
		point ptRefB = pObjB->GetPointRefCenterOfMass(manifold.GetContactPoint());
		point ptContact = manifold.GetContactPoint();
		vector vRefA = (ptContact - pObjA->GetOrigin());
		vector vRefB = (ptContact - pObjB->GetOrigin());
		vector vNormal = manifold.GetNormal();

		// http://www.euclideanspace.com/physics/dynamics/collision/threed/ - For impulse equation below
		double j = -(1.0f + restitutionConstant);

		vector vRefCrossNormalA = vector(vRefA.cross(vNormal));
		vector vRefCrossNormalB = vector(vRefB.cross(vNormal));

		j *= (vRelativeVelocity.dot(vNormal)) + vRefCrossNormalA.dot(pObjA->GetAngularVelocity()) - vRefCrossNormalB.dot(pObjB->GetAngularVelocity());
		//j *= (vRelativeVelocity.dot(vNormal)) + vRefCrossNormalA.dot(vAngularVelocityOfPointA) - vRefCrossNormalB.dot(vAngularVelocityOfPointB);
		
		double denom = (kgInverseMassA + kgInverseMassB);
		double angularInertiaA = vRefCrossNormalA.dot(vector((pObjA->GetState().m_matInverseIntertiaTensor) * vRefCrossNormalA));
		double angularInertiaB = vRefCrossNormalB.dot(vector((pObjB->GetState().m_matInverseIntertiaTensor) * vRefCrossNormalB));

		denom += angularInertiaA;
		denom += angularInertiaB;

		j /= denom;

		vector vImpulse = vNormal * (j);

		vector vImpulseA = vImpulse * kgInverseMassA;
		vector vImpulseB = vImpulse * -kgInverseMassB;

		vImpulseA.w() = 0.0f;
		vImpulseB.w() = 0.0f;
		
		//vector vTorqueA = vRefA.cross(manifold.GetNormal()) * (j) * angularInertiaA;
		//vector vTorqueB = vRefB.cross(manifold.GetNormal()) * (-j) * angularInertiaB;

		vector vTorqueA = vImpulse.cross(vRefA) * (-1.0f) * angularInertiaA;
		vector vTorqueB = vImpulse.cross(vRefB) * (1.0f) * angularInertiaB;

		vTorqueA.w() = 0.0f;
		vTorqueB.w() = 0.0f;

		//vector vTorqueA = vRefA.cross(manifold.GetNormal()) * (-1.0f);// *(j)* kgInverseMassA;
		//vector vTorqueB = vRefB.cross(manifold.GetNormal()) * (1.0f);// *(-j) * kgInverseMassB;


		if (manifold.MaxPenetrationDepth() > penetrationThreshold) {
			const double percentCorrection = 1.0f - 0.30f;		// Penetration percentage to correct
			vector vCorrection = vNormal * manifold.MaxPenetrationDepth() * (percentCorrection);
			//vector vCorrection = manifold.GetNormal() * manifold.MaxPenetrationDepth();//  *percentCorrection;

			//pObjA->translate(vCorrection * -(1.0f) * kgInverseMassA);
			//pObjB->translate(vCorrection * (1.0f) * kgInverseMassB);

			pObjA->translate(vCorrection * -(1.0f));
			pObjB->translate(vCorrection * (1.0f));

			//pObjA->AddPendingTranslation(vCorrection * -kgMassA);
			//pObjB->AddPendingTranslation(vCorrection * kgMassB);

			//pObjA->AddPendingTranslation(vCorrection * -1.0f);
			//pObjB->AddPendingTranslation(vCorrection * 1.0f);
		}

		/*
		vector vImpulseA = manifold.GetNormal() * (-kgMassA / (kgInverseMassA + kgInverseMassB));
		vector vImpulseB = manifold.GetNormal() * (kgMassB / (kgInverseMassA + kgInverseMassB));

		vector vVelocityAfterA = vVelocityBeforeA * ((kgMassA - kgMassB) / totalMass) + vVelocityBeforeB * ((kgMassB * 2.0f) / totalMass);
		vector vVelocityAfterB = vVelocityBeforeB * ((kgMassA - kgMassB) / totalMass) + vVelocityBeforeA * ((kgMassA * 2.0f) / totalMass);

		pDimObjA->SetVelocity(vVelocityAfterA);
		pDimObjB->SetVelocity(vVelocityAfterB);
		*/

		//pObjA->AddPendingImpulse(vImpulseA);
		//pObjB->AddPendingImpulse(vImpulseB);
		
		// Linear
		pObjA->Impulse(vImpulseA);
		pObjB->Impulse(vImpulseB);

		pObjA->ApplyTorqueImpulse(vTorqueA);
		pObjB->ApplyTorqueImpulse(vTorqueB);
	

//Error:
	return r;
}


