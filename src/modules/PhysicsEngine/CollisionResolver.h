#ifndef COLLISION_RESOLVER_H_
#define COLLISION_RESOLVER_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine Module
// dos/src/module/PhysicsEngine/CollisionResolver.h

// Dream Collision Resolver 
// This class is responsible for resolving collisions

#include "core/types/UID.h"
#include "core/types/valid.h"
#include "core/types/Subscriber.h"

struct CollisionGroupEvent;
class ContactPoint;
class CollisionManifold;

class DimObj;
class BoundingVolume;

class CollisionResolver : Subscriber<CollisionGroupEvent>, public valid {
	friend class PhysicsEngine;
	friend class InteractionEngine;

private:
	CollisionResolver();

protected:
	RESULT Initialize();

	virtual RESULT Notify(CollisionGroupEvent *oEvent) override;

private:
	//RESULT ResolveCollision(DimObj *pDimObjA, DimObj *pDimObjB);
	RESULT ResolveCollision(const CollisionManifold &manifold);

private:
	UID m_uid;
};

#endif	// ! COLLISION_RESOLVER_H_