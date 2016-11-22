#ifndef PHYSICS_INTEGRATOR_H_
#define PHYSICS_INTEGRATOR_H_

#include "RESULT/EHM.h"

// Dream Physics Integrator
// This class will integrate and take care of actual physics

#include <chrono>

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#define MINIMUM_TIME_STEP 5
#define DEFUALT_TIMESTEP_MS 20

class ObjectStore;

class PhysicsIntegrator : public valid {
	friend class PhysicsEngine;

private:
	PhysicsIntegrator();

protected:
	RESULT Initialize();
	RESULT Update();
	RESULT SetPhysicsStore(ObjectStore *pObjectStore);
	RESULT SetTimeStep(double msTimeStep);

private:
	ObjectStore *m_pPhysicsObjectStore;		

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

	double m_timeStep = 0.010f;
	double m_elapsedTime;

private:
	UID m_uid;
};

#endif // ! PHYSICS_INTEGRATOR_H_