#ifndef PHYICS_ENGINE_H_
#define PHYICS_ENGINE_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine 
// This class is combines all of the modules of the physics
// engine into one place and allows for an interface to interface 
// with them - this is effectively a controller so that the engine doesn't have
// to worry about the internals of physics

// The physics engine is made up of three main parts:
// 1. Collision Detection - This stage basically determines whether or not objects are colliding 
// and will return a set of objects that are colliding with one another.  Most of the time these
// will be collections of two objects, but we can support multiple simultaneous collisions as well
// the engine will then decide which collections to move to the next stage in the pipeline

// 2. Collision Resolution - This stage resolves the collisions into a set of collision points
// since doing this is much more of a fine grained test than that of the collision detection stage -
// the engine will then determine if these resolved collisions need to be passed on to the next stage

// 3. Physics Integrator - Combining the information from the collision resolution stage with the
// physical information of the given object such as mass, velocity, acceleration, and otherwise the
// physics integrator stage will then make the necessary adjustments to the given positions and
// attributes of the objects.  The integrator will be set at a given time delta, and should be able
// to be ramped up/down to ensure timing requirements.  

#include <memory>

#include "CollisionDetector.h"
#include "CollisionResolver.h"
#include "PhysicsIntegrator.h"

#include "core/types/UID.h"
#include "core/types/valid.h"

class ObjectStore;
class GravityGenerator;

// TODO: Turn into a module
class PhysicsEngine : public valid {
public:
	static std::unique_ptr<PhysicsEngine> MakePhysicsEngine();

private:
	PhysicsEngine();

	RESULT Initialize();

public:
	RESULT Update();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);
	RESULT SetPhysicsGraph(ObjectStore *pObjectStore);

	RESULT RegisterSubscriber(CollisionGroupEventType collisionGroupEvent, Subscriber<CollisionGroupEvent>* pCollisionDetectorSubscriber);
	RESULT RegisterObjectCollisionSubscriber(VirtualObj *pVirtualObject, Subscriber<CollisionObjectEvent>* pCollisionDetectorSubscriber);

public:
	RESULT SetGravityAccelration(float accel);
	RESULT SetGravityState(bool fEnabled);

private:
	GravityGenerator *m_pGravityForceGenerator = nullptr;

private:
	std::unique_ptr<CollisionDetector> m_pCollisionDetector;
	std::unique_ptr<CollisionResolver> m_pCollisionResolver;
	std::unique_ptr<PhysicsIntegrator> m_pPhysicsIntegrator;

private:
	UID m_uid;
};

#endif	// ! PHYSICS_ENGINE_H_