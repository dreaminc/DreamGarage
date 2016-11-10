#ifndef COLLISION_RESOLVER_H_
#define COLLISION_RESOLVER_H_

#include "RESULT/EHM.h"

// Dream Collision Resolver 
// This class is responsible for resolving collisions

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

class CollisionResolver : public valid {
private:
	CollisionResolver();

	// TODO: Do stuff

private:
	UID m_uid;
};

#endif	// ! COLLISION_RESOLVER_H_