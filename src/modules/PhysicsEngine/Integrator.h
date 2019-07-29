#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine Module
// dos/src/modules/PhysicsEngine/Integrator.h

// Dream Physics Integrator
// This class will integrate and take care of actual physics

#include <chrono>
#include <list>
#include <memory>

#include "core/types/UID.h"
#include "core/types/valid.h"

#include "ForceGenerator.h"

#define MINIMUM_TIME_STEP 5
#define DEFUALT_TIMESTEP_MS 20

class ObjectStore;
class VirtualObj;

class Integrator : public valid {
protected:
	Integrator();

protected:
	RESULT Initialize();
	RESULT Update();
	virtual RESULT UpdateObject(VirtualObj *pVirtualObj, double msTimeStep) = 0;
	RESULT SetObjectStore(ObjectStore *pObjectStore);
	RESULT SetTimeStep(double msTimeStep);

private:
	ObjectStore *m_pObjectStore;

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;

	double m_sTimeStep = 0.010f;		// time step in seconds 
	double m_elapsedTime;

private:
	UID m_uid;
};

#endif // ! INTEGRATOR_H_