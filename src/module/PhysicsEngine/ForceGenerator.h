#ifndef FORCE_GENERATOR_H_
#define FORCE_GENERATOR_H_

#include "RESULT/EHM.h"

// Dream Force Generator
// The virtual class defining a force generator

#include "Primitives/Types/UID.h"
#include "Primitives/vector.h"

class ObjectState;

class ForceGenerator {
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
	UID m_uid;
	bool m_fEnabled = true;
};

#endif // ! FORCE_GENERATOR_H_