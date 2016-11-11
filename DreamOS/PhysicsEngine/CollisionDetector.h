#ifndef COLLISION_DETECTOR_H_
#define COLLISION_DETECTOR_H_

#include "RESULT/EHM.h"

// Dream Collision Detector 
// This class is responsible for detecting and reporting collisions
// to dependent classes or subscribers

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

class ObjectStore;

class CollisionDetector : public valid {
	friend class PhysicsEngine;

private:
	CollisionDetector();

protected:
	RESULT Initialize();
	RESULT UpdateObjectStore(ObjectStore *pObjectStore);

private:
	UID m_uid;
};

#endif	// ! COLLISION_DETECTOR_H_