#ifndef PHYSICS_INTEGRATOR_H_
#define PHYSICS_INTEGRATOR_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine Module
// dos/src/modules/PhysicsEngine/PhysicsIntegrator.h

// Dream Physics Integrator
// This class will integrate and take care of actual physics

#include <chrono>
#include <list>
#include <memory>

#include "Integrator.h"
#include "ForceGenerator.h"

#define DEFAULT_GRAVITY_ACCEL -9.8f

class ObjectStore;
class VirtualObj;

class PhysicsIntegrator : public Integrator {
	friend class PhysicsEngine;

private:
	PhysicsIntegrator();

public:
	virtual ~PhysicsIntegrator() = default;

protected:
	virtual RESULT UpdateObject(VirtualObj *pVirtualObj, double msTimeStep) override;

protected:
	RESULT AddGlobalForceGenerator(ForceGenerator *pForceGenerator);
	RESULT ClearForceGenerators();

	// Add force generators 
	std::list<ForceGenerator*> m_globalForceGenerators;
};

#endif // ! PHYSICS_INTEGRATOR_H_