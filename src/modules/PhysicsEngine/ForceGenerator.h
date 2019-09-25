#ifndef FORCE_GENERATOR_H_
#define FORCE_GENERATOR_H_

#include "core/ehm/EHM.h"

// Dream Physics Engine Module
// dos/src/modules/PhysicsEngine/ForceGenerator.h

// Dream Force Generator
// The virtual class defining a force generator

#include "core/types/DObject.h"

#include "core/primitives/vector.h"

class ObjectState;

class ForceGenerator : public DObject {
	friend class PhysicsIntegrator;
	friend class ForceGeneratorFactory;

public:
	ForceGenerator() {
		// empty
	}

	~ForceGenerator() {
		// empty
	}

protected:
	virtual RESULT Update() = 0;

public:
	virtual vector GenerateForce(ObjectState *pObjectState, double startTime, double deltaTime) = 0;

	RESULT SetGeneratorState(bool fEnabled) {
		m_fEnabled = fEnabled;
		return R_PASS;
	}

	bool IsEnabled() {
		return m_fEnabled;
	}

private:
	bool m_fEnabled = true;
};

#endif // ! FORCE_GENERATOR_H_