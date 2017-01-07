#ifndef COLLISION_RESOLVER_H_
#define COLLISION_RESOLVER_H_

#include "RESULT/EHM.h"

// Dream Collision Resolver 
// This class is responsible for resolving collisions

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"
#include "Primitives/Subscriber.h"

struct CollisionGroupEvent;
class ContactPoint;
class CollisionManifold;

class DimObj;
class BoundingVolume;

class CollisionResolver : Subscriber<CollisionGroupEvent>, public valid {
	friend class PhysicsEngine;

private:
	CollisionResolver();

protected:
	RESULT Initialize();

	virtual RESULT Notify(CollisionGroupEvent *oEvent) override;

private:
	RESULT ResolveCollision(DimObj *pDimObjA, DimObj *pDimObjB);

private:
	UID m_uid;
};

#endif	// ! COLLISION_RESOLVER_H_