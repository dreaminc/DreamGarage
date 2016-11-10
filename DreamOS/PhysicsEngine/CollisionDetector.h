#ifndef COLLISION_DETECTOR_H_
#define COLLISION_DETECTOR_H_

#include "RESULT/EHM.h"

// Dream Collision Detector 
// This class is responsible for detecting and reporting collisions
// to dependent classes or subscribers

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

class CollisionDetector : public valid {
private:
	CollisionDetector();

	// TODO: Do stuff

private:
	UID m_uid;
};

#endif	// ! COLLISION_DETECTOR_H_