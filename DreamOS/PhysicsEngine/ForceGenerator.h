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

private:
	UID m_uid;
};

#endif // ! FORCE_GENERATOR_H_