#ifndef PHYSICS_INTEGRATOR_H_
#define PHYSICS_INTEGRATOR_H_

#include "RESULT/EHM.h"

// Dream Physics Integrator
// This class will integrate and take care of actual physics

#include <chrono>

#include "Primitives/Types/UID.h"
#include "Primitives/valid.h"

#include "ForceGenerator.h"
#include <list>
#include <memory>

#define MINIMUM_TIME_STEP 5
#define DEFUALT_TIMESTEP_MS 20
#define DEFAULT_GRAVITY_ACCEL -9.8f

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

protected:
	RESULT AddGlobalForceGenerator(ForceGenerator *pForceGenerator);
	RESULT ClearForceGenerators();

private:
	ObjectStore *m_pPhysicsObjectStore;		

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

	double m_sTimeStep = 0.010f;		// time step in seconds 
	double m_elapsedTime;

	// Add force generators 
	std::list<ForceGenerator*> m_globalForceGenerators;

private:
	UID m_uid;
};

#endif // ! PHYSICS_INTEGRATOR_H_